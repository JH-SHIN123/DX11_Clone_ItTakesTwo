#include "stdafx.h"
#include "..\Public\Effect_Boss_Laser_Charge.h"
#include "DataStorage.h"
#include "UFO.h"

CEffect_Boss_Laser_Charge::CEffect_Boss_Laser_Charge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Laser_Charge::CEffect_Boss_Laser_Charge(const CEffect_Boss_Laser_Charge & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Laser_Charge::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 150;

	return S_OK;
}

HRESULT CEffect_Boss_Laser_Charge::NativeConstruct(void * pArg)
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

	m_pTransformCom->Set_WorldMatrix(m_pUFO->Get_Transform()->Get_WorldMatrix());

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Laser_Charge::Tick(_double TimeDelta)
{
	if (5.0 < m_dControlTime)
		return EVENT_DEAD;

	_matrix ParentMatrix = static_cast<CUFO*>(m_pUFO)->Get_Model()->Get_BoneMatrix("LaserGunRing3") * m_pUFO->Get_Transform()->Get_WorldMatrix();
	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);
	XMStoreFloat4x4(&m_ParentMatrix, ParentMatrix);
	m_pTransformCom->Set_WorldMatrix(ParentMatrix);

	m_dControlTime += TimeDelta;
	if (2.5 < m_dControlTime)
		m_IsActivate = false;

	if (true == m_IsActivate)
	{
		m_dControlTime_Alpha += TimeDelta * 0.5;
		if (1.0 <= m_dControlTime_Alpha)
			m_dControlTime_Alpha = 1.0;
	}
	else
		m_dControlTime_Alpha -= TimeDelta * 0.8;

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Laser_Charge::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Laser_Charge::Render(RENDER_GROUP::Enum eGroup)
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

void CEffect_Boss_Laser_Charge::Check_Instance(_double TimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix ParenMatrix = XMLoadFloat4x4(&m_ParentMatrix);

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, vPos, iIndex);
			continue;
		}

		Instance_Pos((_float)TimeDelta, ParenMatrix, iIndex);
	}
}

void CEffect_Boss_Laser_Charge::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Charge::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Charge::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Laser_Charge::Instance_Pos(_float TimeDelta, _fmatrix ParenMatrix, _int iIndex)
{
	m_pInstance_Parabola_Time[iIndex] = (_double)TimeDelta;

	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_LocalPos[iIndex]);// -vDir * TimeDelta * 200.f * m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime;
	_vector vDir = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vUp);

	_float fDirLength = XMVector3Length(XMLoadFloat4(&m_pInstanceBuffer_LocalPos[iIndex])).m128_f32[0]; // 지금 Pos
	_float fAlphaLength = XMVector3Length(XMLoadFloat3(&m_pInstanceBuffer_Dir[iIndex])).m128_f32[0];	// 초기 세팅됬던 Pos
	_float fAlpha = 1.f - (fDirLength / fAlphaLength);
	m_pInstanceBuffer_STT[iIndex].fTime = fAlpha;

	vPos -= vDir * (_float)TimeDelta * fDirLength;

	XMStoreFloat4(&m_pInstanceBuffer_LocalPos[iIndex], vPos);
	_vector WorldPos = XMVector3Transform(vPos, ParenMatrix);
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, WorldPos);
}

void CEffect_Boss_Laser_Charge::Set_WorldMatrix()
{	
}

void CEffect_Boss_Laser_Charge::Reset_Instance(_double TimeDelta, _vector vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].fTime = 0.0f;

	m_pInstance_Pos_UpdateTime[iIndex] = 2.f;
	m_pInstance_Parabola_Time[iIndex] = 0.0;

	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vDir = (vRandDir * 3.f);
	XMStoreFloat4(&m_pInstanceBuffer_LocalPos[iIndex], vDir);
	m_pInstanceBuffer_LocalPos[iIndex].w = 1.f;
	XMStoreFloat3(&m_pInstanceBuffer_Dir[iIndex], vDir);
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp, vRandDir);
}

HRESULT CEffect_Boss_Laser_Charge::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBuffer_Dir = new _float3[iInstanceCount];
	m_pInstance_Parabola_Time = new _double[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstanceBuffer_LocalPos = new _float4[iInstanceCount];
	m_pInstance_StartPos_Length = new _float[iInstanceCount];

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = { 0.f, 0.f, 0.f, 1.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] = 2.f * (_double(iIndex) / iInstanceCount);
		m_pInstance_Parabola_Time[iIndex] = 0.0;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
		_vector vDir = (vRandDir * 3.f);
		XMStoreFloat4(&m_pInstanceBuffer_LocalPos[iIndex], vDir);
		m_pInstanceBuffer_LocalPos[iIndex].w = 1.f;
		XMStoreFloat3(&m_pInstanceBuffer_Dir[iIndex], vDir);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp, vRandDir);

		m_pInstance_StartPos_Length[iIndex] = XMVector3Length(vDir).m128_f32[0];
	}

	return S_OK;
}

_float3 CEffect_Boss_Laser_Charge::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;

	return _float3();
}

CEffect_Boss_Laser_Charge * CEffect_Boss_Laser_Charge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Laser_Charge*	pInstance = new CEffect_Boss_Laser_Charge(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Laser_Charge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Laser_Charge::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Laser_Charge* pInstance = new CEffect_Boss_Laser_Charge(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Laser_Charge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Laser_Charge::Free()
{
	Safe_Delete_Array(m_pInstance_Parabola_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBuffer_Dir);
	Safe_Delete_Array(m_pInstanceBuffer_LocalPos);
	Safe_Delete_Array(m_pInstance_StartPos_Length);

	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Release(m_pUFO);

	__super::Free();
}
