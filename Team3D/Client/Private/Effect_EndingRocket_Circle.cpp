#include "stdafx.h"
#include "..\Public\Effect_EndingRocket_Circle.h"
#include "DataStorage.h"
#include "EndingRocket.h"

CEffect_EndingRocket_Circle::CEffect_EndingRocket_Circle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_EndingRocket_Circle::CEffect_EndingRocket_Circle(const CEffect_EndingRocket_Circle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_EndingRocket_Circle::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 1;
	return S_OK;
}

HRESULT CEffect_EndingRocket_Circle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

// 	Check_TargetMatrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_EndingRocket_Circle::Tick(_double TimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	if (true == static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Boost())
		m_IsBoosting = true;


	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (0.7 <= m_dControlTime) m_dControlTime = 0.70;
	}
	else
	{
		m_dControlTime -= TimeDelta * 0.1;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	if (true == m_IsBoosting)
	{
		m_fBoostTime += (_float)TimeDelta;
		if (2.f < m_fBoostTime)
		{
			m_fBoostTime = 0.f;
			m_IsBoosting = false;
		}
	}


	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_EndingRocket_Circle::Late_Tick(_double TimeDelta)
{
	//Check_TargetMatrix();
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_EndingRocket_Circle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	_float4 vColor = { 1.000000000f, 0.537254965f, 0.235294187f, 1.000000000f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_Variable("g_vColor", &vColor, sizeof(_float4));

	m_pPointInstanceCom_STT->Render(24, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_EndingRocket_Circle::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_EndingRocket_Circle::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_int iInstanceCount_Max = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount_Max; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_EndingRocket_Circle::Instance_Size(_float TimeDelta, _int iIndex)
{

}

void CEffect_EndingRocket_Circle::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_EndingRocket_Circle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_EndingRocket_Circle::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
}

HRESULT CEffect_EndingRocket_Circle::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(8, 8, true).z;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, WorldMatrix.r[3]);
	}

	return S_OK;
}

void CEffect_EndingRocket_Circle::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Transform()->Get_WorldMatrix();

	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);

	WorldMatrix.r[3] += WorldMatrix.r[2] * 0.2f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

_matrix CEffect_EndingRocket_Circle::Rotate_WorldMatrix(_fmatrix WorldMatrix, _float fDegree, CTransform::STATE eState)
{
	return XMMatrixRotationAxis(WorldMatrix.r[eState], XMConvertToRadians(fDegree));
}

CEffect_EndingRocket_Circle * CEffect_EndingRocket_Circle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_EndingRocket_Circle*	pInstance = new CEffect_EndingRocket_Circle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_EndingRocket_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_EndingRocket_Circle::Clone_GameObject(void * pArg)
{
	CEffect_EndingRocket_Circle* pInstance = new CEffect_EndingRocket_Circle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_EndingRocket_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_EndingRocket_Circle::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
