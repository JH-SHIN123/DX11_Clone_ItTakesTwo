#include "stdafx.h"
#include "..\Public\HangingPlanet.h"
#include "Cody.h"
#include "UI_Generator.h"
#include "Effect_Generator.h"

CHangingPlanet::CHangingPlanet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CHangingPlanet::CHangingPlanet(const CHangingPlanet & rhs)
	: CDynamic_Env(rhs)
{
}

void CHangingPlanet::Hit_Planet(_fvector vForce)
{
	_vector vLookForce;
	vLookForce = XMVector3Normalize(vForce);
	vLookForce = XMVectorSetY(vLookForce, 0.f);
	m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLookForce) * 550000.f, 0.f, XMVectorGetZ(vLookForce) * 550000.f));
}

HRESULT CHangingPlanet::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CHangingPlanet::NativeConstruct(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	CDynamic_Env::NativeConstruct(&tArg.DynamicDesc);

	m_UserData.eID = GameID::eHANGINGPLANET;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	DATABASE->Set_HangingPlanet(this);

	return S_OK;
}

_int CHangingPlanet::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CHangingPlanet::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pDynamicActorCom->Update_DynamicActor();
	m_pTriggerActorCom->Update_TriggerActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CHangingPlanet::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CHangingPlanet::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CHangingPlanet::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::ePLANET, true, this);
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		UI_Delete(Cody, InputButton_InterActive);
}

HRESULT CHangingPlanet::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	/* Dynamic */
	PxGeometry* DynamicGeom = new PxSphereGeometry(5.f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = DynamicGeom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(DynamicGeom);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxSphereGeometry(6.f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	/* Joint */
	PxJointLimitCone LimitCone = PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f);
	m_pJoint = CPhysX::GetInstance()->Create_Joint(m_pDynamicActorCom->Get_Actor(), PxTransform(PxVec3(tArg.vOffset.x, tArg.vOffset.y, tArg.vOffset.z)), nullptr, PxTransform(PxVec3(tArg.vJointPosition.x, tArg.vJointPosition.y, tArg.vJointPosition.z)), LimitCone);

	return S_OK;
}

CHangingPlanet * CHangingPlanet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHangingPlanet* pInstance = new CHangingPlanet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CHangingPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHangingPlanet::Clone_GameObject(void * pArg)
{
	CHangingPlanet* pInstance = new CHangingPlanet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CHangingPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHangingPlanet::Free()
{
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
