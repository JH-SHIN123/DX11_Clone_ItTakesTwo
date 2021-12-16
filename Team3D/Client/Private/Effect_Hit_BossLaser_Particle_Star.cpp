#include "stdafx.h"
#include "..\Public\Effect_Hit_BossLaser_Particle_Star.h"
#include "DataStorage.h"
#include "Cody.h"
#include "May.h"

CEffect_Hit_BossLaser_Particle_Star::CEffect_Hit_BossLaser_Particle_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Hit_BossLaser_Particle_Star::CEffect_Hit_BossLaser_Particle_Star(const CEffect_Hit_BossLaser_Particle_Star & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Hit_BossLaser_Particle_Star::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 20;

	return S_OK;
}

HRESULT CEffect_Hit_BossLaser_Particle_Star::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Star"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(Normalize_Matrix(XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix)));

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Hit_BossLaser_Particle_Star::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 2.0 < m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;
	if (1.0 < m_dActivateTime)
		m_IsActivate = false;

	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (1.0 < m_dControlTime) m_dControlTime = 1.0;
	}
	else
	{
		m_dControlTime -= TimeDelta;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	m_fInstance_SpeedPerSec -= (_float)TimeDelta * 1.05f;
	if (0.f > m_fInstance_SpeedPerSec)  m_fInstance_SpeedPerSec = 0.f;

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Hit_BossLaser_Particle_Star::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Hit_BossLaser_Particle_Star::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(9));

	m_pPointInstanceCom_STT->Render(13, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

_float2 CEffect_Hit_BossLaser_Particle_Star::Get_RandSize()
{
	_float2 vRandSize;
	vRandSize.y = (_float)(rand() % 3 + 1);
	vRandSize.x = vRandSize.y * 0.75f;

	return vRandSize;
}

void CEffect_Hit_BossLaser_Particle_Star::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.5f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
			continue;

		m_pInstanceBuffer_STT[iIndex].vSize.x -= (_float)TimeDelta * 0.35f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= (_float)TimeDelta * 0.35f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;

		Instance_Pos((_float)TimeDelta, iIndex);
	}
}

void CEffect_Hit_BossLaser_Particle_Star::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Hit_BossLaser_Particle_Star::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * 4.f * (m_fInstance_SpeedPerSec * m_fInstance_SpeedPerSec);

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Hit_BossLaser_Particle_Star::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Hit_BossLaser_Particle_Star::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
}

HRESULT CEffect_Hit_BossLaser_Particle_Star::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, WorldMatrix.r[3]);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand_Matrix(_int3(100, 100, 100), WorldMatrix));
		XMStoreFloat3(&m_pInstance_Dir[iIndex], vRandDir);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp, vRandDir);

		if (m_pInstance_Dir[iIndex].y < -0.5f)
			m_pInstance_Dir[iIndex].y *= -1.f;
	}

	return S_OK;
}

_float3 CEffect_Hit_BossLaser_Particle_Star::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;

	return _float3();
}

CEffect_Hit_BossLaser_Particle_Star * CEffect_Hit_BossLaser_Particle_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Hit_BossLaser_Particle_Star*	pInstance = new CEffect_Hit_BossLaser_Particle_Star(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Hit_BossLaser_Particle_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Hit_BossLaser_Particle_Star::Clone_GameObject(void * pArg)
{
	CEffect_Hit_BossLaser_Particle_Star* pInstance = new CEffect_Hit_BossLaser_Particle_Star(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Hit_BossLaser_Particle_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Hit_BossLaser_Particle_Star::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstance_Dir);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
