#include "stdafx.h"
#include "..\public\RotatedRobotBattery.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "RotatedRobotHead.h"
#include "RotatedRobotLever.h"
#include "Effect_Generator.h"

CRotatedRobotBattery::CRotatedRobotBattery(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRotatedRobotParts(pDevice, pDeviceContext)
{
}

CRotatedRobotBattery::CRotatedRobotBattery(const CRotatedRobotParts & rhs)
	: CRotatedRobotParts(rhs)
{
}

HRESULT CRotatedRobotBattery::NativeConstruct_Prototype()
{
	CRotatedRobotParts::NativeConstruct_Prototype();
	return S_OK;
}

HRESULT CRotatedRobotBattery::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(10.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotBattery"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	if (nullptr != pArg)
		memcpy(&m_tRtRobotPartsDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));

	_vector vPosition = m_tRtRobotPartsDesc.vPosition;
	vPosition.m128_f32[0] += 2.55f;
	vPosition.m128_f32[1] -= 1.44f;
	vPosition.m128_f32[2] -= 0.59f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(9.f));

	m_UserData = USERDATA(GameID::eROBOTBATTERY, this);
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

_int CRotatedRobotBattery::Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);

	if (m_bUpdate)
	{
		if (m_IsCollide && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_CODY_PUSH_BATTERY, m_fPush_Battery_Volume);
			m_pGameInstance->Play_Sound(TEXT("Cody_Push_Battery.wav"), CHANNEL_ROBOT_CODY_PUSH_BATTERY, m_fPush_Battery_Volume);
			m_bRotate = true;
			UI_Delete(Cody, InputButton_InterActive);
		}

		if (m_bRotate == true)
		{
			Push_Battery(dTimeDelta);
		}
	}

	return NO_EVENT;
}

_int CRotatedRobotBattery::Late_Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotatedRobotBattery::Render(RENDER_GROUP::Enum eGroup)
{
	CRotatedRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CRotatedRobotBattery::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (m_bUpdate)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			// 0.2f, 0.f, 0.7f
			_vector vUIPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.2f, 0.f, 0.7f, 0.f);

			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTBATTERY, true, vUIPos);
			UI_Create(Cody, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, vUIPos);
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		{
			m_IsCollide = false;
			UI_Delete(Cody, InputButton_InterActive);
		}

		// May

		//if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		//{
		//	((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTBATTERY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//	UI_Create(May, InputButton_InterActive);
		//	UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//	m_IsCollide = true;
		//}
		//else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		//{
		//	m_IsCollide = false;
		//	UI_Delete(May, InputButton_InterActive);
		//}
	}
}

HRESULT CRotatedRobotBattery::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRotatedRobotBattery::Push_Battery(_double dTimeDelta)
{
	// 나중에 스테이지에 뜨ㅏㅣ웠을때 회전하는 축 바꿔야함.
	m_fRotateDelay += (_float)dTimeDelta;

	if (m_fRotateDelay > 0.2f && m_fRotateDelay < 2.1f)
	{
		_vector vDir = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->RotateYawDirectionOnLand(vDir, -dTimeDelta * 0.005f);
		m_pStaticActorCom->Update_StaticActor();
	}
	else if (m_fRotateDelay >= 2.1f)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_BATTERY_IN, m_fBattery_Volume);
		m_pGameInstance->Play_Sound(TEXT("Battery_In.wav"), CHANNEL_ROBOT_BATTERY_IN, m_fBattery_Volume);
		EFFECT->Add_Effect(Effect_Value::RobotBattery_Spark_Rotated, m_pTransformCom->Get_WorldMatrix());

		((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_RobotHead()->Set_Battery_Charged(true);
		((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_Robot_Lever()->Set_BatteryCharged(true);
		m_IsCollide = false;
		m_fRotateDelay = 0.f;
		m_bRotate = false;
		m_bUpdate = false;
		m_pStaticActorCom->Update_StaticActor();
	}
}

CRotatedRobotBattery * CRotatedRobotBattery::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedRobotBattery* pInstance = new CRotatedRobotBattery(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRotatedRobotBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedRobotBattery::Clone_GameObject(void * pArg)
{
	CRotatedRobotBattery* pInstance = new CRotatedRobotBattery(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedRobotBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedRobotBattery::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}