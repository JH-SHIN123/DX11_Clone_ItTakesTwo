#include "stdafx.h"
#include "..\Public\PinBall.h"

CPinBall::CPinBall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall::CPinBall(const CPinBall & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPinBall::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePinBall;
	m_UserData.pGameObject = this;

	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = &PxSphereGeometry(1.f);
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	m_pDynamicActorCom->Get_Actor()->setLinearDamping(10.f);
	m_pDynamicActorCom->Get_Actor()->setAngularDamping(5.f);

	CTriggerActor::ARG_DESC tTriggerActorArg;
	tTriggerActorArg.pTransform = m_pTransformCom;
	tTriggerActorArg.pGeometry = &PxSphereGeometry(1.5f);
	tTriggerActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerActorArg), E_FAIL);

	return S_OK;
}

_int CPinBall::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	MoveMent();
	m_pDynamicActorCom->Update_DynamicActor();
	m_pTriggerActorCom->Update_TriggerActor();

	return NO_EVENT;
}

_int CPinBall::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPinBall::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CPinBall::MoveMent()
{
	m_pDynamicActorCom->Get_Actor()->setAngularVelocity(PxVec3(20.f, 0.f, 0.f));

	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(-5.f, 0.f, 0.f));
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(5.f, 0.f, 0.f));
	if (m_pGameInstance->Key_Pressing(DIK_UP))
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(0.f, 0.f, 5.f));
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(0.f, 0.f, -5.f));
}

CPinBall * CPinBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall* pInstance = new CPinBall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall::Clone_GameObject(void * pArg)
{
	CPinBall* pInstance = new CPinBall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pDynamicActorCom);

	CDynamic_Env::Free();
}
