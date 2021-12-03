#include "stdafx.h"
#include "..\Public\PinBall.h"
#include "Cody.h"
#include "PinBall_BallDoor.h"
#include "PinBall_Handle.h"
#include "Effect_Generator.h"

CPinBall::CPinBall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall::CPinBall(const CPinBall & rhs)
	: CDynamic_Env(rhs)
{
}

void CPinBall::StartGame()
{
	m_bStartGame = true;

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());

	/* 초반 레일과의 단차 보정 */
	vPosition = XMVectorSet(XMVectorGetX(vPosition), 756.9f, 189.f, 1.f);
	m_pDynamicActorCom->Get_Actor()->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
}

void CPinBall::PlayerMove()
{
	/* 메이가 조종할 때 */
	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());

	_float fX = XMVectorGetX(CDataStorage::GetInstance()->GetMay()->Get_Position()) + 1.f;
	vPosition = XMVectorSetX(vPosition, fX);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	(m_pDynamicActorCom->Get_Actor())->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));

	m_pDynamicActorCom->Update_DynamicActor();
}

void CPinBall::Goal(_fvector vGatePosition)
{
	/* 골인 */
	m_pDynamicActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(vGatePosition)));
	m_pDynamicActorCom->Get_Actor()->putToSleep();

	((CPinBall_Handle*)CDataStorage::GetInstance()->Get_Pinball_Handle())->Set_Goal();
	m_bStartGame = false;

	m_pDynamicActorCom->Update_DynamicActor();
}

void CPinBall::Respawn()
{
	_vector vPos = XMLoadFloat3(&m_RespawnPos);
	vPos = XMVectorSetW(vPos, 1.f);

	m_pDynamicActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(vPos)));
	m_pDynamicActorCom->Get_Actor()->putToSleep();
	m_bFailed = false;

	m_pDynamicActorCom->Update_DynamicActor();
}

HRESULT CPinBall::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePINBALL;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	DATABASE->Set_Pinball(this);
	XMStoreFloat3(&m_RespawnPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

_int CPinBall::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	MoveMent(dTimeDelta);

	return NO_EVENT;
}

_int CPinBall::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (true == m_bReady)
	{
		if (0 < m_pAttachBall->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}
	else
	{
		if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CPinBall::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	if (true == m_bFailed)
		return S_OK;

	if (true == m_bReady)
	{
		m_pAttachBall->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pAttachBall->Set_DefaultVariables_Shadow();
		m_pAttachBall->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);
	}
	else
	{
		m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom->Set_DefaultVariables_Shadow();
		m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);
	}

	return S_OK;
}

HRESULT CPinBall::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	if (true == m_bReady)
	{
		m_pAttachBall->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		/* Skinned: 2 / Normal: 3 */
		m_pAttachBall->Render_Model(3, 0, true);
	}

	else
	{
		m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		/* Skinned: 2 / Normal: 3 */
		m_pModelCom->Render_Model(3, 0, true);
	}

	return S_OK;
}

void CPinBall::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY && false == m_bReady)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePINBALL, true, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));

		((CPinBall_BallDoor*)(DATABASE->Get_Pinball_BallDoor()))->Set_DoorState(true);
		m_bReady = true;
	}
}

void CPinBall::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);

	/* Blocked */
	if (eStatus == ContactStatus::eFOUND && eID == GameID::Enum::eBLOCKED)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePINBALL, true, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		((CPinBall_Handle*)CDataStorage::GetInstance()->Get_Pinball_Handle())->Set_RespawnAngle(true);
		m_pDynamicActorCom->Get_Actor()->putToSleep();
		m_pDynamicActorCom->Update_DynamicActor();

		m_bFailed = true;
		m_bStartGame = false;
	}
}

void CPinBall::MoveMent(_double dTimeDelta)
{
	if (false == m_bStartGame)
		return;

	m_pDynamicActorCom->Get_Actor()->setAngularVelocity(PxVec3(-40.f, 0.f, 0.f));
	m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(0.f, 0.f, -30.f));

	if (m_pGameInstance->Key_Pressing(DIK_A)/* || m_pGameInstance->Get_Pad_LStickX() < 20000*/)
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(5.f, 0.f, -30.f));
	if (m_pGameInstance->Key_Pressing(DIK_D)/* || m_pGameInstance->Get_Pad_LStickX() > 40000*/)
		m_pDynamicActorCom->Get_Actor()->setLinearVelocity(PxVec3(-5.f, 0.f, -30.f));

	m_pDynamicActorCom->Update_DynamicActor(755.7f);
}

HRESULT CPinBall::Ready_Component(void * pArg)
{
	/* Dynamic */
	PxGeometry* DynamicGeom = new PxSphereGeometry(0.76f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = DynamicGeom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(DynamicGeom);

	m_pDynamicActorCom->Get_Actor()->setLinearDamping(10.f);
	m_pDynamicActorCom->Get_Actor()->setAngularDamping(5.f);
	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	m_pDynamicActorCom->Get_Actor()->putToSleep();

	/* Trigger */
	PxGeometry* TriggerGeom = new PxSphereGeometry(0.8f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	/* Model */
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_AttachBall"), TEXT("Com_Model_AttachBall"), (CComponent**)&m_pAttachBall), E_FAIL);

	return S_OK;
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
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pAttachBall);

	CDynamic_Env::Free();
}
