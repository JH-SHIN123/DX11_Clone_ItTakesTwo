
#include "stdafx.h"
#include "..\Public\May.h"
#include "SubCamera.h"
#include "UI_Generator.h"
#include "UIObject.h"
#include "Cody.h"
#include "PlayerActor.h"
#include "SpaceRail.h"
#include "SpaceRail_Node.h"
#include "HookUFO.h"
#include "Gauge_Circle.h"
#include"CutScenePlayer.h"
#include "Effect_Generator.h"
#include "Effect_May_Boots.h"
/* For.PinBall */
#include "PinBall_Handle.h"
#include "PinBall_Blocked.h"
#include "PinBall.h"
/* For.Tube*/
#include "HookahTube.h"
/* For.MoonUFO */
#include "MoonUFO.h"
#include "Moon.h"
/* For.BossUFO */
#include "UFO.h"

/* For.UI */
#include "HpBar.h"
#include "MinigameHpBar.h"

/*For.WarpGate*/
#include "WarpGate.h"
#include "LaserTennis_Manager.h"
#include "Script.h"

// m_pGameInstance->Get_Pad_LStickX() > 44000 (Right)
// m_pGameInstance->Get_Pad_LStickX() < 20000 (Left)
// m_pGameInstance->Get_Pad_LStickY() < 20000 (Down)
// m_pGameInstance->Get_Pad_LStickY() > 44000 (Up)

CMay::CMay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCharacter(pDevice, pDeviceContext)
{
}

CMay::CMay(const CMay& rhs)
	: CCharacter(rhs)
{
}

HRESULT CMay::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}

HRESULT CMay::NativeConstruct_Prototype()
{
	CCharacter::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMay::NativeConstruct(void* pArg)
{
	CCharacter::NativeConstruct(pArg);
	Ready_Component();

	m_pModelCom->Set_Animation(ANI_M_Bounce4);
	CDataStorage::GetInstance()->Set_MayPtr(this);
	Add_LerpInfo_To_Model();

	FAILED_CHECK_RETURN(Ready_UI(), E_FAIL);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
	m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RUN, m_fMay_Run_Volume);
	m_pGameInstance->Play_Sound(TEXT("May_Run.wav"), CHANNEL_MAY_RUN, m_fMay_Run_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fMay_Wall_Slide_Volume);
	m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fMay_Wall_Slide_Volume);


	m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
	m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
	m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);

	return S_OK;
}

HRESULT CMay::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_May"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	CTransform::TRANSFORM_DESC PlayerTransformDesc;
	PlayerTransformDesc.dRotationPerSec = XMConvertToRadians(720.f);
	PlayerTransformDesc.dSpeedPerSec = 2.f;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &PlayerTransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.7f, 30.f, 1.f)); // 대충 시작지점
	CPlayerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eMAY, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.fJumpGravity = -50.f;

	ArgDesc.CapsuleControllerDesc.setToDefault();
	ArgDesc.CapsuleControllerDesc.height = 0.5f;
	ArgDesc.CapsuleControllerDesc.radius = 0.5f;
	ArgDesc.CapsuleControllerDesc.material = m_pGameInstance->Get_BasePxMaterial();
	ArgDesc.CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	ArgDesc.CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	ArgDesc.CapsuleControllerDesc.contactOffset = 0.02f;
	ArgDesc.CapsuleControllerDesc.stepOffset = 0.5f;
	ArgDesc.CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	ArgDesc.CapsuleControllerDesc.slopeLimit = 0.707f;
	ArgDesc.CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	////CapsuleControllerDesc.reportCallback = NULL;
	////CapsuleControllerDesc.behaviorCallback = NULL;
	//CapsuleControllerDesc.density = 10.f;
	//CapsuleControllerDesc.scaleCoeff = 0.8f;
	//CapsuleControllerDesc.invisibleWallHeight = 0.f;
	//CapsuleControllerDesc.maxJumpHeight = 10.f;
	//CapsuleControllerDesc.volumeGrowth = 1.5f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_PlayerActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);
	m_pActorCom->Set_PlayerType(CPlayerActor::PLAYER_MAY);


	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_May_Boots"), nullptr, (CGameObject**)&m_pEffect_GravityBoots), E_FAIL);
	m_pEffect_GravityBoots->Set_Model(m_pModelCom);

	FAILED_CHECK_RETURN(Ready_Layer_Gauge_Circle(TEXT("Layer_MayCircle_Gauge")), E_FAIL);

	return S_OK;
}

HRESULT CMay::Ready_UI()
{
	UI_Create(May, PlayerMarker);

	CGameObject* pGameObject = nullptr;
	_uint iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MayHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pHpBar = static_cast<CHpBar*>(pGameObject);
	m_pHpBar->Set_PlayerID(Player::May);
	m_pHpBar->Set_ShaderOption(1);

	iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MaySubHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pSubHpBar = static_cast<CHpBar*>(pGameObject);
	m_pSubHpBar->Set_PlayerID(Player::May);
	m_pSubHpBar->Set_ShaderOption(1);

	iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameMayHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pMinigameHpBar = static_cast<CMinigameHpBar*>(pGameObject);
	m_pMinigameHpBar->Set_PlayerID(Player::May);
	m_pMinigameHpBar->Set_ShaderOption(1);

	iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameMaySubHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pMinigameSubHpBar = static_cast<CMinigameHpBar*>(pGameObject);
	m_pMinigameSubHpBar->Set_PlayerID(Player::May);
	m_pMinigameSubHpBar->Set_ShaderOption(1);

	return S_OK;
}

void CMay::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_M_Jog, ANI_M_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Roll_Start, ANI_M_Roll_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Roll_Jog, ANI_M_Jog, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_M_Roll_Start, ANI_M_Roll_Stop, true, 15.f);

	m_pModelCom->Add_LerpInfo(ANI_M_ExhaustedMH, ANI_M_Jog_Exhausted_Start, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_M_Jog_Exhausted_Start, ANI_M_Jog, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_M_Jog_Exhausted_Start, ANI_M_Jog_Start, true, 20.f);

	m_pModelCom->Add_LerpInfo(ANI_M_GroundPound_Land, ANI_M_GroundPound_Land_Exit, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Sprint, ANI_M_SprintTurnAround, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_M_Sprint, ANI_M_Sprint, false);

	m_pModelCom->Add_LerpInfo(ANI_M_DoubleJump, ANI_M_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Jump_Start, ANI_M_AirDash_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_M_DoubleJump, ANI_M_AirDash_Start, false);

	m_pModelCom->Add_LerpInfo(ANI_M_ZeroGravity_MH, ANI_M_Jump_180L, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_M_ZeroGravity_MH, ANI_M_Jump_180R, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_M_ZeroGravity_MH, ANI_M_Jump_Falling, true, 10.f);

	m_pModelCom->Add_LerpInfo(ANI_M_WallSlide_MH, ANI_M_WallSlide_Jump, true, 30.f);
	m_pModelCom->Add_LerpInfo(ANI_M_WallSlide_Jump, ANI_M_WallSlide_Enter, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_M_WallSlide_Enter, ANI_M_WallSlide_MH, true, 30.f);

	m_pModelCom->Add_LerpInfo(ANI_M_MH, ANI_M_Valve_Rotate_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Valve_Rotate_MH, ANI_M_Valve_Rotate_R, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Valve_Rotate_R, ANI_M_Valve_Rotate_MH, false);

	m_pModelCom->Add_LerpInfo(ANI_M_Rocket_Enter, ANI_M_Rocket_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Rocket_Exit, ANI_M_Jump_Land_High, false);


	return;
}

_int CMay::Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);
	if (CCutScenePlayer::GetInstance()->Get_IsPlayCutScene())
	{
		m_pActorCom->Set_ZeroGravity(true, true, true);
		m_pActorCom->Update(dTimeDelta); 
		m_pModelCom->Update_Animation(dTimeDelta);
		return NO_EVENT;
	}
	/* UI */
	UI_Generator->Set_TargetPos(Player::Cody, UI::PlayerMarker, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	/* Script */
	Script_Trigger(dTimeDelta);

	m_pCamera = (CSubCamera*)CDataStorage::GetInstance()->Get_SubCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	KeyInput_Rail(dTimeDelta);
	_bool Test = m_pActorCom->Get_IsOnGravityPath();

	if (false == m_bMoveToRail && false == m_bOnRail && false == m_IsInUFO)
	{
		LaserTennis(dTimeDelta);
		Wall_Jump(dTimeDelta);
		if (Trigger_Check(dTimeDelta))
		{
			TakeRailEnd(dTimeDelta);
			Hit_StarBuddy(dTimeDelta);
			Hit_Rocket(dTimeDelta);
			Activate_RobotLever(dTimeDelta);
			Pull_VerticalDoor(dTimeDelta);
			Rotate_Valve(dTimeDelta);
			In_GravityPipe(dTimeDelta);
			PinBall(dTimeDelta);
			Warp_Wormhole(dTimeDelta);
			Touch_FireDoor(dTimeDelta);
			WallLaserTrap(dTimeDelta);
			Hook_UFO(dTimeDelta);
			Falling_Dead(dTimeDelta);
			BossMissile_Control(dTimeDelta);
			Ride_Ending_Rocket(dTimeDelta);
		}
		else
		{
			// 트리거 끝나고 애니메이션 초기화
			Trigger_End(dTimeDelta);
			m_IsFalling = m_pActorCom->Get_IsFalling();
			m_pActorCom->Set_GroundPound(m_bGroundPound);

			if (m_bRoll == false || m_bSprint == true)
				KeyInput(dTimeDelta);
			if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false && m_bLandHigh == false)
			{
				Sprint(dTimeDelta);
				Move(dTimeDelta);
				Roll(dTimeDelta);
				Jump(dTimeDelta);
			}
			Ground_Pound(dTimeDelta);
		}
	}

	/* 레일 타겟을 향해 날라가기 */
	// Forward 조정
	MoveToTargetRail(dTimeDelta);

	/* 레일타기 : 타겟을 찾지 못하면 타지않음. */
	TakeRail(dTimeDelta);

	if (true == m_bOnRail || true == m_bMoveToRail || true == m_IsInUFO)
	{
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pActorCom->Set_Position(vPlayerPos);
	}
	else
		m_pActorCom->Update(dTimeDelta); // Set Position하면 이거 할필요없다.

	//m_pActorCom->Set_IsOnGravityPath(false);
	m_pModelCom->Update_Animation(dTimeDelta);
	m_pEffect_GravityBoots->Update_Matrix(m_pTransformCom->Get_WorldMatrix());

	// Control RadiarBlur - 제일 마지막에 호출
	Trigger_RadiarBlur(dTimeDelta);

	return NO_EVENT;
}

_int CMay::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);
	if (CCutScenePlayer::GetInstance()->Get_IsPlayCutScene())
	{
		if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		return NO_EVENT;
	}

	/* LateTick : 레일의 타겟 찾기*/
	Find_TargetSpaceRail();
	ShowRailTargetTriggerUI();
	Clear_TagerRailNodes();

	/* 메이 UFO탔을 때 */
	InUFO(dTimeDelta);

	if (true == m_IsTouchFireDoor || true == m_IsWallLaserTrap_Touch || true == m_IsDeadLine)
		return NO_EVENT;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f)) 
	{
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMay::Render(RENDER_GROUP::Enum eGroup)
{
	CCharacter::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	if (eGroup == RENDER_GROUP::RENDER_NONALPHA)
	{
		m_pModelCom->Set_DefaultVariables_Shadow();
		m_pModelCom->Render_Model(0);
	}
	else if (eGroup == RENDER_GROUP::RENDER_ALPHA && false == m_IsEnding)
	{
		m_pModelCom->Render_Model(30);
		m_pModelCom->Render_Model(31);
	}

	return S_OK;
}


CMay* CMay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMay* pInstance = new CMay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMay::Clone_GameObject(void* pArg)
{
	CMay* pInstance = new CMay(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMay::Free()
{
	/* For. Rail */
	m_pSearchTargetRailNode = nullptr;
	m_pTargetRail = nullptr;
	m_pTargetRailNode = nullptr;
	m_vecTargetRailNodes.clear();

	Safe_Release(m_pGauge_Circle);
	Safe_Release(m_pHpBar);
	Safe_Release(m_pSubHpBar);
	Safe_Release(m_pMinigameHpBar);
	Safe_Release(m_pMinigameSubHpBar);

	//Safe_Release(m_pCamera); 
	Safe_Release(m_pTargetPtr);
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffect_GravityBoots);

	CCharacter::Free();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMay::KeyInput(_double dTimeDelta)
{

#ifdef __CONTROL_MAY_KEYBOARD
#pragma region Teleport
	if (m_pGameInstance->Key_Down(DIK_1)) /* 스타트 지점 */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 0.f, 15.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_2)) /* 2층 */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 127.f, 170.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_3)) /* 2스테이지 입구 */
		m_pActorCom->Set_Position(XMVectorSet(620.f, 760.f, 195.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_4)) /* 2스테이지 */
		m_pActorCom->Set_Position(XMVectorSet(960.f, 720.f, 193.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_5))/* 3스테이지 */
		m_pActorCom->Set_Position(XMVectorSet(-650.f, 760.f, 195.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_6))/* 3층 */
		m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_7))/* Boss */
		m_pActorCom->Set_Position(XMVectorSet(62.f, 250.f, 187.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_8))/* Moon */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 760.f, 194.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_9))/* 우산 */
		m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_0))/* 레이저 테니스 */
		m_pActorCom->Set_Position(XMVectorSet(64.f, 730.f, 1000.f, 1.f));
	if (m_pGameInstance->Get_CurrentLevelStep() == 2)
		m_IsEnding = true;

