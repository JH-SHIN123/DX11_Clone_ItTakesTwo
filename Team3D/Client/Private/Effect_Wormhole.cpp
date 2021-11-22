#include "stdafx.h"
#include "..\Public\Effect_Wormhole.h"
#include "GameInstance.h"

CEffect_Wormhole::CEffect_Wormhole(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Wormhole::CEffect_Wormhole(const CEffect_Wormhole & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_Wormhole::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	lstrcpy(m_EffectDesc_Prototype.ModelName, L"Test_Wormhole");

	return S_OK;
}

HRESULT CEffect_Wormhole::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	CModel_Instance::ARG_DESC Data;
	char szActorName[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, m_EffectDesc_Prototype.ModelName, MAX_PATH, szActorName, MAX_PATH, NULL, NULL);
	Data.pActorName = szActorName;
	Data.fCullingRadius = m_EffectDesc_Clone.fCullingRadius;
	Data.pWorldMatrices = m_pInstanceBuffer;
	Data.iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	for (_uint i = 0; i < Data.iInstanceCount; ++i)
	{
		XMStoreFloat4x4(&Data.pWorldMatrices[i], m_pTransformCom->Get_WorldMatrix());
		//Data.pWorldMatrices[i]._41 = _float((i % 100) * 10.f) + m_;
		//Data.pWorldMatrices[i]._43 = _float((i / 100) * 10.f);
	}
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelInstanceCom, &Data), E_FAIL);
	//
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Textrue_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);

	return S_OK;
}

_int CEffect_Wormhole::Tick(_double TimeDelta)
{
	m_fTime += (_float)TimeDelta * 0.2f;
// 	if (720.f <= m_fTime)
// 		m_fTime = 0.f;
// 
// 	m_fRadianAngle = sin(m_fTime);

	if (4.f <= m_fTime)
		m_fTime = 0.f;

	return _int();
}

_int CEffect_Wormhole::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
}

HRESULT CEffect_Wormhole::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelInstanceCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelInstanceCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(2));
	m_pModelInstanceCom->Set_DefaultVariables_Perspective();
	m_pModelInstanceCom->Render_Model(6);


	return S_OK;
}

void CEffect_Wormhole::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_Wormhole::Ready_Instance()
{
	return S_OK;
}

CEffect_Wormhole * CEffect_Wormhole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Wormhole*	pInstance = new CEffect_Wormhole(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Wormhole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Wormhole::Clone_GameObject(void * pArg)
{
	CEffect_Wormhole* pInstance = new CEffect_Wormhole(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Wormhole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Wormhole::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);
	__super::Free();
}
