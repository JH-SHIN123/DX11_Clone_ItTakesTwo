#include "stdafx.h"
#include "Effect_GravityPipe.h"
#include "GameInstance.h"
#include "TriggerActor.h"
#include "Cody.h"
#include "May.h"

CEffect_GravityPipe::CEffect_GravityPipe(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_GravityPipe::CEffect_GravityPipe(const CEffect_GravityPipe & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_GravityPipe::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_GravityPipe::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

// 	CModel_Instance::ARG_DESC Data;
// 	char szActorName[MAX_PATH] = "";
// 	WideCharToMultiByte(CP_ACP, 0, m_EffectDesc_Prototype.ModelName, MAX_PATH, szActorName, MAX_PATH, NULL, NULL);
// 	Data.pActorName = szActorName;
// 	Data.fCullingRadius = m_EffectDesc_Clone.fCullingRadius;
// 	Data.pWorldMatrices = m_pInstanceBuffer;
// 	Data.iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
// 
// 	for (_uint i = 0; i < Data.iInstanceCount; ++i)
// 	{
// 		Data.pWorldMatrices[i] = MH_XMFloat4x4Identity();
// 		Data.pWorldMatrices[i]._41 = 10.f;
// 		Data.pWorldMatrices[i]._43 = _float((i / 100) * 10.f);
// 	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_flowmaptest", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Wormhole_Noise", TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);
	//

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransformCom), E_FAIL);

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans));
	m_pPhysxTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysxTransformCom->Set_WorldMatrix(PhysxWorldMatrix);


	CTriggerActor::ARG_DESC ArgDesc;
	m_UserData = USERDATA(GameID::eGRAVITYPIPE, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransformCom;
	ArgDesc.pGeometry = &PxCapsuleGeometry(4.f, 20.f);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CEffect_GravityPipe::Tick(_double TimeDelta)
{
	m_fTime += (_float)TimeDelta * 0.1f;

	if (3.f <= m_fTime)
		m_fTime = 0.f;


	return _int();
}

_int CEffect_GravityPipe::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
}

HRESULT CEffect_GravityPipe::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(5);

	return S_OK;
}

void CEffect_GravityPipe::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_GravityPipe::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	// May

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

HRESULT CEffect_GravityPipe::Ready_Instance()
{
	return S_OK;
}

CEffect_GravityPipe * CEffect_GravityPipe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_GravityPipe*	pInstance = new CEffect_GravityPipe(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_GravityPipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_GravityPipe::Clone_GameObject(void * pArg)
{
	CEffect_GravityPipe* pInstance = new CEffect_GravityPipe(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_GravityPipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_GravityPipe::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_ColorRamp);
	__super::Free();
}
