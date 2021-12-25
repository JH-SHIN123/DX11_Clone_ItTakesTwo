#include "stdafx.h"
#include "..\public\RobotLever.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "RobotHead.h"
#include "NoBatterySign.h"
#include"CutScenePlayer.h"
CRobotLever::CRobotLever(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRobotParts(pDevice, pDeviceContext)
{
}

CRobotLever::CRobotLever(const CRobotParts & rhs)
	: CRobotParts(rhs)
{
}

HRESULT CRobotLever::NativeConstruct_Prototype()
{
	CRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobotLever::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RealRobotLever"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	if (nullptr != pArg)
		memcpy(&m_tRobotPartsDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	_vector vPosition = m_tRobotPartsDesc.vPosition;
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 1.2f);
	vPosition = XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) - 1.7f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	// 로봇 OffSet XMVectorSet(15.f, 0.f, 20.f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-70.f));

	m_UserData = USERDATA(GameID::eROBOTLEVER, this);
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);
	
	CTriggerActor::ARG_DESC TriggerArgDesc;
	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.7f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);


	return S_OK;
}

_int CRobotLever::Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);

	if (m_bUpdate == true)
	{
		if (m_IsCodyCollide && m_pGameInstance->Key_Down(DIK_E))
		{
			UI_Delete(Cody, InputButton_InterActive);
			m_IsCodyCollide = false;
			m_bRotate = true;
			m_bCountHitDelay = true;
		}

		if (m_IsMayCollide && m_pGameInstance->Key_Down(DIK_O)	|| m_IsMayCollide && m_pGameInstance->Pad_Key_Down(DIP_Y))
		{
			UI_Delete(May, InputButton_PS_InterActive);
			m_IsMayCollide = false;
			m_bRotate = true;
			m_bCountHitDelay = true;
		}

		if (m_bRotate == true)
		{
			Activate_Lever(dTimeDelta);
		}
	}

	return NO_EVENT;
}

