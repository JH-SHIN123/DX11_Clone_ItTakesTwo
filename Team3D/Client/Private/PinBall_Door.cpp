#include "stdafx.h"
#include "..\Public\PinBall_Door.h"
#include "Cody.h"
#include "UI_Generator.h"
#include "PinBall_BallDoor.h"
#include "PinBall.h"

CPinBall_Door::CPinBall_Door(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall_Door::CPinBall_Door(const CPinBall_Door & rhs)
	: CDynamic_Env(rhs)
{
}

void CPinBall_Door::Set_DoorState(_bool bState)
{
	m_bTrigger = true;
	m_bDoorState = bState;
}

HRESULT CPinBall_Door::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall_Door::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePINBALLDOOR;
	m_UserData.pGameObject = this;

	/* Trigger */
	PxGeometry* TriggerGeom = new PxSphereGeometry(0.3f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) - 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Speed(1.f, 45.f);
	XMStoreFloat3(&m_ResetPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CDataStorage::GetInstance()->Set_Pinball_Door(this);

	return S_OK;
}

_int CPinBall_Door::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CPinBall_Door::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall_Door::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPinBall_Door::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall_Door::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY && false == m_bTrigger && false == m_bDoorState && false == m_bGoal)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePINBALLDOOR, true, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		((CPinBall*)(CDataStorage::GetInstance()->Get_Pinball()))->Set_Ready();
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		UI_Delete(Cody, InputButton_InterActive);
}

void CPinBall_Door::Movement(_double dTimeDelta)
{
	if (false == m_bTrigger)
		return;

	/* Open */
	if (false == m_bDoorState)
	{
		_float	fDis = (_float)dTimeDelta;
		m_fDistance += fDis;

		if (m_fDistance >= 1.f)
		{
			((CPinBall_BallDoor*)(CDataStorage::GetInstance()->Get_Pinball_BallDoor()))->Set_DoorState(false);
			m_bTrigger = false;
			m_fDistance = 0.f;
			m_bDoorState = true;
		}
		m_pTransformCom->Go_Straight(dTimeDelta);
	}
	/* Close */
	else
	{
		//_float	fDis = (_float)dTimeDelta;
		//m_fDistance += fDis;

		//if (m_fDistance >= 1.f)
		//{
		//	//((CPinBall_BallDoor*)(CDataStorage::GetInstance()->Get_Pinball_BallDoor()))->Set_DoorState(true);
		//	m_bTrigger = false;
		//	m_fDistance = 0.f;
		//	m_bDoorState = false;
		//}
		//m_pTransformCom->Go_Straight(-dTimeDelta);

		_vector vPos = XMLoadFloat3(&m_ResetPos);
		vPos = XMVectorSetW(vPos, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_bTrigger = false;
		m_bDoorState = false;
	}

	m_pStaticActorCom->Update_StaticActor();
	m_pTriggerActorCom->Update_TriggerActor();
}

CPinBall_Door * CPinBall_Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall_Door* pInstance = new CPinBall_Door(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall_Door::Clone_GameObject(void * pArg)
{
	CPinBall_Door* pInstance = new CPinBall_Door(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall_Door::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