#pragma endregion

#pragma region Local variable
	_vector vCameraLook, vCameraRight;
	if (m_pActorCom->Get_IsOnGravityPath() == false)
	{
		vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	}
	else
	{
		PxVec3 vNormal = m_pActorCom->Get_GravityNormal();
		_vector vGravityPathNormal = XMVector3Normalize(XMVectorSet(vNormal.x, vNormal.y, vNormal.z, 0.f));

		vCameraRight = XMVector3Normalize(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
		vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, vGravityPathNormal));
	}
	_bool bMove[2] = { false, false };
	_bool bRoll = false;

#pragma endregion
	if (m_pGameInstance->Key_Down(DIK_U))/* 메이 우주선 태우기 */
		Set_UFO(true);

	//if (m_pGameInstance->Key_Down(DIK_Y))/* 3층 */
	//	m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
	//if (m_pGameInstance->Key_Down(DIK_O))/* 우산 */
	//	m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));

#pragma region 8Way_Move

	if (m_IsAirDash == false)
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP) && m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_UP) && m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook - vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_DOWN) && m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_DOWN) && m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook - vCameraRight) / 2.f);
		}
		else
		{
			if (m_pGameInstance->Key_Pressing(DIK_LEFT) && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Key_Pressing(DIK_RIGHT) && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Key_Pressing(DIK_UP) && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Key_Pressing(DIK_DOWN) && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}

			if (m_pGameInstance->Key_Pressing(DIK_UP))
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, vCameraLook);
				m_iSavedKeyPress = UP;
			}
			if (m_pGameInstance->Key_Pressing(DIK_DOWN))
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
				m_iSavedKeyPress = DOWN;
			}

			if (m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
				m_iSavedKeyPress = LEFT;
			}
			if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, vCameraRight);
				m_iSavedKeyPress = RIGHT;
			}
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_SprintTurnAround)
		{
			if (m_fSprintAcceleration < 12.f)
				m_fSprintAcceleration += (_float)dTimeDelta * 20.f;
		}
		if (m_pModelCom->Is_AnimFinished(ANI_M_SprintTurnAround))
		{
			m_IsTurnAround = false;
		}


		if (bMove[0] || bMove[1])
		{
			m_bMove = true;
		}
	}

#pragma endregion

#pragma region Pad Square
	if (m_pGameInstance->Key_Down(DIK_L) && m_bRoll == false && m_bCanMove == true)
	{
		//XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DASH, m_fMay_Dash_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dash.wav"), CHANNEL_MAY_DASH, m_fMay_Dash_Volume);
			EFFECT->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());

			m_fAcceleration = 5.f;
			m_pModelCom->Set_Animation(ANI_M_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Roll_Stop);

			m_bAction = false;
			m_bRoll = true;

			Start_RadiarBlur(0.3f);
		}
		else
		{
			if (m_pModelCom->Get_CurAnimIndex() != ANI_M_AirDash_Start && m_iAirDashCount == 0)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DASH, m_fMay_Dash_Volume);
				m_pGameInstance->Play_Sound(TEXT("May_Dash.wav"), CHANNEL_MAY_DASH, m_fMay_Dash_Volume);

				m_iAirDashCount += 1;
				m_fAcceleration = 5.f;
				m_pActorCom->Jump_Start(1.2f);
				m_pModelCom->Set_Animation(ANI_M_AirDash_Start);
				m_IsAirDash = true;

				Start_RadiarBlur(0.3f);
			}
		}

	}
#pragma endregion

#pragma region PAD X

	if (m_pGameInstance->Key_Down(DIK_K) && m_iJumpCount == 0)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
		m_IsJumping = true;
		m_pModelCom->Set_Animation(ANI_M_Jump_Start);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
	}

	else if (m_pGameInstance->Key_Down(DIK_K) && m_iJumpCount == 1)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
		m_IsJumping = true;
		m_pModelCom->Set_Animation(ANI_M_DoubleJump);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
	}
#pragma endregion

#pragma region LS_Click
	if (m_pGameInstance->Key_Down(DIK_APOSTROPHE) && m_pModelCom->Get_CurAnimIndex() != ANI_M_Jog_Exhausted_Start && m_pModelCom->Get_CurAnimIndex() != ANI_M_Jog_Stop_Exhausted && m_pModelCom->Get_CurAnimIndex() != ANI_M_Sprint_Start_FromDash)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint)
		{
			m_pModelCom->Set_Animation(ANI_M_Jog_Exhausted_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
		}
		if (m_bSprint == false)
		{
			m_bSprint = true;
		}
		else
		{
			m_bSprint = false;
		}
	}
#pragma endregion

#pragma region PAD O

	if (m_pGameInstance->Key_Down(DIK_SEMICOLON) && m_pActorCom->Get_IsJump() == true)
	{
		m_fAcceleration = 5.0f;
		m_fJogAcceleration = 25.f;
		m_fSprintAcceleration = 35.f;
		m_bGroundPound = true;
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_GROUNDPOUND_ROLL, m_fMay_GroundPound_Roll_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_GroundPound_Roll.wav"), CHANNEL_MAY_GROUNDPOUND_ROLL, m_fMay_GroundPound_Roll_Volume);

		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_GROUNDPOUND_ROLL_VOICE, m_fMay_GroundPound_Roll_Voice_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_GroundPound_Roll_Voice.wav"), CHANNEL_MAY_GROUNDPOUND_ROLL_VOICE, m_fMay_GroundPound_Roll_Voice_Volume);


	}
#pragma endregion 

#pragma region Effect GravityBoots
	if (m_pActorCom->Get_IsOnGravityPath() == true)
	{
		m_pEffect_GravityBoots->Set_IsActivate_GravityBoots();

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog)
		{
			if (false == m_IsLeftFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.25f && m_pModelCom->Get_ProgressAnim() < 0.29f) // 왼발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(false);
				m_IsLeftFoot_Effect = true;
				m_IsRightFoot_Effect = false;
			}
			if (false == m_IsRightFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.65f && m_pModelCom->Get_ProgressAnim() < 0.68f)// 오른발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(true);
				m_IsLeftFoot_Effect = false;
				m_IsRightFoot_Effect = true;
			}
		}
		else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint)
		{
			if (false == m_IsLeftFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.49f  && m_pModelCom->Get_ProgressAnim() < 0.53f) // 왼발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(true);
				m_IsLeftFoot_Effect = true;
				m_IsRightFoot_Effect = false;
			}
			if (false == m_IsRightFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.1f && m_pModelCom->Get_ProgressAnim() < 0.13f) // 오른발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(false);
				m_IsLeftFoot_Effect = false;
				m_IsRightFoot_Effect = true;
			}
		}
		else
		{
			m_IsLeftFoot_Effect = false;
			m_IsRightFoot_Effect = false;
		}
	}
#pragma  endregion

#else
#pragma region Local variable
	_vector vCameraLook, vCameraRight;
	if (m_pActorCom->Get_IsOnGravityPath() == false)
	{
		vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	}
	else
	{
		PxVec3 vNormal = m_pActorCom->Get_GravityNormal();
		_vector vGravityPathNormal = XMVector3Normalize(XMVectorSet(vNormal.x, vNormal.y, vNormal.z, 0.f));

		vCameraRight = XMVector3Normalize(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
		vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, vGravityPathNormal));
	}
	_bool bMove[2] = { false, false };
	_bool bRoll = false;

#pragma endregion
#pragma region Teleport
	if (m_pGameInstance->Key_Down(DIK_1)) /* 스타트 지점 */
	{
		m_pActorCom->Set_Position(XMVectorSet(60.f, 0.f, 15.f, 1.f));
		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
	}
	if (m_pGameInstance->Key_Down(DIK_2)) /* 2층 */
	{
		m_pActorCom->Set_Position(XMVectorSet(60.f, 127.f, 170.f, 1.f));
		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
	}
	if (m_pGameInstance->Key_Down(DIK_3)) /* 2스테이지 입구 */
	{
		m_pActorCom->Set_Position(XMVectorSet(620.f, 760.f, 195.f, 1.f));
		DATABASE->Set_May_Stage(ST_RAIL);
		DATABASE->Set_Cody_Stage(ST_RAIL);
	}
	if (m_pGameInstance->Key_Down(DIK_4)) /* 2스테이지 */
	{
		m_pActorCom->Set_Position(XMVectorSet(960.f, 720.f, 193.f, 1.f));
		DATABASE->Set_May_Stage(ST_RAIL);
		DATABASE->Set_Cody_Stage(ST_RAIL);
	}
	if (m_pGameInstance->Key_Down(DIK_5))/* 3스테이지 */
	{
		m_pActorCom->Set_Position(XMVectorSet(-650.f, 760.f, 195.f, 1.f));
		DATABASE->Set_May_Stage(ST_PINBALL);
		DATABASE->Set_Cody_Stage(ST_PINBALL);
	}
	if (m_pGameInstance->Key_Down(DIK_7))/* Boss */
		m_pActorCom->Set_Position(XMVectorSet(62.f, 250.f, 187.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_8))/* Moon */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 760.f, 194.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_9))/* 우주선 내부 */
		m_pActorCom->Set_Position(XMVectorSet(63.f, 600.f, 1005.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_0))/* 벽점프 전 */
		m_pActorCom->Set_Position(XMVectorSet(-830.374512f, 793.359192f, 192.788605f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_Y))/* 3층 */
		m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_X))/* 우산 */
		m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));
#pragma endregion

#pragma region 8Way_Move

	if (m_IsAirDash == false)
	{
		// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
		// 오른쪽윈
		if (m_pGameInstance->Get_Pad_LStickX() > 44000 && m_pGameInstance->Get_Pad_LStickY() < 34000)
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook + vCameraRight) / 2.f);
		}
		// 왼쪽위
		else if (m_pGameInstance->Get_Pad_LStickX() < 20000 && m_pGameInstance->Get_Pad_LStickY() < 34000)
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook - vCameraRight) / 2.f);
		}
		// 오른쪽아래
		else if (m_pGameInstance->Get_Pad_LStickX() > 44000 && m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook + vCameraRight) / 2.f);
		}
		// 왼쪽아래
		else if (m_pGameInstance->Get_Pad_LStickX() < 20000 && m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook - vCameraRight) / 2.f);
		}
		else
		{
			if (m_pGameInstance->Get_Pad_LStickX() <= 25000 && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickX() >= 41000 && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickY() <= 25000 && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickY() >= 41000 && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) && m_IsTurnAround == false)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}

			if ( m_pGameInstance->Get_Pad_LStickY() < 20000)
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, vCameraLook);
				m_iSavedKeyPress = UP;
			}

			if ( m_pGameInstance->Get_Pad_LStickY() > 44000)
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
				m_iSavedKeyPress = DOWN;
			}

			if (m_pGameInstance->Get_Pad_LStickX() < 20000)
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
				m_iSavedKeyPress = LEFT;
			}
			if (m_pGameInstance->Get_Pad_LStickX() > 44000)
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, vCameraRight);
				m_iSavedKeyPress = RIGHT;
			}
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_SprintTurnAround)
		{
			if (m_fSprintAcceleration < 12.f)
				m_fSprintAcceleration += (_float)dTimeDelta * 20.f;
		}

		if (m_pModelCom->Is_AnimFinished(ANI_M_SprintTurnAround))
			m_IsTurnAround = false;


		if (bMove[0] || bMove[1])
			m_bMove = true;
	}

#pragma endregion

#pragma region Pad Square
	if (m_pGameInstance->Pad_Key_Down(DIP_X) && m_bRoll == false && m_bCanMove == true)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DASH, m_fMay_Dash_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dash.wav"), CHANNEL_MAY_DASH, m_fMay_Dash_Volume);

			m_fAcceleration = 5.f;
			m_pModelCom->Set_Animation(ANI_M_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Roll_Stop);

			m_bAction = false;
			m_bRoll = true;
		}
		else
		{
			if (m_pModelCom->Get_CurAnimIndex() != ANI_M_AirDash_Start && m_iAirDashCount == 0)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DASH, m_fMay_Dash_Volume);
				m_pGameInstance->Play_Sound(TEXT("May_Dash.wav"), CHANNEL_MAY_DASH, m_fMay_Dash_Volume);

				m_iAirDashCount += 1;
				m_fAcceleration = 5.f;
				m_pActorCom->Jump_Start(1.2f);
				m_pModelCom->Set_Animation(ANI_M_AirDash_Start);
				m_IsAirDash = true;
			}
		}
	}
#pragma endregion

#pragma region PAD X
	if (m_pGameInstance->Pad_Key_Down(DIP_B) && m_iJumpCount == 0 /*&& m_pModelCom->Get_CurAnimIndex() != ANI_M_Jump_Falling */&& m_bCanMove == true)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
		m_IsJumping = true;
		m_pModelCom->Set_Animation(ANI_M_Jump_Start);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
	}

	else if (m_pGameInstance->Pad_Key_Down(DIP_B) && m_iJumpCount == 1/* && m_pModelCom->Get_CurAnimIndex() != ANI_M_Jump_Falling */&& m_bCanMove == true)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
		m_IsJumping = true;
		m_pModelCom->Set_Animation(ANI_M_DoubleJump);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
	}
