#include "stdafx.h"
#include "..\Public\PinBall_BallDoor.h"
#include "PinBall_Handle.h"
#include "PinBall.h"

CPinBall_BallDoor::CPinBall_BallDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall_BallDoor::CPinBall_BallDoor(const CPinBall_BallDoor & rhs)
	: CDynamic_Env(rhs)
{
}

void CPinBall_BallDoor::Set_DoorState(_bool bDoorState)
{
	m_bReady = true;
	m_bDoorState = bDoorState;
}

HRESULT CPinBall_BallDoor::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall_BallDoor::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	DATABASE->Set_Pinball_BallDoor(this);

	return S_OK;
}

_int CPinBall_BallDoor::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	MoveMent(dTimeDelta);

	return NO_EVENT;
}

_int CPinBall_BallDoor::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall_BallDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	if (false == m_bReady && true == m_bDoorState)
		return S_OK;

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPinBall_BallDoor::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall_BallDoor::MoveMent(_double dTimeDelta)
{
	if (false == m_bReady)
		return;

	/* Open */
	if (false == m_bDoorState)
	{
		_float	fAngle = 100.f * (_float)dTimeDelta;
		m_fAngle += fAngle;

		if (m_fAngle >= 100.f)
		{
			((CPinBall*)(DATABASE->Get_Pinball()))->Set_TriggerCheck();
			m_bReady = false;
		}

		m_pTransformCom->RotatePitch_Angle(dTimeDelta, 100.f);
	}
	/* Close */
	else
	{
		_float	fAngle = 100.f * -(_float)dTimeDelta;
		m_fAngle += fAngle;

		if (m_fAngle <= 0.f)
		{
			m_bReady = false;

			/* Sound */
			m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_HANDLE);
			m_pGameInstance->Play_Sound(TEXT("Pinball_Shooter_SetStartPosition.wav"), CHANNEL_PINBALL_HANDLE);

			((CPinBall_Handle*)(DATABASE->Get_Pinball_Handle()))->Set_Ready(true);
			((CPinBall*)(DATABASE->Get_Pinball()))->Set_Ready(true);
		}
		m_pTransformCom->RotatePitch_Angle(-dTimeDelta, 100.f);
	}

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CPinBall_BallDoor::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CPinBall_BallDoor * CPinBall_BallDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall_BallDoor* pInstance = new CPinBall_BallDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall_BallDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall_BallDoor::Clone_GameObject(void * pArg)
{
	CPinBall_BallDoor* pInstance = new CPinBall_BallDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall_BallDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall_BallDoor::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
