#include "stdafx.h"
#include "..\Public\Effect_Boss_Laser_Explosion.h"
#include "DataStorage.h"
#include "UFO.h"

CEffect_Boss_Laser_Explosion::CEffect_Boss_Laser_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Laser_Explosion::CEffect_Boss_Laser_Explosion(const CEffect_Boss_Laser_Explosion & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Laser_Explosion::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 120;

	return S_OK;
}

HRESULT CEffect_Boss_Laser_Explosion::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Spark_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	m_pUFO = (CUFO*)DATABASE->Get_BossUFO();
	NULL_CHECK_RETURN(m_pUFO, E_FAIL);
	Safe_AddRef(m_pUFO);

	_matrix ParentMatrix = static_cast<CUFO*>(m_pUFO)->Get_Model()->Get_BoneMatrix("LaserGunRing3") * m_pUFO->Get_Transform()->Get_WorldMatrix();
	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);
	XMStoreFloat4x4(&m_ParentMatrix, ParentMatrix);
	m_pTransformCom->Set_WorldMatrix(ParentMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Laser_Explosion::Tick(_double TimeDelta)
{
	if (3.0 <= m_dControlTime)
		return EVENT_DEAD;

	m_dControlTime += TimeDelta;
	if (1.2 < m_dControlTime)
		m_IsActivate = false;

	m_dControlTime_Alpha -= TimeDelta * 0.25;
	if (0.0 > m_dControlTime_Alpha) m_dControlTime_Alpha = 0.0;

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Laser_Explosion::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Laser_Explosion::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime_Alpha;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));

	m_pPointInstanceCom_STT->Render(6, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Laser_Explosion::Check_Instance(_double TimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix ParenMatrix = XMLoadFloat4x4(&m_ParentMatrix);

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.25f;

		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
		{
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
		}

		m_pInstanceBuffer_STT[iIndex].vSize.x -= (_float)TimeDelta * 0.8f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= (_float)TimeDelta * 0.8f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;

		Instance_Pos((_float)TimeDelta, ParenMatrix, iIndex);
	}
}

void CEffect_Boss_Laser_Explosion::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Explosion::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Explosion::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Explosion::Instance_Pos(_float TimeDelta, _fmatrix ParenMatrix, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBuffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * 10.f * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	//vPos.m128_f32[1] += TimeDelta * (1.f - m_pInstanceBuffer_STT[iIndex].fTime) * 2.f * vDir.m128_f32[1];

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Boss_Laser_Explosion::Set_WorldMatrix()
{
}

void CEffect_Boss_Laser_Explosion::Reset_Instance(_double TimeDelta, _vector vPos, _int iIndex)
{
}

HRESULT CEffect_Boss_Laser_Explosion::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBuffer_Dir = new _float3[iInstanceCount];

	_float4 vPos;
	 XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	 _vector vLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.f;

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
		vRandDir = XMVector3Normalize(vRandDir + (vLookDir * 2.f));
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp, vRandDir);
		vRandDir *= (_float)(rand() % 10 + 1) / 10.f;
		XMStoreFloat3(&m_pInstanceBuffer_Dir[iIndex], vRandDir);
	}

	return S_OK;
}

_float3 CEffect_Boss_Laser_Explosion::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;

	return _float3();
}

CEffect_Boss_Laser_Explosion * CEffect_Boss_Laser_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Laser_Explosion*	pInstance = new CEffect_Boss_Laser_Explosion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Laser_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Laser_Explosion::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Laser_Explosion* pInstance = new CEffect_Boss_Laser_Explosion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Laser_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Laser_Explosion::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBuffer_Dir);

	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Release(m_pUFO);

	__super::Free();
}