#pragma endregion

#pragma region LS_Click
	if (m_pGameInstance->Pad_Key_Down(DIP_LS) && m_pModelCom->Get_CurAnimIndex() != ANI_M_Jog_Exhausted_Start && m_pModelCom->Get_CurAnimIndex() != ANI_M_Jog_Stop_Exhausted && m_pModelCom->Get_CurAnimIndex() != ANI_M_Sprint_Start_FromDash)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint)
		{
			m_pModelCom->Set_Animation(ANI_M_Jog_Exhausted_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
		}
		if (m_bSprint == false)
			m_bSprint = true;
		else
			m_bSprint = false;
	}
#pragma endregion

#pragma region PAD O

	if (m_pGameInstance->Pad_Key_Down(DIP_A) && m_pActorCom->Get_IsJump() == true && m_iJumpCount > 0 && m_bAfterGroundPound == false)
	{
		m_fAcceleration = 5.0f;
		m_fJogAcceleration = 25.f;
		m_fSprintAcceleration = 35.f;
		m_bGroundPound = true;

		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_GROUNDPOUND_ROLL, m_fMay_GroundPound_Roll_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_GroundPound_Roll.wav"), CHANNEL_MAY_GROUNDPOUND_ROLL, m_fMay_GroundPound_Roll_Volume);

		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_GROUNDPOUND_ROLL_VOICE, m_fMay_GroundPound_Roll_Voice_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_GroundPound_Roll_Voice.wav"), CHANNEL_MAY_GROUNDPOUND_ROLL_VOICE, m_fMay_GroundPound_Roll_Voice_Volume);

	}

#pragma endregion 

#pragma region Effect GravityBoots
	if (m_pActorCom->Get_IsOnGravityPath() == true)
	{
		m_pEffect_GravityBoots->Set_IsActivate_GravityBoots();

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog)
		{
			if (false == m_IsLeftFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.25f && m_pModelCom->Get_ProgressAnim() < 0.29f) // 왼발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(false);
				m_IsLeftFoot_Effect = true;
				m_IsRightFoot_Effect = false;
			}
			if (false == m_IsRightFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.65f && m_pModelCom->Get_ProgressAnim() < 0.68f)// 오른발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(true);
				m_IsLeftFoot_Effect = false;
				m_IsRightFoot_Effect = true;
			}
		}
		else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint)
		{
			if (false == m_IsLeftFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.49f  && m_pModelCom->Get_ProgressAnim() < 0.53f) // 왼발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(true);
				m_IsLeftFoot_Effect = true;
				m_IsRightFoot_Effect = false;
			}
			if (false == m_IsRightFoot_Effect && m_pModelCom->Get_ProgressAnim() > 0.1f && m_pModelCom->Get_ProgressAnim() < 0.13f) // 오른발
			{
				m_pEffect_GravityBoots->Add_WalkingParticle(false);
				m_IsLeftFoot_Effect = false;
				m_IsRightFoot_Effect = true;
			}
		}
		else
		{
			m_IsLeftFoot_Effect = false;
			m_IsRightFoot_Effect = false;
		}
	}
#pragma  endregion
#endif
}

void CMay::Enforce_IdleState()
{
	m_bSprint = false;
	m_bRoll = false;
	m_bMove = false;
	m_bShortJump = false;
	m_bGroundPound = false;
	m_IsTurnAround = false;
	m_bWallAttach = false;
	m_IsWallJumping = false;
	m_bAction = false;

	m_IsJumping = false;
	m_IsAirDash = false;
	m_IsFalling = false;
	m_bFallAniOnce = false;
		
	m_bPlayGroundPoundOnce = false;

	m_fIdleTime = 0.f;
	m_bCanMove = true;

	m_iJumpCount = 0;
	m_iAirDashCount = 0;

	//m_IsOnGrind = false;
	//m_IsHitStarBuddy = false;
	//m_IsHitRocket = false;
	//m_IsActivateRobotLever = false;
	//m_IsPullVerticalDoor = false;
	//m_IsEnterValve = false;
	//m_IsInGravityPipe = false;
	//m_IsPinBall = false;
	//m_IsDeadLine = false;
	//m_IsWarpNextStage = false;
	//m_IsWarpDone = false;
	//m_IsTouchFireDoor = false;
	//m_IsHookUFO = false;
	//m_IsBossMissile_Control = false;
	//m_IsWallLaserTrap_Touch = false;
	//m_bWallAttach = false;
	//m_IsRippedOffAnimPlaying = false;
	//m_bLaserTennis = false;
	//m_IsEnding = false;

	m_pActorCom->Set_IsFalling(false);
	m_pActorCom->Set_ZeroGravity(false, false, false);
	m_pActorCom->Set_Jump(false);
	m_pModelCom->Set_Animation(ANI_M_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
}

void CMay::Update_Tirgger_Pos(_vector vPos)
{
	XMStoreFloat3(&m_vTriggerTargetPos, vPos);
}

_uint CMay::Get_CurState() const
{
	if (nullptr == m_pModelCom) return 0;

	return m_pModelCom->Get_CurAnimIndex();
}

void CMay::Move(const _double dTimeDelta)
{
	if (m_bSprint == false && m_bMove && m_pTransformCom)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		if (m_pActorCom->Get_IsOnGravityPath() == false) // 중력 발판 위에 있지 않을때.
		{
			_vector vPlayerUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

			// 양수 일때?
			_float fPlayerUpX = XMVectorGetX(vPlayerUp);
			_float fPlayerUpY = XMVectorGetY(vPlayerUp);
			_float fPlayerUpZ = XMVectorGetZ(vPlayerUp);

			// xyz 성분중 가장 큰 친구를 찾자.
			_float fAbsX = fabs(fPlayerUpX);
			_float fAbsY = fabs(fPlayerUpY);
			_float fAbsZ = fabs(fPlayerUpZ);

			if ((fAbsX > fAbsY) && (fAbsX > fAbsZ))
			{
				// x가 z,y 보다 크다면 카메라LookVector의 X성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetX(vDirection, 0.f));
			}

			else if ((fAbsY > fAbsX) && (fAbsY > fAbsZ))
			{
				// y가 x,z 보다 크다면 카메라 LookVector의 Y성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetY(vDirection, 0.f));
			}

			else if ((fAbsZ > fAbsX) && (fAbsZ > fAbsY))
			{
				// z가 x,y 보다 크다면 카메라 LookVector의 Z성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetZ(vDirection, 0.f));
			}
		}
		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

		if (m_fJogAcceleration > 10.f)
			m_fJogAcceleration -= (_float)dTimeDelta * 50.f;
		else
			m_fJogAcceleration = 10.f;

		m_pActorCom->Move(vDirection / m_fJogAcceleration, dTimeDelta);

		if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false && ANI_M_Jump_Land_Jog != m_pModelCom->Get_CurAnimIndex())
		{

			if (m_pModelCom->Is_AnimFinished(ANI_M_Jog_Start) == true) // JogStart -> Jog
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				}

				m_pModelCom->Set_Animation(ANI_M_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Jog) == true) // Jog -> Jog // 보간속도 Up
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				}

				m_pModelCom->Set_Animation(ANI_M_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop || m_pModelCom->Get_CurAnimIndex() == ANI_M_MH || m_pModelCom->Get_CurAnimIndex() == ANI_M_MH_Gesture_Small_Stretch || m_pModelCom->Get_CurAnimIndex() == ANI_M_ActionMH)	// Idle To Jog Start. -> Jog 예약
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				}

				m_pModelCom->Set_Animation(ANI_M_Jog_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH_To_Idle ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted))
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				}

				m_pModelCom->Set_Animation(ANI_M_Jog_Exhausted_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				}

				m_bRoll = false;
				m_pModelCom->Set_Animation(ANI_M_Roll_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
				return;
			}
		}
		m_bMove = false;
	}
	else
	{
		if (m_bShortJump == false)
		{
			m_fJogAcceleration = 25.f;
			if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog) // jog 였다면
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH); // jog to stop 끝나면 idle 예약.
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Start) // JogStart 였다면
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_MH) // IDLE 상태라면
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
				m_fIdleTime += (_float)dTimeDelta;
				if (m_bAction == false)
				{
					if (m_fIdleTime > 5.f && m_pModelCom->Is_AnimFinished(ANI_M_MH)) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
					{
						m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
						m_pModelCom->Set_Animation(ANI_M_MH_Gesture_Small_Stretch); // 배 두들기는 애니메이션 재생
						m_fIdleTime = 0.f;
					}
				}
				else if (m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
					m_pModelCom->Set_Animation(ANI_M_MH_To_Action);
					m_pModelCom->Set_NextAnimIndex(ANI_M_ActionMH);
				}
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_MH_To_Action) == true && m_bAction == true)
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
				m_pModelCom->Set_Animation(ANI_M_ActionMH);
				m_pModelCom->Set_NextAnimIndex(ANI_M_ActionMH_To_Idle);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_ActionMH) == true && m_bAction == true)
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
				m_pModelCom->Set_Animation(ANI_M_ActionMH_To_Idle);
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
				m_bAction = false;
			}
		}
	}
}
void CMay::Roll(const _double dTimeDelta)
{
	if ((m_bRoll && m_pTransformCom))
	{
		if (m_pModelCom->Is_AnimFinished(ANI_M_Roll_Start))
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DASH_LANDING, m_fMay_Dash_Landing_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dash_Landing.wav"), CHANNEL_MAY_DASH_LANDING, m_fMay_Dash_Landing_Volume);

			m_fAcceleration = 5.0;
			m_pModelCom->Set_Animation(ANI_M_Roll_Stop);
			if (m_bMove == false)
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

			m_bRoll = false;
			m_bAction = true;
			return;
		}

		m_fAcceleration -= (_float)dTimeDelta * 10.f;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		if (m_pActorCom->Get_IsOnGravityPath() == false)
		{
			_vector vPlayerUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

			// 양수 일때?
			_float fPlayerUpX = XMVectorGetX(vPlayerUp);
			_float fPlayerUpY = XMVectorGetY(vPlayerUp);
			_float fPlayerUpZ = XMVectorGetZ(vPlayerUp);

			// xyz 성분중 가장 큰 친구를 찾자.
			_float fAbsX = fabs(fPlayerUpX);
			_float fAbsY = fabs(fPlayerUpY);
			_float fAbsZ = fabs(fPlayerUpZ);

			if ((fAbsX > fAbsY) && (fAbsX > fAbsZ))
			{
				// x가 z,y 보다 크다면 카메라LookVector의 X성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetX(vDirection, 0.f));
			}

			else if ((fAbsY > fAbsX) && (fAbsY > fAbsZ))
			{
				// y가 x,z 보다 크다면 카메라 LookVector의 Y성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetY(vDirection, 0.f));
			}

			else if ((fAbsZ > fAbsX) && (fAbsZ > fAbsY))
			{
				// z가 x,y 보다 크다면 카메라 LookVector의 Z성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetZ(vDirection, 0.f));
			}
		}

		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 15.f), dTimeDelta);
	}

	if (m_IsAirDash && m_bCanMove == true && m_pTransformCom)
	{
		if (m_fAcceleration <= 0.f)
		{
			m_IsAirDash = false;
		}

		if (m_fAcceleration > 0.f)
			m_fAcceleration -= (_float)dTimeDelta * 10.f;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		if (m_pActorCom->Get_IsOnGravityPath() == false)
		{
			_vector vPlayerUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

			// 양수 일때?
			_float fPlayerUpX = XMVectorGetX(vPlayerUp);
			_float fPlayerUpY = XMVectorGetY(vPlayerUp);
			_float fPlayerUpZ = XMVectorGetZ(vPlayerUp);

			// xyz 성분중 가장 큰 친구를 찾자.
			_float fAbsX = fabs(fPlayerUpX);
			_float fAbsY = fabs(fPlayerUpY);
			_float fAbsZ = fabs(fPlayerUpZ);

			if ((fAbsX > fAbsY) && (fAbsX > fAbsZ))
			{
				// x가 z,y 보다 크다면 카메라LookVector의 X성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetX(vDirection, 0.f));
			}

			else if ((fAbsY > fAbsX) && (fAbsY > fAbsZ))
			{
				// y가 x,z 보다 크다면 카메라 LookVector의 Y성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetY(vDirection, 0.f));
			}

			else if ((fAbsZ > fAbsX) && (fAbsZ > fAbsY))
			{
				// z가 x,y 보다 크다면 카메라 LookVector의 Z성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetZ(vDirection, 0.f));
			}
		}
		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), dTimeDelta);
	}

}
void CMay::Sprint(const _double dTimeDelta)
{
	if (m_bSprint == true && m_bMove == true)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		if (m_pActorCom->Get_IsOnGravityPath() == false)
		{
			_vector vPlayerUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

			// 양수 일때
			_float fPlayerUpX = XMVectorGetX(vPlayerUp);
			_float fPlayerUpY = XMVectorGetY(vPlayerUp);
			_float fPlayerUpZ = XMVectorGetZ(vPlayerUp);

			// xyz 성분중 가장 큰 친구를 찾자.
			_float fAbsX = fabs(fPlayerUpX);
			_float fAbsY = fabs(fPlayerUpY);
			_float fAbsZ = fabs(fPlayerUpZ);

			if ((fAbsX > fAbsY) && (fAbsX > fAbsZ))
			{
				// x가 z,y 보다 크다면 카메라LookVector의 X성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetX(vDirection, 0.f));
			}

			else if ((fAbsY > fAbsX) && (fAbsY > fAbsZ))
			{
				// y가 x,z 보다 크다면 카메라 LookVector의 Y성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetY(vDirection, 0.f));
			}

			else if ((fAbsZ > fAbsX) && (fAbsZ > fAbsY))
			{
				// z가 x,y 보다 크다면 카메라 LookVector의 Z성분을 0으로 만들고 노말라이즈
				vDirection = XMVector3Normalize(XMVectorSetZ(vDirection, 0.f));
			}
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_SprintTurnAround)
			m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * 3.f);
		else
			m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

		if (m_fSprintAcceleration > 5.f)
			m_fSprintAcceleration -= (_float)dTimeDelta * 50.f;
		else
			m_fSprintAcceleration = 5.f;

		m_pActorCom->Move(vDirection / m_fSprintAcceleration, dTimeDelta);

		if (m_bRoll == false && m_IsJumping == false && m_IsTurnAround == false)
		{
			if (m_pModelCom->Is_AnimFinished(ANI_M_Sprint_Start_FromDash) == true) // JogStart -> Jog
			{
				m_pModelCom->Set_Animation(ANI_M_Sprint);
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_RUN) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RUN, m_fMay_Run_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Run.wav"), CHANNEL_MAY_RUN, m_fMay_Run_Volume);
				}
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Sprint) == true) // Jog -> Jog // 보간속도 Up
			{
				m_pModelCom->Set_Animation(ANI_M_Sprint);
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_RUN) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RUN, m_fMay_Run_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Run.wav"), CHANNEL_MAY_RUN, m_fMay_Run_Volume);
				}
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_MH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_MH_Gesture_Small_Stretch ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_ActionMH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop)	// Idle To Jog Start. -> Jog 예약
			{
				m_pModelCom->Set_Animation(ANI_M_Sprint_Start_FromDash);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Sprint);
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_RUN) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RUN, m_fMay_Run_Volume);
					m_pGameInstance->Play_Sound(TEXT("May_Run.wav"), CHANNEL_MAY_RUN, m_fMay_Run_Volume);
				}
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
			{
				m_bRoll = false;
				if (m_bSprint == false)
				{
					m_pModelCom->Set_Animation(ANI_M_Roll_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
						m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
					}
				}
				else
				{
					m_pModelCom->Set_Animation(ANI_M_Sprint_Start_FromDash);
					m_pModelCom->Set_NextAnimIndex(ANI_M_Sprint);
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_RUN) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RUN, m_fMay_Run_Volume);
						m_pGameInstance->Play_Sound(TEXT("May_Run.wav"), CHANNEL_MAY_RUN, m_fMay_Run_Volume);
					}
				}
			}
		}
		m_bMove = false;
	}

	else if (m_bSprint == true && m_bMove == false)
	{
		if (m_bShortJump == false)
		{
			m_fSprintAcceleration = 35.f;
			if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) // jog 였다면
			{
				m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
				m_bSprint = false;
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop_Exhausted); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_ExhaustedMH); // jog to stop 끝나면 idle 예약.
			}
		}
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint_Start_FromDash) // JogStart 였다면
	{
		m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
		m_bSprint = false;
		m_pModelCom->Set_Animation(ANI_M_Jog_Stop_Exhausted); // jog to stop 으로 바꿔
		m_pModelCom->Set_NextAnimIndex(ANI_M_ExhaustedMH);
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH) // IDLE 상태라면
	{
		m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
		m_fIdleTime += (_float)dTimeDelta;

		if (m_fIdleTime > 3.f && m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
		{
			m_pModelCom->Set_Animation(ANI_M_ExhaustedMH_To_Idle);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_bSprint = false;
			m_fIdleTime = 0.f;
		}

	}
}
void CMay::Jump(const _double dTimeDelta)
{
	if (m_bShortJump == true)
	{
		if (m_iJumpCount == 1)
		{
			m_IsJumping = true;
			m_pActorCom->Jump_Start(2.6f);
			m_bShortJump = false;
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_JUMP, m_fMay_Jump_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Jump.wav"), CHANNEL_MAY_JUMP, m_fMay_Jump_Volume);
		}
		if (m_iJumpCount == 2)
		{
			m_IsJumping = true;
			m_pActorCom->Jump_Start(2.6f);
			m_bShortJump = false;
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_RUN);
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_JUMP_DOUBLE, m_fMay_Jump_Double_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Jump_Double.wav"), CHANNEL_MAY_JUMP_DOUBLE, m_fMay_Jump_Double_Volume);
		}
	}
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_JUMP_LANDING, m_fMay_Jump_Landing_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_Jump_Landing.wav"), CHANNEL_MAY_JUMP_LANDING, m_fMay_Jump_Landing_Volume);


		m_bSprint = false;
		m_iAirDashCount = 0;

		if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
			}

			m_pModelCom->Set_Animation(ANI_M_Jump_Land_Jog);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
		}
		else
		{
			m_pModelCom->Set_Animation(ANI_M_Jump_Land);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		}
		m_IsJumping = false;
		m_iJumpCount = 0;
	}
	else if (m_IsJumping == false && m_IsFalling == true && m_bRoll == false && m_bGroundPound == false)
	{

		m_bSprint = false;

		if (m_bFallAniOnce == false)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Roll_Start || ANI_M_Roll_Stop)
			{
				m_pModelCom->Set_Animation(ANI_M_Jump_LongJump);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_M_Jump_Falling);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
				m_iJumpCount = 1;
			}
			m_bFallAniOnce = true;
		}
	}
	else if (m_IsJumping == false && m_IsFalling == false && m_bFallAniOnce == true && m_bRoll == false && m_bGroundPound == false)
	{
		if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MAY_WALK) == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
				m_pGameInstance->Play_Sound(TEXT("May_Walk.wav"), CHANNEL_MAY_WALK, m_fMay_Walk_Volume);
			}

			m_pModelCom->Set_Animation(ANI_M_Jump_Land_Jog);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
		}
		else
		{
			//m_pModelCom->Set_Animation(ANI_M_Jump_Land);
			//m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		}

		m_bFallAniOnce = false;
		m_IsJumping = false;
		m_iJumpCount = 0;
		m_iAirDashCount = 0;
	}
	if ((m_pGameInstance->Pad_Key_Down(DIP_B) && m_IsFalling == true) || (m_pGameInstance->Key_Down(DIK_K) && m_IsFalling == true))
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_JUMP_DOUBLE, m_fMay_Jump_Double_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_Jump_Double.wav"), CHANNEL_MAY_JUMP_DOUBLE, m_fMay_Jump_Double_Volume);

		m_bShortJump = true;
		m_IsJumping = true;
		m_iJumpCount = 1;
		return;
	}

}
void CMay::Ground_Pound(const _double dTimeDelta)
{
	if (m_bGroundPound == true)
	{
		if (m_fGroundPoundAirDelay > 0.4f)
		{
			m_pModelCom->Set_Animation(ANI_M_GroundPound_Falling);
			m_pActorCom->Set_Gravity(-9.8f);
			m_pActorCom->Jump_Start(-20.f);
			m_fGroundPoundAirDelay = 0.f;
			m_bGroundPound = false;
		}
		else
		{
			m_bCanMove = false;
			m_pModelCom->Set_Animation(ANI_M_GroundPound_Start);
			m_pActorCom->Set_Jump(false);
			m_IsJumping = false;
			m_bShortJump = false;
			m_pActorCom->Set_Gravity(0.f);
			m_fGroundPoundAirDelay += (_float)dTimeDelta;
		}
	}

	if (m_pModelCom->Is_AnimFinished(ANI_M_GroundPound_Falling) && m_bPlayGroundPoundOnce == false)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_GROUNDPOUND_LANDING, m_fMay_GroundPound_Landing_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_GroundPound_Landing.wav"), CHANNEL_MAY_GROUNDPOUND_LANDING, m_fMay_GroundPound_Landing_Volume);

		m_bPlayGroundPoundOnce = true;
		m_pModelCom->Set_Animation(ANI_M_GroundPound_Land_Exit);
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
	}
	if (m_pModelCom->Is_AnimFinished(ANI_M_GroundPound_Land_Exit))
	{
		m_pModelCom->Set_Animation(ANI_M_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		m_bPlayGroundPoundOnce = false;
		m_IsAirDash = false;
		//m_bCanMove = true;
		m_bAfterGroundPound = true;
	}
	if (m_bAfterGroundPound == true)
		m_iAfterGroundPoundCount += 1;

	if (m_iAfterGroundPoundCount >= 10) // 1.5초 경직
	{
		m_iAfterGroundPoundCount = 0;
		m_bAfterGroundPound = false;
		m_bCanMove = true;
	}
}