_int CRobotLever::Late_Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRobotLever::Render(RENDER_GROUP::Enum eGroup)
{
	CRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CRobotLever::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (m_bUpdate == true)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTLEVER, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			UI_Create(Cody, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
			m_IsCodyCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		{
			m_IsCollide = false;
			UI_Delete(Cody, InputButton_InterActive);
			m_IsCodyCollide = false;
		}

		// May
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		{
			((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTLEVER, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			UI_Create(May, InputButton_PS_InterActive);
			UI_Generator->Set_TargetPos(Player::May, UI::InputButton_PS_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsMayCollide = true;
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		{
			m_IsCollide = false;
			m_IsMayCollide = false;
			UI_Delete(May, InputButton_PS_InterActive);
		}
	}
}

HRESULT CRobotLever::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRobotLever::Activate_Lever(_double dTimeDelta)
{
	// 만약 배터리가 없다면? 오른쪽 갔다가 0.5초 후에 다시 왼쪽으로 돌아와.
	if (m_bBatteryCharged == false)
	{

		m_fStopDelay += (_float)dTimeDelta;
		if (m_fStopDelay > 0.2f && m_fStopDelay <= 0.6f)
		{
			if (m_bSoundOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_LEVER, m_fLeverVolume);
				m_pGameInstance->Play_Sound(TEXT("Robot_Lever.wav"), CHANNEL_ROBOT_LEVER, m_fLeverVolume);
				m_bSoundOnce = true;
			}
			_vector vDir = XMVector3Normalize((XMVectorSet(-1.f, 0.f, 0.f, 0.f) + XMVectorSet(0.f, 0.f, -1.f, 0.f) * 2.f));
			m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);

			if (m_bNoBatteryHit == false)
			{
				switch (m_tRobotPartsDesc.iStageNum)
				{
				case ST_GRAVITYPATH:
					((CRobotParts*)DATABASE->Get_STGravityRobot())->Get_RobotHead()->Set_Lever_Hit_When_NoBattery(true);
					((CRobotParts*)DATABASE->Get_STGravityRobot())->Get_NoBatterySign()->Set_HitLever(true);
					break;
				case ST_RAIL:
					((CRobotParts*)DATABASE->Get_STPlanetRobot())->Get_RobotHead()->Set_Lever_Hit_When_NoBattery(true);
					((CRobotParts*)DATABASE->Get_STPlanetRobot())->Get_NoBatterySign()->Set_HitLever(true);
					break;
				}
				
				m_bNoBatteryHit = true;
			}
		}
		else if (m_fStopDelay > 1.f && m_fStopDelay <= 1.4f)
		{
			if (m_bSoundBackOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_LEVER_BACK, m_fLeverBackVolume);
				m_pGameInstance->Play_Sound(TEXT("Robot_Lever_Back.wav"), CHANNEL_ROBOT_LEVER_BACK, m_fLeverBackVolume);
				m_bSoundBackOnce = true;
			}
			_vector vDir = XMVector3Normalize((XMVectorSet(-1.f, 0.f, 0.1f, 0.f) + XMVectorSet(0.f, 0.f, -1.f, 0.f) / 8.f));
			m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);
		}
		else if (m_fStopDelay > 1.4f)
		{
			m_bSoundOnce = false;
			m_bSoundBackOnce = false;
			m_bNoBatteryHit = false;
			m_fStopDelay = 0.f;
			m_bRotate = false;
		}
	}
	else if (m_bBatteryCharged == true)
	{
		if (m_tRobotPartsDesc.iStageNum == ST_GRAVITYPATH)
		{
#ifdef __PLAY_CUTSCENE
			if (false == CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Active_GravityPath_01) )
			{
				CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Active_GravityPath_01"));
				CCutScenePlayer::GetInstance()->Set_IsCutScenePlayer(CCutScene::CutSceneOption::CutScene_Active_GravityPath_01, true);
			}
#endif
			DATABASE->Set_GravityStageClear(true);
		}
		else if (m_tRobotPartsDesc.iStageNum == ST_RAIL)
		{
#ifdef __PLAY_CUTSCENE
			if (false == CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Clear_Rail))
			{
				CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Clear_Rail"));
				CCutScenePlayer::GetInstance()->Set_IsCutScenePlayer(CCutScene::CutSceneOption::CutScene_Clear_Rail, true);
			}
#endif
			DATABASE->Set_RailStageClear(true);
		}

		m_fStopDelay += (_float)dTimeDelta;
		if (m_fStopDelay > 0.2f && m_fStopDelay <= 0.6f)
		{
			if (m_bSoundOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_LEVER, m_fLeverVolume);
				m_pGameInstance->Play_Sound(TEXT("Robot_Lever.wav"), CHANNEL_ROBOT_LEVER, m_fLeverVolume);
				m_bSoundOnce = true;
			}
			_vector vDir = XMVector3Normalize((XMVectorSet(-1.f, 0.f, 0.f, 0.f) + XMVectorSet(0.f, 0.f, -1.f, 0.f) * 2.f));
			m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);
			switch (m_tRobotPartsDesc.iStageNum)
			{
			case ST_GRAVITYPATH:
				((CRobotParts*)DATABASE->Get_STGravityRobot())->Get_RobotHead()->Set_Lever_Active(true);
				((CRobotParts*)DATABASE->Get_STGravityRobot())->Get_NoBatterySign()->Set_BatteryCharged(true);
				break;
			case ST_RAIL:
				((CRobotParts*)DATABASE->Get_STPlanetRobot())->Get_RobotHead()->Set_Lever_Active(true);
				((CRobotParts*)DATABASE->Get_STPlanetRobot())->Get_NoBatterySign()->Set_BatteryCharged(true);
				break;
			}
		}
		else if (m_fStopDelay > 0.6f)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_ACTIVE, m_fRobotActiveVolume);
			m_pGameInstance->Play_Sound(TEXT("Robot_Active.wav"), CHANNEL_ROBOT_ACTIVE, m_fRobotActiveVolume);

			m_bNoBatteryHit = false;
			m_fStopDelay = 0.f;
			m_bRotate = false;
			m_bUpdate = false;
		}
	}
}

CRobotLever * CRobotLever::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobotLever* pInstance = new CRobotLever(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRobotLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobotLever::Clone_GameObject(void * pArg)
{
	CRobotLever* pInstance = new CRobotLever(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobotLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobotLever::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}