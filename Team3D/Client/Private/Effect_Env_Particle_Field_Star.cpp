#include "stdafx.h"
#include "..\Public\Effect_Env_Particle_Field_Star.h"

CEffect_Env_Particle_Field_Star::CEffect_Env_Particle_Field_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CEffect_Env_Particle_Field(pDevice, pDeviceContext)
{
}

CEffect_Env_Particle_Field_Star::CEffect_Env_Particle_Field_Star(const CEffect_Env_Particle_Field & rhs)
	: CEffect_Env_Particle_Field(rhs)
{
}

HRESULT CEffect_Env_Particle_Field_Star::NativeConstruct_Prototype(void * pArg)
{
	return __super::NativeConstruct_Prototype(pArg);
}

HRESULT CEffect_Env_Particle_Field_Star::NativeConstruct(void * pArg)
{
	return __super::NativeConstruct(pArg);
}

_int CEffect_Env_Particle_Field_Star::Tick(_double TimeDelta)
{
	Check_Culling();
	Check_State(TimeDelta);
	Check_BossFloor(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Env_Particle_Field_Star::Late_Tick(_double TimeDelta)
{
	return __super::Late_Tick(TimeDelta);
}

HRESULT CEffect_Env_Particle_Field_Star::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = (_float)m_dControl_Time;
	m_pPointInstanceCom_STT->Set_DefaultVariables();

	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fAlpha, sizeof(_float));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(9));
	m_pPointInstanceCom_STT->Render(15, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Env_Particle_Field_Star::Check_State(_double TimeDelta)
{
	if (nullptr == m_pInstanceBuffer_STT || m_IsCulling)
		return;

	switch (m_eStateValue_Next)
	{
	case CEffect_Env_Particle_Field::STATE_START:
		State_Start(TimeDelta);
		break;
	case  CEffect_Env_Particle_Field::STATE_DISAPPEAR:
		State_Disappear(TimeDelta);
		break;
	default:
		m_eStateValue_Next = STATE_DISAPPEAR;
		break;
	}
}

void CEffect_Env_Particle_Field_Star::State_Start(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fvector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fResetPosTime_Half = m_Particle_Desc.fResetPosTime / 2.f;

	if (1.0 <= m_dControl_Time)
		m_dControl_Time = 1.0;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, vWorldPos);

		if (fResetPosTime_Half < m_pInstance_Pos_UpdateTime[iIndex])
		{
			m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
			if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
				m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		}

		if (fResetPosTime_Half >= m_pInstance_Pos_UpdateTime[iIndex])
		{
			m_pInstanceBuffer_STT[iIndex].fTime -= fTimeDelta  * (_float)m_dControl_Time;
			if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
				m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		}

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vLocalPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
		vLocalPos += vDir * fTimeDelta * m_Particle_Desc.fSpeedPerSec;
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vLocalPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };

		m_pInstanceBuffer_STT[iIndex].vRight.x += (_float)TimeDelta * m_pInstanceBuffer_STT[iIndex].vRight.y;
		if (360.f < m_pInstanceBuffer_STT[iIndex].vRight.x)
			m_pInstanceBuffer_STT[iIndex].vRight.x -= 360.f;
	}
}

void CEffect_Env_Particle_Field_Star::State_Disappear(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fvector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (0.0 >= m_dControl_Time)
	{
		m_dControl_Time = 0.0;
		return;
	}

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, vWorldPos);

		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
		vPos += vDir * fTimeDelta * m_Particle_Desc.fSpeedPerSec;
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
	}
}

CEffect_Env_Particle_Field_Star * CEffect_Env_Particle_Field_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Env_Particle_Field_Star*	pInstance = new CEffect_Env_Particle_Field_Star(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Env_Particle_Field_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Env_Particle_Field_Star::Clone_GameObject(void * pArg)
{
	CEffect_Env_Particle_Field_Star* pInstance = new CEffect_Env_Particle_Field_Star(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Env_Particle_Field_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Env_Particle_Field_Star::Free()
{
	__super::Free();
}