void CMay::Add_OffSet_Pos(_fvector vAddOffSet)
{
}


#pragma region Trigger
void CMay::SetTriggerID(GameID::Enum eID, _bool IsCollide, _fvector vTriggerTargetPos, _uint _iPlayerName)
{
	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	XMStoreFloat3(&m_vTriggerTargetPos, vTriggerTargetPos);

	/* For.Valve */
	m_iValvePlayerName = _iPlayerName;
}

void CMay::SetTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerTargetWorld, _uint _iPlayerName)
{
	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	XMStoreFloat4x4(&m_TriggerTargetWorld, vTriggerTargetWorld);
	XMStoreFloat3(&m_vTriggerTargetPos, vTriggerTargetWorld.r[3]);

	/* For.Valve */
	m_iValvePlayerName = _iPlayerName;
}

void CMay::SetTriggerID_Ptr(GameID::Enum eID, _bool IsCollide, CGameObject * pTargetPtr)
{
	if (nullptr != m_pTargetPtr)
		Safe_Release(m_pTargetPtr);

	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	m_pTargetPtr = pTargetPtr;
	Safe_AddRef(m_pTargetPtr);
}

void CMay::SetCameraTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerCameraWorld)
{
	m_eCameraTriggerID = eID;
	m_IsCamTriggerCollide = IsCollide;
	XMStoreFloat4x4(&m_TriggerCameraWorld, vTriggerCameraWorld);
}

void CMay::SetCameraTriggerID_Pos(_fvector vCamTriggerPos)
{
	XMStoreFloat4(&m_vCamTriggerPos, vCamTriggerPos);
}

