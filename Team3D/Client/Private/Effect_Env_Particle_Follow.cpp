#include "stdafx.h"
#include "..\Public\Effect_Env_Particle_Follow.h"
#include "DataStorage.h"

CEffect_Env_Particle_Follow::CEffect_Env_Particle_Follow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CEffect_Env_Particle(pDevice, pDeviceContext)
{
}

CEffect_Env_Particle_Follow::CEffect_Env_Particle_Follow(const CEffect_Env_Particle_Follow & rhs)
	: CEffect_Env_Particle(rhs)
{
}

HRESULT CEffect_Env_Particle_Follow::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Env_Particle_Follow::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	m_iTextureColor_Index = 1;

	return S_OK;
}

_int CEffect_Env_Particle_Follow::Tick(_double TimeDelta)
{
	Check_Target();
	Check_State(TimeDelta);

	return _int();
}

_int CEffect_Env_Particle_Follow::Late_Tick(_double TimeDelta)
{
	return __super::Late_Tick(TimeDelta);
}

HRESULT CEffect_Env_Particle_Follow::Render(RENDER_GROUP::Enum eGroup)
{
	return __super::Render(eGroup);
}

void CEffect_Env_Particle_Follow::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle_Follow::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle_Follow::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle_Follow::Check_Target()
{
	m_IsDetected_Cody = false;
	m_IsDetected_May  = false;

	_vector vPos_Cody = DATABASE->GetCody()->Get_Position();
	_vector vPos_May  = DATABASE->GetMay()->Get_Position();

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDist_Cody = XMVector3Length(vPos_Cody - vPos).m128_f32[0];
	_float fDist_May = XMVector3Length(vPos_May - vPos).m128_f32[0];

	XMStoreFloat4(&m_vPos_Cody, vPos_Cody);
	XMStoreFloat4(&m_vPos_May, vPos_May);

	if (fDist_Cody <= m_fFollow_Distance)
		m_IsDetected_Cody = true;

	if (fDist_May <= m_fFollow_Distance)
		m_IsDetected_May = true;

	if (true == m_IsDetected_Cody || true == m_IsDetected_May)
		m_eStateValue_Next = STATE_START;
	else
		m_eStateValue_Next = STATE_DISAPPEAR;
}

void CEffect_Env_Particle_Follow::Check_State(_double TimeDelta)
{
	if (nullptr == m_pInstanceBuffer_STT)
		return;

	switch (m_eStateValue_Next)
	{
	case CEffect_Env_Particle::STATE_START:
		State_Start(TimeDelta);
		break;
	case  CEffect_Env_Particle::STATE_DISAPPEAR:
		State_Disappear(TimeDelta);
		break;
	default:
		m_eStateValue_Next = STATE_DISAPPEAR;
		break;
	}
}

void CEffect_Env_Particle_Follow::State_Start(_double TimeDelta)
{
	if (false == m_IsDetected_Cody && false == m_IsDetected_May)
	{
		m_eStateValue_Next = STATE_DISAPPEAR;
		return;
	}

	_float fTimeDelta = (_float)TimeDelta;
	_fmatrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_vector vPos_Cody = XMLoadFloat4(&m_vPos_Cody);
	_vector vPos_May = XMLoadFloat4(&m_vPos_May);

	m_dControl_Time += TimeDelta * 0.75f;
	if (1.0 <= m_dControl_Time)
		m_dControl_Time = 1.0;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		_bool IsMissing_Target = false;

		if (false == m_IsDetected_Cody && iIndex % 2 == 1)
			IsMissing_Target = true;

		if (false == m_IsDetected_May  && iIndex % 2 == 0)
			IsMissing_Target = true;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, WorldMatrix);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * 0.033f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * 0.033f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };

		if (true == IsMissing_Target)
			continue;


		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
		_vector vDir = XMVectorZero();

		if (iIndex % 2 == 1)		// Cody
			vDir = XMVector3Normalize(vPos_Cody - vPos);
		else if (iIndex % 2 == 0)	// May
			vDir = XMVector3Normalize(vPos_May - vPos);

		vPos += vDir * fTimeDelta * 0.5f;
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
	}
}

void CEffect_Env_Particle_Follow::State_Disappear(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fmatrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_vector vPos_Cody = XMLoadFloat4(&m_vPos_Cody);
	_vector vPos_May = XMLoadFloat4(&m_vPos_May);

	m_dControl_Time -= TimeDelta * 0.75f;
	if (0.0 >= m_dControl_Time)
	{
		m_dControl_Time = 0.0;
		return;
	}

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		_bool IsMissing_Target = false;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, WorldMatrix);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * 0.033f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * 0.033f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };

		m_pInstanceBuffer_STT[iIndex].fTime -= fTimeDelta  * (_float)m_dControl_Time;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
	}
}

void CEffect_Env_Particle_Follow::Reset_Instance(_int iIndex, _fmatrix WorldMatrix)
{
	m_pInstanceBuffer_STT[iIndex].vSize		 = Get_Rand_Size();
	m_pInstanceBuffer_STT[iIndex].vTextureUV = Get_TexUV_Rand(4, 2);
	m_pInstanceBuffer_STT[iIndex].fTime		 = 0.f;
	m_pInstance_Pos_UpdateTime[iIndex]		 = m_fResetPosTime;

	_vector vPos = XMLoadFloat4(&Get_Rand_Pos());
	vPos = XMVector3Transform(vPos, WorldMatrix);
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

CEffect_Env_Particle_Follow * CEffect_Env_Particle_Follow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Env_Particle_Follow*	pInstance = new CEffect_Env_Particle_Follow(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Env_Particle_Follow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Env_Particle_Follow::Clone_GameObject(void * pArg)
{
	CEffect_Env_Particle_Follow* pInstance = new CEffect_Env_Particle_Follow(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Env_Particle_Follow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Env_Particle_Follow::Free()
{
	__super::Free();
}
