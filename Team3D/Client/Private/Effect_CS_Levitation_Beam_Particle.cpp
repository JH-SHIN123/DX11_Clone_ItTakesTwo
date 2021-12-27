#include "stdafx.h"
#include "..\Public\Effect_CS_Levitation_Beam_Particle.h"
#include "DataStorage.h"

CEffect_CS_Levitation_Beam_Particle::CEffect_CS_Levitation_Beam_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_CS_Levitation_Beam_Particle::CEffect_CS_Levitation_Beam_Particle(const CEffect_CS_Levitation_Beam_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_CS_Levitation_Beam_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 50;
	return S_OK;
}

HRESULT CEffect_CS_Levitation_Beam_Particle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Spark_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_CS_Levitation_Beam_Particle::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 5.0 < m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;
	if (10.0 < m_dActivateTime)
		return EVENT_DEAD;

	if (true == m_IsActivate)
	{
		m_dAlphaTime += TimeDelta * 0.5f;
		if (1.0 <= m_dAlphaTime) m_dAlphaTime = 1.0;
	}
	else
	{
		m_dAlphaTime -= TimeDelta * 0.5f;
		if (0.0 >= m_dAlphaTime) m_dAlphaTime = 0.0;
	}

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_CS_Levitation_Beam_Particle::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_CS_Levitation_Beam_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dAlphaTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));

	m_pPointInstanceCom_STT->Render(11, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_CS_Levitation_Beam_Particle::Check_Instance(_double TimeDelta)
{
	_fmatrix ParentMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.25f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)	m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, m_pTransformCom->Get_WorldMatrix(), iIndex);
			continue;
		}

		m_pInstanceBuffer_STT[iIndex].vSize.x -= (_float)TimeDelta * 0.06f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= (_float)TimeDelta * 0.5f;
		if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
		if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;

		Instance_Pos((_float)TimeDelta, iIndex);

	}
}

void CEffect_CS_Levitation_Beam_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_Levitation_Beam_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition.y += TimeDelta * 2.f;
}

void CEffect_CS_Levitation_Beam_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_Levitation_Beam_Particle::Reset_Instance(_double TimeDelta, _fmatrix ParentMatrix, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].fTime = 0.5f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

	_vector vRandPos = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 2, 100)));
	vRandPos.m128_f32[1] = 0.f;
	_float fRandPower = ((_float)(rand() % 10) + 0.5f) * 0.05f;
	vRandPos *= fRandPower;

	vRandPos = XMVector3Transform(vRandPos, ParentMatrix);
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vRandPos);
}

HRESULT CEffect_CS_Levitation_Beam_Particle::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_matrix ParentMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix NormalizeMatrix = __super::Normalize_Matrix(ParentMatrix);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f, 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex] = (_double(iIndex) / iInstanceCount);


		_vector vRandPos = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 2, 100)));
		vRandPos.m128_f32[1] = 0.f;
		_float fRandPower = ((_float)(rand() % 10) + 0.5f) * 0.05f;
		vRandPos *= fRandPower;

		vRandPos = XMVector3Transform(vRandPos, ParentMatrix);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vRandPos);
	}

	return S_OK;
}

CEffect_CS_Levitation_Beam_Particle * CEffect_CS_Levitation_Beam_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_CS_Levitation_Beam_Particle*	pInstance = new CEffect_CS_Levitation_Beam_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_CS_Levitation_Beam_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_CS_Levitation_Beam_Particle::Clone_GameObject(void * pArg)
{
	CEffect_CS_Levitation_Beam_Particle* pInstance = new CEffect_CS_Levitation_Beam_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_CS_Levitation_Beam_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_CS_Levitation_Beam_Particle::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