_bool CMay::Trigger_Check(const _double dTimeDelta)
{
	if (m_IsCollide == true)
	{
		if (m_eTargetGameID == GameID::eSTARBUDDY && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)))
		{
			m_pModelCom->Set_Animation(ANI_M_Lever_Left);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsHitStarBuddy = true;
		}
		else if (m_eTargetGameID == GameID::eMOONBABOON && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)))
		{
			m_pModelCom->Set_Animation(ANI_M_Grind_Grapple_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Grind_Grapple_ToGrind);
			m_IsOnGrind = true;
		}
		else if (m_eTargetGameID == GameID::eROCKET && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)))
		{
			m_pModelCom->Set_Animation(ANI_M_RocketFirework);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsHitRocket = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTLEVER && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)))
		{
			if (DATABASE->Get_May_Stage() == ST_GRAVITYPATH)
			{
				m_pActorCom->Set_Position(XMVectorSet(70.5799332f, 21.3829994f, 174.975174f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			else if (DATABASE->Get_May_Stage() == ST_PINBALL)
			{
				m_pTransformCom->Set_RotateAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
				m_pActorCom->Set_Position(XMVectorSet(-817.311035f, 789.223682f, 228.280615f, 1.f));
			}
			else if (DATABASE->Get_May_Stage() == ST_RAIL)
			{
				m_pActorCom->Set_Position(XMVectorSet(1035.06592f, 740.905029f, 212.1604f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}

			m_pModelCom->Set_Animation(ANI_M_Lever_Left);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsActivateRobotLever = true;
		}
		else if (m_eTargetGameID == GameID::eVERTICALDOOR && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O))) // 패드입력
		{
			m_pModelCom->Set_Animation(ANI_M_Bounce2); // Trees/DoorInteraction 추출해야함.
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsPullVerticalDoor = true;
		}
		else if (m_eTargetGameID == GameID::eSPACEVALVE && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)) && m_iValvePlayerName == Player::May)
		{
			// Set_Pos
			m_pModelCom->Set_Animation(ANI_M_Valve_Rotate_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Valve_Rotate_MH);
			m_IsEnterValve = true;
		}
		else if (m_eTargetGameID == GameID::eGRAVITYPIPE)
		{

			if (m_IsInGravityPipe == false)
			{
				m_bShortJump = false;
				m_IsJumping = false;
				m_iJumpCount = 0;
				m_pModelCom->Set_Animation(ANI_M_ZeroGravity_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_M_ZeroGravity_MH);
				m_bGoToGravityCenter = true;
				m_vTriggerTargetPos.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 2.f;
			}
			m_IsInGravityPipe = true;
		}
		/* For.PinBall */
		else if (m_eTargetGameID == GameID::ePINBALLHANDLE && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)) && false == m_IsPinBall)
		{
			m_pModelCom->Set_Animation(ANI_M_PinBall_Enter);
			m_pModelCom->Set_Animation(ANI_M_PinBall_MH);

			UI_Generator->Delete_InterActive_UI(Player::May, UI::PinBall_Handle);

			/* 플레이어->핸들방향으로 플레이어 회전 */
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

			_vector vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			_vector vLookDir = XMVector3Normalize(vLook);
			_vector vRightDir = XMVector3Normalize(vRight);

			_float fAngle = acosf(XMVectorGetX(XMVector3Dot(vDir, vLookDir)));

			if (0 > XMVectorGetX(XMVector3Dot(vDir, vRightDir)))
				fAngle *= -1.f;

			_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotateMatrix));
			m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp, RotateMatrix));
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotateMatrix));

			/* 핸들->플레이어 위치로 고정시킴 */
			((CPinBall_Handle*)(CDataStorage::GetInstance()->Get_Pinball_Handle()))->Set_PlayerMove(true);

			/* 플레이어->위치 조정, 핸들 최소, 최대X값 설정 */
			m_vTriggerTargetPos.x -= 0.95f;
			m_vTriggerTargetPos.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_MinMaxX.x = m_vTriggerTargetPos.x;
			m_MinMaxX.y = m_vTriggerTargetPos.x - 5.f;

			_vector vTriggerPos = XMLoadFloat3(&m_vTriggerTargetPos);
			vTriggerPos = XMVectorSetW(vTriggerPos, 1.f);
			m_pActorCom->Set_Position(vTriggerPos);

			m_IsPinBall = true;
		}
		else if (GameID::eWARPGATE == m_eTargetGameID && false == m_IsWarpNextStage)
		{
			// 메이 전용 포탈로 이동(웜홀)
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_fWarpTimer = 0.f;
			m_IsWarpNextStage = true;
			m_IsWarpDone = true;
			XMStoreFloat4x4(&m_TriggerTargetWorld, static_cast<CWarpGate*>(m_pTargetPtr)->Get_NextPortal_Matrix());
			m_pCamera->Set_StartPortalMatrix(static_cast<CWarpGate*>(m_pTargetPtr)->Get_Transform()->Get_WorldMatrix());
		}
		else if (GameID::eFIREDOOR == m_eTargetGameID && false == m_IsTouchFireDoor)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DEAD_BURN, m_fMay_Dead_Burn_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dead_Burn.wav"), CHANNEL_MAY_DEAD_BURN, m_fMay_Dead_Burn_Volume);

			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			Enforce_IdleState();
			m_IsTouchFireDoor = true;
		}
		else if (GameID::eBOSSMISSILE == m_eTargetGameID && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)) && m_IsBossMissile_Control == false)
		{
			m_pModelCom->Set_Animation(ANI_M_Jump_Start, 31.f);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Rocket_Enter);
			m_IsBossMissile_Control = true;
			m_IsMoveToRocket = true;
			m_pActorCom->Jump_Start(3.f);
		}
		else if (m_eTargetGameID == GameID::eWALLLASERTRAP && false == m_IsWallLaserTrap_Touch)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DEAD_BURN, m_fMay_Dead_Burn_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dead_Burn.wav"), CHANNEL_MAY_DEAD_BURN, m_fMay_Dead_Burn_Volume);

			m_pModelCom->Set_Animation(ANI_M_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			Enforce_IdleState();
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_IsWallLaserTrap_Touch = true;
			if (false == m_IsWarpNextStage)
			{
				_vector vWormholePos = XMVectorZero();
			}
		}
		else if (m_eTargetGameID == GameID::eHOOKUFO && (m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)) && m_IsHookUFO == false)
		{
			// 최초 1회 OffSet 조정
			if (m_IsHookUFO == false)
			{
				m_vTriggerTargetPos.y = m_vTriggerTargetPos.y - 5.f;
				m_vHookUFOOffsetPos = XMLoadFloat3(&m_vTriggerTargetPos);
				_vector vPlayerPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
				_vector vTriggerPos = XMVectorSetY(m_vHookUFOOffsetPos, 0.f);
				_vector vPlayerToTrigger = XMVector3Normalize(vTriggerPos - vPlayerPos);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vRight = XMVector3Cross(vPlayerToTrigger, vUp);

				_vector vTestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector vTargetPos = m_vHookUFOOffsetPos;

				_vector vDir = vTargetPos - vTestPos;
				_float  fDist = XMVectorGetX(XMVector3Length(vDir));

				_vector vFixUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vTriggerToPlayer = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_vHookUFOOffsetPos);
				m_fRopeAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vFixUp, vTriggerToPlayer));
				m_faArmLength = fDist;

				//_vector vPosition = XMVectorSet(XMVectorGetX(vTestPos), m_faArmLength * cos(m_fRopeAngle), m_faArmLength * sin(m_fRopeAngle), 1.f) + XMVectorSetW(XMLoadFloat3(&m_vTriggerTargetPos), 1.f);
				XMStoreFloat3(&m_vStartPosition, XMVectorSet(XMVectorGetX(vTestPos), XMVectorGetY(vTestPos), XMVectorGetZ(vTestPos), 1.f)/* + (XMLoadFloat3(&m_vTriggerTargetPos)*/);

			}
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_UFO_THROW, m_fMay_Rope_UFO_Throw_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Throw.wav"), CHANNEL_CHARACTER_UFO_THROW, m_fMay_Rope_UFO_Throw_Volume);

			m_pModelCom->Set_Animation(ANI_M_Swinging_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Swinging_Fwd);
			m_IsHookUFO = true;
			m_bGoToHooker = true;
			m_pActorCom->Set_ZeroGravity(true, false, true);
		}
		else if ((m_eTargetGameID == GameID::eMAYJUMPWALL || m_eTargetGameID == GameID::eDUMMYWALL) && m_pActorCom->Get_IsWallCollide() == true && m_bWallAttach == false
			&& m_IsJumping == true && m_IsFalling == false)
		{
			/*PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
			_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);*/

			m_pModelCom->Set_Animation(ANI_M_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_M_WallSlide_MH);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bWallAttach = true;
		}
		else if (m_eTargetGameID == GameID::eDEADLINE && false == m_IsDeadLine)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_DEAD_FALL, m_fMay_Dead_Fall_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Dead_Fall.wav"), CHANNEL_MAY_DEAD_FALL, m_fMay_Dead_Fall_Volume);

			/* 데드라인 */
			m_pModelCom->Set_Animation(ANI_M_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Death_Fall_MH);

			m_pActorCom->Set_ZeroGravity(true, false, true);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_IsDeadLine = true;
		}
		else if (m_eTargetGameID == GameID::eSAVEPOINT)
		{
			/* 세이브포인트->트리거와 충돌시 세이브포인트 갱신 */
			m_vSavePoint = m_vTriggerTargetPos;
		}
		else if (m_eTargetGameID == GameID::eHOOKAHTUBE)
		{
			/* 튜브*/
			if (2 == ((CHookahTube*)m_pTargetPtr)->Get_Option())
				m_pActorCom->Jump_Start(6.f);
			else
				m_pActorCom->Jump_Start(4.f);

			_uint iRandom = rand() % 4;
			switch (iRandom)
			{
			case 0:
				m_pModelCom->Set_Animation(ANI_M_Bounce1);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
				m_IsJumping = true;
				break;
			case 1:
				m_pModelCom->Set_Animation(ANI_M_Bounce2);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
				m_IsJumping = true;
				break;
			case 2:
				m_pModelCom->Set_Animation(ANI_M_Bounce3);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
				m_IsJumping = true;
				break;
			case 3:
				m_pModelCom->Set_Animation(ANI_M_Bounce4);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Falling);
				m_IsJumping = true;
				break;
			default:
				break;
			}
			m_IsCollide = false;
		}
		else if (m_eTargetGameID == GameID::eBOSSUFO && true == m_IsLaserRippedOff)
		{
			if (true == m_pModelCom->Is_AnimFinished(ANI_M_SpaceStation_BossFight_LaserRippedOff))
			{
				m_IsRippedOffAnimPlaying = false;
				m_IsLaserRippedOff = false;
			}

			if (m_pGameInstance->Key_Down(DIK_O) || m_pGameInstance->Pad_Key_Down(DIP_Y))
			{
				m_IsRippedOffAnimPlaying = true;
				m_pModelCom->Set_Animation(ANI_M_SpaceStation_BossFight_LaserRippedOff);
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
				m_pActorCom->Set_Position(XMVectorSet(60.9975f, 342.838f, 199.3799f, 1.f));
				((CUFO*)DATABASE->Get_BossUFO())->Set_UFOAnimation(UFO_LaserRippedOff, UFO_Left);
				((CCody*)DATABASE->GetCody())->Set_AnimationRotate(190.f);
				((CCody*)DATABASE->GetCody())->Get_Model()->Set_Animation(ANI_C_CutScene_BossFight_LaserRippedOff);
				((CCody*)DATABASE->GetCody())->Get_Model()->Set_NextAnimIndex(ANI_C_MH);
			}
		}
		else if (m_eTargetGameID == GameID::eLASERTENNISPOWERCOORD && m_pGameInstance->Key_Down(DIK_O) && false == m_bLaserTennis)
		{
			m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
			m_pGameInstance->Play_Sound(TEXT("StartButton_Touch&Detach.wav"), CHANNEL_LASERPOWERCOORD);

			LASERTENNIS->Increase_PowerCoord();

			UI_Generator->Delete_InterActive_UI(Player::May, UI::PowerCoord);

			m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
			m_pActorCom->Set_Position(XMVectorSet(m_vTriggerTargetPos.x, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), m_vTriggerTargetPos.z - 3.f, 1.f));

			m_pModelCom->Set_Animation(ANI_M_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

			m_bLaserTennis = true;
		}
		else if (m_eTargetGameID == GameID::eLASER_LASERTENNIS)
		{
			/* Hit Effect 생성 */

			/* HP 감소 */
			LASERTENNIS->Set_CodyCount();
			Set_MinigameHpBarReduction(30);

			m_IsCollide = false;
		}
		else if (m_IsEnding == true && m_bSetEndingOffSetOnce == false)
		{
			m_bSetEndingOffSetOnce = true;
			m_pModelCom->Set_Animation(ANI_M_Rocket_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Rocket_MH);
		}
	}

	// Trigger 여따가 싹다모아~
	if (m_bOnRailEnd || m_IsOnGrind || m_IsHitStarBuddy || m_IsHitRocket || m_IsActivateRobotLever || m_IsPullVerticalDoor || m_IsEnterValve || m_IsInGravityPipe || m_IsPinBall || m_IsDeadLine
		|| m_IsWarpNextStage || m_IsWarpDone || m_IsTouchFireDoor || m_IsHookUFO || m_IsBossMissile_Control || m_IsWallLaserTrap_Touch || m_bWallAttach || 
		m_IsRippedOffAnimPlaying || m_bLaserTennis || m_IsEnding)
		return true;

	return false;
}

_bool CMay::Trigger_End(const _double dTimeDelta)
{
	//if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jump_Land ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_RocketFirework ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_BruteCombat_Attack_Var1 ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_Lever_Left ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_Valve_Rotate_MH ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_PinBall_MH ||
	//	m_pModelCom->Get_CurAnimIndex() == ANI_M_Pull)
	//{
	//	m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
	//}

	if (m_pModelCom->Is_AnimFinished(ANI_M_Jump_Land_High))
	{
		m_pModelCom->Set_Animation(ANI_M_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		m_bLandHigh = false;
	}

	if (ANI_M_Jump_Land_High == m_pModelCom->Get_CurAnimIndex())
	{
		m_bLandHigh = true;
	}

	return false;
}
#pragma endregion

void CMay::Hit_StarBuddy(const _double dTimeDelta)
{
	if (m_IsHitStarBuddy == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));

		if (m_pModelCom->Is_AnimFinished(ANI_M_Lever_Left))
		{
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_IsHitStarBuddy = false;
			m_IsCollide = false;
		}
	}
}
void CMay::Hit_Rocket(const _double dTimeDelta)
{
	if (m_IsHitRocket == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pModelCom->Is_AnimFinished(ANI_M_RocketFirework))
		{
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_IsHitRocket = false;
			m_IsCollide = false;
		}
	}
}

void CMay::Activate_RobotLever(const _double dTimeDelta)
{
	if (m_IsActivateRobotLever == true)
	{
		//m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pModelCom->Is_AnimFinished(ANI_M_Lever_Left))
		{
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_IsActivateRobotLever = false;
			m_IsCollide = false;
		}
	}
}

void CMay::Pull_VerticalDoor(const _double dTimeDelta)
{
	if (false == m_IsPullVerticalDoor)
		return;

	_bool IsTriggerEnd = false;
	if (m_pGameInstance->Pad_Key_Down(DIP_LB) || m_pGameInstance->Key_Down(DIK_I))
		IsTriggerEnd = true;

	if (m_IsPullVerticalDoor == true)
	{
		m_pModelCom->Set_Animation(ANI_M_Pull);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Pull);

		_vector vSwitchPos = XMLoadFloat3(&m_vTriggerTargetPos);
		vSwitchPos.m128_f32[3] = 1.f;
		vSwitchPos.m128_f32[2] -= 1.f;
		vSwitchPos.m128_f32[1] += 12.f;
		m_pActorCom->Set_ZeroGravity(true, false, true);
		m_pActorCom->Set_Position(vSwitchPos);

		m_pTransformCom->RotateYawDirectionOnLand(XMVectorSet(0.f, 0.f, 1.f, 0.f), dTimeDelta);

		if (true == IsTriggerEnd)
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_M_Jump_Land);
			m_IsCollide = false;
			m_IsPullVerticalDoor = false;
		}
	}
}

