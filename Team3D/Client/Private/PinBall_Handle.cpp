#include "stdafx.h"
#include "..\Public\PinBall_Handle.h"
#include "..\Public\PinBall_HandleBase.h"
#include "PinBall_Spring.h"
#include "PinBall.h"
#include "May.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "PinBall_Door.h"
#include "SlideDoor.h"

CPinBall_Handle::CPinBall_Handle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall_Handle::CPinBall_Handle(const CPinBall_Handle & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPinBall_Handle::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall_Handle::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePINBALLHANDLE;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(0.5f, 0.f);
	m_fRespawnPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	DATABASE->Set_Pinball_Handle(this);

	return S_OK;
}

_int CPinBall_Handle::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if (true == m_bGoal)
	{
		m_fGoalTime += (_float)dTimeDelta;
		if (3.f <= m_fGoalTime && false == m_bGoalTimeCheck)
		{
			m_bGoalTimeCheck = true;
			m_bRespawnAngle = true;
			((CPinBall*)(DATABASE->Get_Pinball()))->Set_Failed();
			((CCody*)(DATABASE->GetCody()))->PinBall_Respawn(dTimeDelta);
		}
		Respawn_Angle(dTimeDelta);
		Respawn_Pos(dTimeDelta);
	}
	else
	{
		MoveMent(dTimeDelta);
		PlayerMove();
		Respawn_Angle(dTimeDelta);
		Respawn_Pos(dTimeDelta);
	}

	UI_Generator->CreateInterActiveUI_AccordingRange(Player::May, UI::PinBall_Handle, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f, m_IsCollision);

	return NO_EVENT;
}

_int CPinBall_Handle::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall_Handle::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPinBall_Handle::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall_Handle::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// May 병합할 때 여기 주의
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY && false == m_bPlayerMove)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::ePINBALLHANDLE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::ePINBALLHANDLE, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollision = false;
	}
}

void CPinBall_Handle::MoveMent(_double dTimeDelta)
{
	if (false == m_bReady)
		return;

	if (false == m_bHandleReady)
	{
		m_fReady += (_float)dTimeDelta;
		m_pTransformCom->Go_Down(dTimeDelta);

		if (m_fReady >= 0.5f)
		{
			m_fReady = 0.f;
			m_bHandleReady = true;
		}
	}
	else
	{
		_float	fAngle = 90.f * (_float)dTimeDelta;
		m_fHandleAngle += fAngle;

		if (m_fHandleAngle >= 90.f)
		{
			m_fHandleAngle = 0.f;
			m_bHandleReady = false;
			m_bReady = false;
			m_bFinish = true;
		}
		m_pTransformCom->RotatePitch_Angle(dTimeDelta, 90.f);
	}
	m_pStaticActorCom->Update_StaticActor();
}

void CPinBall_Handle::PlayerMove()
{
	if (true == m_bPlayerMove)
	{
		_float fX = XMVectorGetX(CDataStorage::GetInstance()->GetMay()->Get_Position()) + 1.f;
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetX(vPos, fX);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pStaticActorCom->Update_StaticActor();

		((CPinBall_HandleBase*)(DATABASE->Get_Pinball_HandleBase()))->PlayerMove();
		((CPinBall_Spring*)(DATABASE->Get_Pinball_Spring()))->PlayerMove();
		((CPinBall*)(DATABASE->Get_Pinball()))->PlayerMove();
	}
}

void CPinBall_Handle::Respawn_Angle(_double dTimeDelta)
{
	if (true == m_bRespawnAngle)
	{
		_float	fAngle = 90.f * (_float)dTimeDelta;
		m_fHandleAngle += fAngle;

		if (m_fHandleAngle >= 90.f)
		{
			m_fHandleAngle = 0.f;
			m_bRespawnAngle = false;
			m_bRespawnAngleToMove = true;
		}
		m_pTransformCom->RotatePitch_Angle(dTimeDelta, -90.f);
		m_pStaticActorCom->Update_StaticActor();
	}

	if (true == m_bRespawnAngleToMove)
	{
		m_fReady += (_float)dTimeDelta;
		m_pTransformCom->Go_Up(dTimeDelta);

		if (m_fReady >= 0.5f)
		{
			m_bRespawnAngleToMove = false;
			m_bHandleReady = false;
			m_bReady = false;
			m_fReady = 0.f;
			m_bRespawnPos = true;
			m_bFinish = false;
		}
		m_pStaticActorCom->Update_StaticActor();
	}
}

void CPinBall_Handle::Respawn_Pos(_double dTimeDelta)
{
	if (true == m_bRespawnPos)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + (3.f * (_float)dTimeDelta));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pStaticActorCom->Update_StaticActor();

		((CPinBall_HandleBase*)(DATABASE->Get_Pinball_HandleBase()))->Respawn_Pos(dTimeDelta);
		((CPinBall_Spring*)(DATABASE->Get_Pinball_Spring()))->Respawn_Pos(dTimeDelta);

		if (0.1f >= (m_fRespawnPosX - XMVectorGetX(vPos)))
		{
			vPos = XMVectorSetX(vPos, m_fRespawnPosX);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pStaticActorCom->Update_StaticActor();

			m_bRespawnPos = false;
			m_bReady = false;

			((CPinBall*)(DATABASE->Get_Pinball()))->Respawn();
			((CPinBall_Door*)(DATABASE->Get_Pinball_Door()))->Set_DoorState(true);

			if (true == m_bGoalTimeCheck)
			{
				m_bGoal = false;
				((CPinBall_Door*)(DATABASE->Get_Pinball_Door()))->Set_Goal();
				((CSlideDoor*)(DATABASE->Get_SlideDoor()))->Open_Door();
			}
			else
				((CCody*)(DATABASE->GetCody()))->PinBall_Respawn(dTimeDelta);
		}
	}
}

HRESULT CPinBall_Handle::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* geom = new PxSphereGeometry(2.f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(geom);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetX(vPosition, XMVectorGetX(vPosition) - 2.f);
	m_pTriggerActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(vPosition)));


	return S_OK;
}

CPinBall_Handle * CPinBall_Handle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall_Handle* pInstance = new CPinBall_Handle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall_Handle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall_Handle::Clone_GameObject(void * pArg)
{
	CPinBall_Handle* pInstance = new CPinBall_Handle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall_Handle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall_Handle::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