void CMay::Rotate_Valve(const _double dTimeDelta)
{

	if (m_IsEnterValve == true)
	{
		m_pActorCom->Set_Position(XMVectorSet(80.7704391f, 125.251999f, 195.823730f, 1.f));
		if (DATABASE->Get_ValveCount() == 6)
		{
			m_bStruggle = false;
			m_iRotateCount = 0;
			m_IsEnterValve = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_M_MH);
			DATABASE->Add_ValveCount_Cody(false);
			DATABASE->Set_Valve_Activate(true);
		}

		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if ((m_pGameInstance->Pad_Key_Down(DIP_RB) || m_pGameInstance->Key_Down(DIK_RIGHT)) && m_pModelCom->Get_CurAnimIndex() != ANI_M_Valve_Rotate_R && m_bStruggle == false)
		{
			m_pModelCom->Set_Animation(ANI_M_Valve_Rotate_R);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Valve_Rotate_MH);
			m_iRotateCount += 1;
			DATABASE->Add_ValveCount_Cody(false);
		}
		if (m_pModelCom->Is_AnimFinished(ANI_M_Valve_Rotate_R))
		{
			m_pModelCom->Set_Animation(ANI_M_Valve_Rotate_MH);
			//m_IsEnterValve = false;
			//m_IsCollide = false;
			if (m_iRotateCount == 3)
			{
				m_bStruggle = true;
				m_pModelCom->Set_Animation(ANI_M_Valve_StruggleRightEnter);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Valve_StruggleRight);
				DATABASE->Set_ValveCount(m_iRotateCount);
			}
		}
	}
}

void CMay::In_GravityPipe(const _double dTimeDelta)
{
	if (m_bGoToGravityCenter == true)
	{
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vTargetPos = XMLoadFloat3(&m_vTriggerTargetPos);

		_vector vDir = vTargetPos - vPlayerPos;
		_float  fDist = XMVectorGetX(XMVector3Length(vDir));
		_float  fEpsilon = 0.2f;

		if (fDist <= m_vTriggerTargetPos.y + fEpsilon)
		{
			m_bGoToGravityCenter = false;
		}
		m_pActorCom->Move(XMVector3Normalize(vDir) * fDist / 25.f, dTimeDelta);
	}
	else if (m_bGoToGravityCenter == false)
	{
		if (m_IsInGravityPipe && m_IsCollide == true)
		{
			m_pActorCom->Set_ZeroGravity(true, true, true);
			if (m_pGameInstance->Pad_Key_Pressing(DIP_B) || m_pGameInstance->Key_Pressing(DIK_K))
			{
				m_pActorCom->Set_ZeroGravity(true, true, false);

				if (m_bGravityPipe_FirstIn == false)
				{
					SCRIPT->Render_Script(1, CScript::HALF, 2.f);
					m_pGameInstance->Set_SoundVolume(CHANNEL_VOICE_MAY_1, m_fMay_GravityPipe_Voice_Volume);
					m_pGameInstance->Play_Sound(TEXT("02.wav"), CHANNEL_VOICE_MAY_1, m_fMay_GravityPipe_Voice_Volume);
					m_bGravityPipe_FirstIn = true;
				}
			}

			if (m_pGameInstance->Pad_Key_Pressing(DIP_A) || m_pGameInstance->Key_Pressing(DIK_SEMICOLON))
			{
				m_pActorCom->Set_ZeroGravity(true, false, false);
			}
#ifdef __CONTROL_MAY_KEYBOARD
			if (m_pGameInstance->Key_Pressing(DIK_UP))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if ( m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Key_Pressing(DIK_DOWN))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
#elif
			if (m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Key_Pressing(DIK_UP))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Key_Pressing(DIK_LEFT))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Get_Pad_LStickY() > 44000 || m_pGameInstance->Key_Pressing(DIK_DOWN))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Key_Pressing(DIK_RIGHT))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
#endif
		}
		else if (m_IsInGravityPipe == true && m_IsCollide == false)
		{
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			m_IsFalling = true;
			m_IsJumping = false;
			m_bShortJump = false;
			m_bMove = false;
			m_bGroundPound = false;
			m_bRoll = false;

			m_pActorCom->Set_IsFalling(true);
			m_pActorCom->Jump_Start(1.5f);
			m_pActorCom->Set_Jump(true);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pActorCom->Set_Gravity(-9.8f);
			m_IsInGravityPipe = false;
			m_pModelCom->Set_Animation(ANI_M_Jump_180R);
		}
	}
}

void CMay::PinBall(const _double dTimeDelta)
{
	if (true == m_IsPinBall)
	{
		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vLeft = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		/* 공이 죽었을 때 or 골인 했을 때 */
		if (true == ((CPinBall*)(CDataStorage::GetInstance()->Get_Pinball()))->Get_Failed()/* || m_pGameInstance->Key_Down(DIK_R)*/)
		{
			m_pModelCom->Set_Animation(ANI_M_PinBall_Exit);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

			m_IsPinBall = false;
			m_IsCollide = false;
			return;
		}

		/* 벽 올리고 내리고 */
		if (m_pGameInstance->Key_Down(DIK_RBRACKET) || m_pGameInstance->Pad_Key_Down(DIP_RB))
		{
			/* Sound */
			m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_HANDLE);
			m_pGameInstance->Play_Sound(TEXT("Pinball_Wall_Change.wav"), CHANNEL_PINBALL_HANDLE);

			m_pModelCom->Set_Animation(ANI_M_PinBall_Right_Hit);
			m_pModelCom->Set_NextAnimIndex(ANI_M_PinBall_MH_Hit);
			((CPInBall_Blocked*)(CDataStorage::GetInstance()->Get_Pinball_Blocked()))->Switching();
		}

		if (false == ((CPinBall*)(CDataStorage::GetInstance()->Get_Pinball()))->Get_StartGame())
		{
			/* 공 발사 */
			if (m_pGameInstance->Key_Down(DIK_LBRACKET) || m_pGameInstance->Pad_Key_Down(DIP_LB))
			{
				/* Sound */
				m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_HANDLE);
				m_pGameInstance->Play_Sound(TEXT("Pinball_Shooter_Shot.wav"), CHANNEL_PINBALL_HANDLE);

				m_pModelCom->Set_Animation(ANI_M_PinBall_Left_Hit);
				m_pModelCom->Set_NextAnimIndex(ANI_M_PinBall_MH_Hit);

				((CPinBall*)(CDataStorage::GetInstance()->Get_Pinball()))->StartGame();
				((CPinBall_Handle*)(CDataStorage::GetInstance()->Get_Pinball_Handle()))->Set_PlayerMove(false);
			}
			/* 오른쪽 */
			if (m_pGameInstance->Key_Pressing(DIK_RIGHT)/* || m_pGameInstance->Get_Pad_LStickX() > 40000*/)
			{
				if (false == m_IsPinBallSoundCheck)
				{
					m_pGameInstance->Play_Sound(TEXT("Pinball_Shooter_Move.wav"), CHANNEL_PINBALL_HANDLEMOVE, 1.f);
					m_IsPinBallSoundCheck = true;
				}

				m_pModelCom->Set_Animation(ANI_M_PinBall_Right);
				m_pModelCom->Set_NextAnimIndex(ANI_M_PinBall_MH);

				/* x값 범위 내에서 움직임 */
				if (m_MinMaxX.y <= XMVectorGetX(vPosition))
					m_pActorCom->Move(vLeft * 0.05f, dTimeDelta);
			}
			/* 왼쪽 */
			else if (m_pGameInstance->Key_Pressing(DIK_LEFT)/* || m_pGameInstance->Get_Pad_LStickX() < 20000*/)
			{
				/* Sound */
				if (false == m_IsPinBallSoundCheck)
				{
					m_pGameInstance->Play_Sound(TEXT("Pinball_Shooter_Move.wav"), CHANNEL_PINBALL_HANDLEMOVE, 1.f);
					m_IsPinBallSoundCheck = true;
				}

				m_pModelCom->Set_Animation(ANI_M_PinBall_Left);
				m_pModelCom->Set_NextAnimIndex(ANI_M_PinBall_MH);

				/* x값 범위 내에서 움직임 */
				if (m_MinMaxX.x >= XMVectorGetX(vPosition))
					m_pActorCom->Move(vRight * 0.05f, dTimeDelta);
			}
			else
			{
				m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_HANDLEMOVE);
				m_IsPinBallSoundCheck = false;

				m_pModelCom->Set_Animation(ANI_M_PinBall_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_M_PinBall_MH);
			}
		}
	}
}

void CMay::InUFO(const _double dTimeDelta)
{
	if (false == m_IsInUFO)
		return;
	 
	if (m_pGameInstance->Key_Down(DIK_Y))/* 메이 우주선 내리기 */
	{
		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
		Set_UFO(false);
		return;
	}

	CModel* pUFOModel = ((CMoonUFO*)(DATABASE->Get_MoonUFO()))->Get_Model();
	CTransform* pUFOTransform = ((CMoonUFO*)(DATABASE->Get_MoonUFO()))->Get_Transform();

	_matrix BoneChair = pUFOModel->Get_BoneMatrix("Chair");
	_float4x4 matWorld, matScale;
	XMStoreFloat4x4(&matWorld, BoneChair * pUFOTransform->Get_WorldMatrix());

	_float4 vChairPos = {};
	memcpy(&vChairPos, &(matWorld._41), sizeof(_float4));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vChairPos));

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	vUp = XMVector3Normalize(vPosition - ((CMoon*)(DATABASE->Get_Mooon()))->Get_Position()) * 0.8f;
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp)) * 0.8f;
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * 0.8f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
}

void CMay::Set_ActiveHpBar(_bool IsCheck)
{
	if (nullptr == m_pHpBar)
		return;

	m_pHpBar->Set_Active(IsCheck);
}

void CMay::Set_HpBarReduction(_float fDamage)
{
	if (nullptr == m_pHpBar || nullptr == m_pSubHpBar)
		return;

	m_pHpBar->Set_Hp(fDamage);
	m_pSubHpBar->Set_Active(true);
	m_pSubHpBar->Set_Hp(fDamage);
}

void CMay::Set_ActiveMinigameHpBar(_bool IsCheck)
{
	if (nullptr == m_pMinigameHpBar)
		return;

	m_pMinigameHpBar->Set_Active(IsCheck);
}

void CMay::Set_MinigameHpBarReduction(_float fDamage)
{
	if (nullptr == m_pMinigameHpBar || nullptr == m_pMinigameSubHpBar)
		return;

	m_pMinigameHpBar->Set_Hp(fDamage);
	m_pMinigameSubHpBar->Set_Active(true);
	m_pMinigameSubHpBar->Set_Hp(fDamage);
}


void CMay::LaserTennis(const _double dTimeDelta)
{
	if (false == m_bLaserTennis)
		return;

	if (true == LASERTENNIS->Get_StartGame())
	{
		m_pActorCom->Jump_Start(2.f);
		m_pModelCom->Set_Animation(ANI_M_RocketFirework);
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

		m_bLaserTennis = false;
	}

	if (m_pGameInstance->Key_Down(DIK_I))
	{
		m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
		m_pGameInstance->Play_Sound(TEXT("StartButton_Touch&Detach.wav"), CHANNEL_LASERPOWERCOORD);

		LASERTENNIS->Decrease_PowerCoord();
		m_bLaserTennis = false;
	}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		UI_Generator->Delete_InterActive_UI(Player::May, UI::PowerCoord);
		LASERTENNIS->KeyCheck(CLaserTennis_Manager::TARGET_MAY);
	}
}

void CMay::Set_UFO(_bool bCheck)
{
	/* 중력끄고, 충돌끄고 */
	m_IsInUFO = bCheck;
	m_pActorCom->Set_ZeroGravity(bCheck, false, bCheck);
	m_pActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, bCheck);

	m_pModelCom->Set_Animation(ANI_M_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

	((CMoonUFO*)(DATABASE->Get_MoonUFO()))->Set_MayInUFO(bCheck);
}

void CMay::Warp_Wormhole(const _double dTimeDelta)
{
	if (false == m_IsWarpNextStage && false == m_IsWarpDone)
		return;

	m_fWarpTimer += (_float)dTimeDelta;

	if (true == m_IsWarpNextStage)
	{
		if (m_fWarpTimer_InWormhole <= m_fWarpTimer)
		{
			_float4 vWormhole = m_vWormholePos;
			vWormhole.z -= 1.f;
			m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat4(&vWormhole));

			m_pModelCom->Set_Animation(ANI_M_SpacePortal_Travel_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_SpacePortal_Travel_MH);

			m_pActorCom->Set_Position(XMLoadFloat4(&m_vWormholePos));
		}

		if (m_fWarpTimer_InWormhole + m_fWarpTimer_Max <= m_fWarpTimer)
		{
			m_pModelCom->Set_Animation(ANI_M_SpacePortal_Travel_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsWarpNextStage = false;

			_vector vNextStage_Pos = XMLoadFloat4x4(&m_TriggerTargetWorld).r[3];
			vNextStage_Pos.m128_f32[3] = 1.f;

			m_pActorCom->Set_Position(vNextStage_Pos);

			_matrix PortalMatrix = XMLoadFloat4x4(&m_TriggerTargetWorld);
			_vector vTriggerPos = PortalMatrix.r[3];
			_vector vLook = PortalMatrix.r[2];
			vTriggerPos += vLook * 20.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vTriggerPos);
			m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 0.f));

		}
	}
	else
	{
		_matrix PortalMatrix = XMLoadFloat4x4(&m_TriggerTargetWorld);
		_vector vTriggerPos = PortalMatrix.r[3];
		_vector vLook = PortalMatrix.r[2];
		vTriggerPos += vLook * 30.f;
		m_pTransformCom->Rotate_ToTargetOnLand(vTriggerPos);
		m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 0.f));

		// 슈루룩
		if (m_fWarpTimer_InWormhole + m_fWarpTimer_Max + 0.25f >= m_fWarpTimer)
		{
			_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vDir = XMVector3Normalize(vDir);
			m_pActorCom->Move(vDir * 0.5f, dTimeDelta);
		}
		else
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_IsWarpDone = false;
		}
	}
}

void CMay::Touch_FireDoor(const _double dTimeDelta)
{
	if (false == m_IsTouchFireDoor)
		return;

	m_fDeadTime += (_float)dTimeDelta;
	if (m_fDeadTime >= 2.f && m_fDeadTime <= 2.4f)
	{
		_float fMyPosZ = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];
		_float fTriggerPosZ = m_vTriggerTargetPos.z;

		_vector vSavePosition = XMLoadFloat3(&m_vSavePoint);
		if (fTriggerPosZ < fMyPosZ)
		{
			vSavePosition.m128_f32[1] += 0.7f;
			vSavePosition = XMVectorSetW(vSavePosition, 1.f);
		}
		else
			vSavePosition = XMVectorSet(64.f, 0.9f, 25.f, 1.f);

		m_pActorCom->Set_Position(vSavePosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);

		m_fDeadTime = 2.5f;
	}
	else if (m_fDeadTime >= 2.5f && m_fDeadTime <= 2.75f)
	{

	}
	else if (m_fDeadTime >= 2.75f)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_Resurrection.wav"), CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);

		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_M_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		m_fDeadTime = 0.f;
		m_bCanMove = true;
		m_IsCollide = false;
		m_IsTouchFireDoor = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);
	}
}

void CMay::BossMissile_Control(const _double dTimeDelta)
{
	if (m_IsBossMissile_Control == true && m_bEscapeFromRocket == false)
	{

		if (m_IsMoveToRocket = true)
		{
			_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vRocketPos = m_vRocketOffSetPos;
			_vector vRocketLook = m_matRocketMatrix.r[2];
			vRocketPos += XMVector3Normalize(vRocketLook) * 0.1f;

			_vector vDir = vRocketPos - vPlayerPos;

			_vector vNormDir = XMVector3Normalize(vDir);
			_float fLength = XMVectorGetX(XMVector3Length(vDir));

			m_pTransformCom->RotateYawDirectionOnLand(XMVectorSetY(vRocketLook, 0.f), dTimeDelta);
			m_pActorCom->Move((vNormDir) / 10.f * fLength, dTimeDelta);

			if (fLength <= 0.12f)
			{
				m_pActorCom->Set_ZeroGravity(true, false, true);
				m_IsMoveToRocket = false;
				m_pActorCom->Set_Position(m_vRocketOffSetPos);
			}

		}

		if (m_pModelCom->Is_AnimFinished(ANI_M_Rocket_Enter))
		{
			m_pModelCom->Set_Animation(ANI_M_Rocket_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Rocket_MH);
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Rocket_MH)
		{
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_pActorCom->Set_Position(m_vRocketOffSetPos);
			m_pTransformCom->Set_WorldMatrix(m_matRocketMatrix);
		}
	}
	if (m_bEscapeFromRocket == true)
	{
		// 로켓으로 부터 탈출!
		m_pActorCom->Set_ZeroGravity(false, false, false);
		m_IsMoveToRocket = false;
		m_bEscapeFromRocket = false;
		m_IsBossMissile_Control = false;

		m_pActorCom->Jump_Start(3.5f);
		m_pModelCom->Set_Animation(ANI_M_Rocket_Exit);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_Land_High);
	}
}

void CMay::Ride_Ending_Rocket(const _double dTimeDelta)
{
	if (m_IsEnding)
	{
		m_pModelCom->Set_Animation(ANI_M_Rocket_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Rocket_MH);

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Rocket_MH)
		{
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_pActorCom->Set_Position(m_vEndingRocketOffSetPos);
			m_pTransformCom->Set_WorldMatrix(m_matEndingRocketMatrix);
		}
	}
}

void CMay::WallLaserTrap(const _double dTimeDelta)
{
	if (false == m_IsWallLaserTrap_Touch)
		return;

	m_IsWallLaserTrap_Effect = true;
	m_fDeadTime += (_float)dTimeDelta;
	if (m_fDeadTime >= 2.f)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);
		m_pGameInstance->Play_Sound(TEXT("May_Resurrection.wav"), CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);

		_vector vSavePosition = XMVectorSet(-803.32f, 789.125f, 189.37f, 1.f);

		m_pActorCom->Set_Position(vSavePosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_M_MH);
		m_fDeadTime = 0.f;
		m_IsCollide = false;
		m_IsWallLaserTrap_Touch = false;
		m_IsWallLaserTrap_Effect = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);
	}
}

void CMay::Set_LaserRippedOff()
{
	m_IsLaserRippedOff = true;
}

void CMay::Set_ActorPosition(_vector vPosition)
{
	m_pActorCom->Set_Position(vPosition);
}

void CMay::Set_ActorGravity(_bool IsZeroGravity, _bool IsUp, _bool _bStatic)
{
	m_pActorCom->Set_ZeroGravity(IsZeroGravity, IsUp, _bStatic);
}

void CMay::Falling_Dead(const _double dTimeDelta)
{
	/* 데드라인과 충돌시 1초후에 리스폰 */
	if (m_IsDeadLine == true)
	{
		m_fDeadTime += (_float)dTimeDelta;
		if (m_fDeadTime >= 1.f)
		{

			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_Resurrection.wav"), CHANNEL_MAY_RESURRECTION, m_fMay_Resurrection_Volume);

			_vector vSavePosition = XMLoadFloat3(&m_vSavePoint);
			vSavePosition = XMVectorSetW(vSavePosition, 1.f);

			m_pActorCom->Set_Position(vSavePosition);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_fDeadTime = 0.f;
			m_IsCollide = false;
			m_IsDeadLine = false;
			m_pActorCom->Set_ZeroGravity(false, false, false);
			Enforce_IdleState();
		}
		else
		{
			_vector vTriggerTargetPos = XMLoadFloat3(&m_vTriggerTargetPos);
			vTriggerTargetPos = XMVectorSetW(vTriggerTargetPos, 1.f);
			m_pActorCom->Set_Position(vTriggerTargetPos);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTriggerTargetPos);
		}
	}
}

void CMay::Hook_UFO(const _double dTimeDelta)
{
	if (m_IsHookUFO == true)
	{

		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_UFO_THROW) == false && m_bUFOCatchSoundOnce == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fMay_Rope_UFO_Throw_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Catch.wav"), CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fMay_Rope_UFO_Throw_Volume);
			m_bUFOCatchSoundOnce = true;
		}

		_float Gravity = -0.3f;

		// ZY
		m_faAcceleration = (-1.f * Gravity / m_faArmLength) * sin(m_fRopeAngle);
		if (m_pGameInstance->Get_Pad_LStickY() > 44000 || m_pGameInstance->Key_Pressing(DIK_UP))
			m_faAcceleration += (_float)dTimeDelta;
		if (m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Key_Pressing(DIK_DOWN))
			m_faAcceleration -= (_float)dTimeDelta;
		m_faVelocity += m_faAcceleration;
		m_faVelocity *= m_faDamping;
		m_fRopeAngle += m_faVelocity / 50.f;


		_vector vPosition = XMVectorSet((m_vHookUFOOffsetPos.m128_f32[0] - m_vStartPosition.x) * sin(-m_fRopeAngle), (m_vHookUFOOffsetPos.m128_f32[1] - m_vStartPosition.y) * cos(m_fRopeAngle), ((m_vHookUFOOffsetPos.m128_f32[2] - m_vStartPosition.z) * sin(-m_fRopeAngle)), 1.f);
		m_pActorCom->Set_Position(m_vHookUFOOffsetPos + vPosition);

		_vector vTriggerToPlayer = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f) - XMVectorSetY(m_vHookUFOOffsetPos, 0.f));
		vTriggerToPlayer = XMVectorSetW(vTriggerToPlayer, 1.f);
		m_pTransformCom->RotateYawDirectionOnLand(-vTriggerToPlayer, (_float)dTimeDelta / 2.f);



		////////////////////////////////////////


		if (m_pGameInstance->Pad_Key_Down(DIP_B) || m_pGameInstance->Key_Down(DIK_K)) // 로프 놓기
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_UFO_RELEASE, m_fMay_Rope_UFO_Release_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Release.wav"), CHANNEL_CHARACTER_UFO_RELEASE, m_fMay_Rope_UFO_Release_Volume);

			m_bGoToHooker = false;
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			m_pModelCom->Set_Animation(ANI_M_Swinging_Exit_Fwd);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jump_180R);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pActorCom->Set_Gravity(-9.8f);
			m_pActorCom->Set_IsFalling(true);
			m_pActorCom->Jump_Start(3.5f);
			m_pActorCom->Set_Jump(true);
			m_IsHookUFO = false;
			m_IsCollide = false;
			((CHookUFO*)DATABASE->Get_HookUFO())->Set_MayUIDisable();
		}
	}
}

void CMay::Wall_Jump(const _double dTimeDelta)
{
	if (true == m_bWallAttach && false == m_IsWallJumping)
	{

		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_WALLJUMP_SLIDE) == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fMay_Wall_Slide_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fMay_Wall_Slide_Volume);
		}
		m_pActorCom->Move((-m_pTransformCom->Get_State(CTransform::STATE_UP) / 50.f), dTimeDelta);
		if (m_pGameInstance->Pad_Key_Down(DIP_B) || m_pGameInstance->Key_Down(DIK_K))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);
			m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_WALLJUMP, m_fMay_WallJump_Volume);
			m_pGameInstance->Play_Sound(TEXT("May_WallJump.wav"), CHANNEL_MAY_WALLJUMP, m_fMay_WallJump_Volume );

			m_pTransformCom->RotateYaw(XMConvertToRadians(-180.f));
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_IsWallJumping = true;
			m_pModelCom->Set_Animation(ANI_M_WallSlide_Jump);
			m_pActorCom->Jump_Start(2.7f);
			m_pActorCom->Set_WallCollide(false);
		}
	}

	if (m_IsWallJumping == true)
	{
		if (m_fWallToWallSpeed <= 50.f)
			m_fWallToWallSpeed += (_float)dTimeDelta * 57.f;

		PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
		_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
		m_pActorCom->Move(XMVector3Normalize(vWallUp) / m_fWallToWallSpeed, dTimeDelta);
		m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);

		if (m_pModelCom->Is_AnimFinished(ANI_M_WallSlide_Jump))
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_M_Jump_Falling);
			m_bWallAttach = false;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.55f;
			//m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-179.f));
		}
		if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == true)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[0] = 0.f;
			m_pTransformCom->RotateYaw(XMConvertToRadians(-180.f));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bWallAttach = true;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.55f;
			m_pModelCom->Set_Animation(ANI_M_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_M_WallSlide_MH);
		}
		else if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == false)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[0] = 0.f;
			m_pTransformCom->RotateYaw(XMConvertToRadians(-180.f));
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_bWallAttach = false;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.5f;
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_pActorCom->Set_WallCollide(false);
		}

		m_fWallJumpingTime += (_float)dTimeDelta;
	}
}

#pragma region Path
void CMay::Set_SpaceRailNode(CSpaceRail_Node* pRail)
{
	if (nullptr == pRail) return;
	m_vecTargetRailNodes.push_back(pRail);
}
void CMay::KeyInput_Rail(_double dTimeDelta)
{
	if ((m_pGameInstance->Pad_Key_Down(DIP_Y) || m_pGameInstance->Key_Down(DIK_O)) && false == m_bOnRail && false == m_IsDeadLine)
	{
		Start_SpaceRail();
		UI_Delete(May, InputButton_InterActive_Rail);
	}

	if (m_bOnRail)
	{
		if (m_pGameInstance->Pad_Key_Down(DIP_B) || m_pGameInstance->Key_Down(DIK_K))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_MAY_RAIL);

			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			Loop_RadiarBlur(false);

			m_iJumpCount = 0;
			m_bShortJump = true;

			m_pTargetRail = nullptr;
			m_pSearchTargetRailNode = nullptr;
			m_pTargetRailNode = nullptr;

			m_vecTargetRailNodes.clear();

			m_bMoveToRail = false;
			m_bOnRail = false;
		}
	}
}
void CMay::Clear_TagerRailNodes()
{
	m_vecTargetRailNodes.clear();
}
void CMay::Find_TargetSpaceRail()
{
	// 레일타기 키 눌렸을때만, 타겟 찾기, 키가 눌렸지만, 충돌한 레일 트리거가 존재하지 않을때
	if (m_vecTargetRailNodes.empty()) {
		m_pSearchTargetRailNode = nullptr;
		return;
	}

	CTransform* pCamTransform = m_pCamera->Get_Transform();
	if (nullptr == pCamTransform) return;
	if (nullptr == m_pTransformCom) return;

	// 카메라와 플레이어 룩벡터 비교해서 왼쪽 오른쪽 구분하기
	_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	// 거리가 아닌, 카메라 Look 벡터와의 각도차로 계산
	vCamLook = XMVector3Normalize(vCamLook);
	vCamLook = XMVectorSetY(vCamLook, 0.f); // Cam Look 수평으로 맞추기
	vPlayerUp = XMVector3Normalize(vPlayerUp);
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	_bool isSearch = false;
	_vector vNodePosition = XMVectorZero();
	_float fMinDist = FLT_MAX;
	for (auto& pNode : m_vecTargetRailNodes)
	{
		if (nullptr == pNode) continue;

		vNodePosition = pNode->Get_Position();

		/* 외적 : 방향 체크 */
		_vector vToTarget = XMVector3Normalize(vNodePosition - vCamPos);
		vToTarget = XMVectorSetY(vToTarget, 0.f);

		/* @Return CCW(1 : 오른쪽) CW(-1 : 왼쪽) Else(0) */
		_int iCCW_Std = MH_CrossCCW(vCamLook, vPlayerLook, vPlayerUp);
		_int iCCW_Cmp = MH_CrossCCW(vCamLook, vToTarget, vPlayerUp);

		if (1 == iCCW_Std && -1 == iCCW_Cmp) /* 오른쪽 노드만 검색 */
			continue;
		else if (-1 == iCCW_Std && 1 == iCCW_Cmp)/* 왼쪽 노드만 검색*/
			continue;

		/* 가장 가까운 노드 찾기  */
		_float fDist = XMVectorGetX(XMVector3Length(vNodePosition - vCamPos));

		/* 가장 각도가 적은 타겟노드 찾기 */
		if (fMinDist > fDist)
		{
			m_pSearchTargetRailNode = pNode;
			fMinDist = fDist;
			isSearch = true;
		}
	}

	if (false == isSearch)
		m_pSearchTargetRailNode = nullptr;
}

void CMay::Start_SpaceRail()
{
	if (nullptr == m_pSearchTargetRailNode) return;

	if (m_pSearchTargetRailNode) {
		// 타겟 지정시, 연기이펙트
		EFFECT->Add_Effect(Effect_Value::Landing_Smoke, m_pSearchTargetRailNode->Get_WorldMatrix());
		// R-Blur
		Loop_RadiarBlur(true);

		// 타겟을 찾았다면, 레일 탈 준비
		m_pTargetRailNode = m_pSearchTargetRailNode;
		m_pModelCom->Set_Animation(ANI_M_Grind_Grapple_Enter); // 줄던지고 댕겨서 날라가기
		m_bMoveToRail = true;
	}
}
void CMay::MoveToTargetRail(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom || false == m_bMoveToRail || nullptr == m_pTargetRailNode || true == m_bOnRail) return;

	m_pModelCom->Set_NextAnimIndex(ANI_M_Grind_Grapple_Enter); // 레일 착지

	_float fMoveToSpeed = 5.f;
	_float fDist = m_pTransformCom->Move_ToTargetRange(m_pTargetRailNode->Get_Position(), 0.1f, dTimeDelta * fMoveToSpeed);
	if (fDist < 0.15f)
	{
		/* 타는 애니메이션으로 변경 */
		m_pModelCom->Set_Animation(ANI_M_Grind_Grapple_ToGrind);
		m_pModelCom->Set_NextAnimIndex(ANI_M_Grind_Slow_MH);

		/* 타야할 Path 지정 */
		m_pTargetRail = (CSpaceRail*)DATABASE->Get_SpaceRail(m_pTargetRailNode->Get_RailTag());

		/* 예외처리 - 레일 태그 틀렸을 경우 */
		if (nullptr == m_pTargetRail)
		{
			m_pTargetRailNode = nullptr;
			m_bOnRail = false;
			m_bMoveToRail = false;
			return;
		}

		/* Edge Type에 따라서 Process / Rewind 결정 */
		CSpaceRail::EDGE eEdgeState = (CSpaceRail::EDGE)m_pTargetRailNode->Get_EdgeState();

		CPath::STATE ePathState = CPath::STATE_END;
		if (CSpaceRail::EDGE_FIRST == eEdgeState || CSpaceRail::EDGE_FIRST_END == eEdgeState)
			ePathState = CPath::STATE_FORWARD;
		else if (CSpaceRail::EDGE_LAST == eEdgeState || CSpaceRail::EDGE_LAST_END == eEdgeState)
			ePathState = CPath::STATE_BACKWARD;

		m_pTargetRail->Start_Path(CSpaceRail::SUBJ_MAY, ePathState, m_pTargetRailNode->Get_FrameIndex(), true);

		/* 카메라가 레일타는 방향으로 세팅 */
		//m_pCamera->Get_Transform()->Set_State();

		/* 레일 앞까지 도착, 레일 타기 시작 */
		m_pTargetRailNode = nullptr;
		m_bOnRail = true;
		m_bMoveToRail = false;
		EFFECT->Add_Effect(Effect_Value::May_Rail, m_pTransformCom->Get_WorldMatrix());

		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RAIL, m_fRailSoundVolume);
		m_pGameInstance->Play_Sound(TEXT("Rail_Ride.wav"), CHANNEL_MAY_RAIL, m_fRailSoundVolume, true);
	}
}

void CMay::TakeRail(_double dTimeDelta)
{
	if (nullptr == m_pTargetRail || false == m_bOnRail) return;

	/* 타는 애니메이션으로 변경 */
	if (m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Key_Pressing(DIK_LEFT))
		m_pModelCom->Set_Animation(ANI_M_Grind_Slow_MH_Left);
	else if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pModelCom->Set_Animation(ANI_M_Grind_Slow_MH_Right);
	else
		m_pModelCom->Set_NextAnimIndex(ANI_M_Grind_Slow_MH);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_bOnRail = m_pTargetRail->Take_Path(CSpaceRail::SUBJ_MAY, dTimeDelta, WorldMatrix);
	if (m_bOnRail)
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	else
	{
		m_pGameInstance->Stop_Sound(CHANNEL_MAY_RAIL);
		m_pGameInstance->Set_SoundVolume(CHANNEL_MAY_RAIL, m_fRailSoundVolume);
		m_pGameInstance->Play_Sound(TEXT("Rail_End.wav"), CHANNEL_MAY_RAIL, m_fRailSoundVolume);

		m_pTargetRail = nullptr;
		m_pTargetRailNode = nullptr;
		m_pSearchTargetRailNode = nullptr;
		m_pModelCom->Set_Animation(ANI_M_Jump_Falling);
		m_bOnRailEnd = true;
	}
}

void CMay::TakeRailEnd(_double dTimeDelta)
{
	if (m_bOnRailEnd)
	{
		_double dRailEndForceTime = 0.2;
		if (m_dRailEnd_ForceDeltaT >= dRailEndForceTime)
		{
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			Loop_RadiarBlur(false);

			m_dRailEnd_ForceDeltaT = 0.0;
			m_bOnRailEnd = false;
		}
		else
		{
			m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_UP), dTimeDelta);
			m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK), (dRailEndForceTime - m_dRailEnd_ForceDeltaT) * 2.5f);
			m_dRailEnd_ForceDeltaT += dTimeDelta;
		}
	}
}

void CMay::ShowRailTargetTriggerUI()
{
	// Show UI
	if (m_pSearchTargetRailNode && nullptr == m_pTargetRailNode && false == m_bOnRail)
	{
		m_pGauge_Circle->Set_Active(true);
		m_pGauge_Circle->Set_TargetPos(m_pSearchTargetRailNode->Get_Position());
	}
	else {
		m_pGauge_Circle->Set_Active(false);
		UI_Delete(May, InputButton_InterActive_Rail);
		m_pGauge_Circle->Set_DefaultSetting();
	}
}
#pragma endregion

HRESULT CMay::Ready_Layer_Gauge_Circle(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Gauge_Circle"), &iOption, &pGameObject), E_FAIL);
	m_pGauge_Circle = static_cast<CGauge_Circle*>(pGameObject);
	m_pGauge_Circle->Set_SwingPointPlayerID(Player::May);
	// 범위 설정
	m_pGauge_Circle->Set_Range(20.f);

	return S_OK;
}

#pragma region RadiarBlur
void CMay::Start_RadiarBlur(_double dBlurTime)
{
	//if (m_bRadiarBlur) return;

	m_bRadiarBlur_Trigger = true;
	m_dRadiarBlurTime = dBlurTime;
	m_dRadiarBlurDeltaT = 0.0;

	Set_RadiarBlur(true);
}

void CMay::Loop_RadiarBlur(_bool bLoop)
{
	m_bRadiarBlur_Loop = bLoop;

	if (m_bRadiarBlur_Loop)
		Set_RadiarBlur(true);
	else
		Set_RadiarBlur(false);
}

void CMay::Trigger_RadiarBlur(_double dTimeDelta)
{
	if (m_bRadiarBlur_Loop)
	{
		Set_RadiarBlur(true);
	}
	else if (m_bRadiarBlur_Trigger)
	{
		if (m_dRadiarBlurDeltaT >= m_dRadiarBlurTime)
		{
			Set_RadiarBlur(false);
			m_dRadiarBlurDeltaT = 0.0;
			m_bRadiarBlur_Trigger = false;
		}
		else
		{
			m_dRadiarBlurDeltaT += dTimeDelta;
			Set_RadiarBlur(true);
		}
	}
}

void CMay::Set_RadiarBlur(_bool bActive)
{
	_matrix CombineViewMatrix, CombineProjMatrix;

	CombineViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBVIEW);
	CombineProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBPROJ);

	_matrix matCombineMatrix = CombineViewMatrix * CombineProjMatrix;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVector3TransformCoord(vPos, matCombineMatrix);

	_float3 vConvertPos;
	XMStoreFloat3(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.y += 1.f;

	if (1.f <= vConvertPos.z)
	{
		vConvertPos.x *= -1.f;
		vConvertPos.y *= -1.f;
	}

	D3D11_VIEWPORT Viewport = m_pGameInstance->Get_ViewportInfo(2);
	vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f);
	vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f);

	_float2 vFocusPos = { vConvertPos.x / g_iWinCX + 0.5f, vConvertPos.y / g_iWinCY };
	vFocusPos.y -= 0.08f; // Offset 0.04f
	m_pGameInstance->Set_RadiarBlur_Sub(bActive, vFocusPos);
}
#pragma endregion

#pragma region Script
void CMay::Script_Trigger(_double dTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (vPosition.m128_f32[1] > 125.f && m_bSecondFloor_FirstIn == false)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_VOICE_MAY_2, m_fSecondFloor_Voice_Volume);
		m_pGameInstance->Play_Sound(TEXT("03.wav"), CHANNEL_VOICE_MAY_2, m_fSecondFloor_Voice_Volume);
		m_bSecondFloor_FirstIn = true;
		m_IsSecondFloor_Voice_Playing = true;
	}

	if (m_IsSecondFloor_Voice_Playing == true)
	{
		m_fSecondFloor_Script_DelayTime += (_float)dTimeDelta;

		// 02 우주개코원숭이 - "감옥에서 탈출했군. 이 배신자들!"
		if (m_fSecondFloor_Script_DelayTime > 0.f && m_fSecondFloor_Script_Once[0] == false)
		{
			SCRIPT->Render_Script(2, CScript::HALF, 2.f, false);
			m_fSecondFloor_Script_Once[0] = true;
		}
		// 03 우주개코원숭이 - "하지만 기억해라."
		else if (m_fSecondFloor_Script_DelayTime > 2.1f && m_fSecondFloor_Script_Once[1] == false)
		{
			SCRIPT->Render_Script(3, CScript::HALF, 1.4f, false);
			m_fSecondFloor_Script_Once[1] = true;
		}
		// 04 우주개코원숭이 - "절대 살아서 나가진 못한다!"
		else if (m_fSecondFloor_Script_DelayTime > 3.6f && m_fSecondFloor_Script_Once[2] == false)
		{
			SCRIPT->Render_Script(4, CScript::HALF, 1.7f, false);
			m_fSecondFloor_Script_Once[2] = true;
		}
		// 05 코디 	  - "저 친구는 좀 진정해야겠는데."
		else if (m_fSecondFloor_Script_DelayTime > 5.4f && m_fSecondFloor_Script_Once[3] == false)
		{
			SCRIPT->Render_Script(5, CScript::HALF, 1.5f, false);
			m_fSecondFloor_Script_Once[3] = true;
		}
		// 06 메이 	  - "녀석은 무시해! 포털이 있어!"
		else if (m_fSecondFloor_Script_DelayTime > 7.f && m_fSecondFloor_Script_Once[4] == false)
		{
			SCRIPT->Render_Script(6, CScript::HALF, 2.f, false);
			m_fSecondFloor_Script_Once[4] = true;
		}
		// 07 메이 	  - "저걸 이용할 수 있을 거야."
		else if (m_fSecondFloor_Script_DelayTime > 9.1f && m_fSecondFloor_Script_Once[5] == false)
		{
			SCRIPT->Render_Script(7, CScript::HALF, 2.f, false);
			m_fSecondFloor_Script_Once[5] = true;
		}
	}
}
#pragma region