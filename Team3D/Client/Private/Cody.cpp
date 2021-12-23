#include "stdafx.h"
#include "..\Public\Cody.h"
#include "MainCamera.h"
#include "UI_Generator.h"
#include "UIObject.h"
#include "May.h"
#include "DataStorage.h"
#include "PlayerActor.h"
#include "SpaceRail.h"
#include "SpaceRail_Node.h"
#include "UmbrellaBeam_Joystick.h"
#include "Effect_Generator.h"
#include "Effect_Cody_Size.h"
#include "ControlRoom_Battery.h"
#include "HookUFO.h"
#include "Gauge_Circle.h"
#include "CutScenePlayer.h"

/* For. PinBall */
#include "PinBall.h"
#include "PinBall_Door.h"
#include "HangingPlanet.h"
/* For.Tube*/
#include "HookahTube.h"
/* For.SpaceShip */
#include "ElectricBox.h"
#include "ElectricWall.h"
/* For.LaserTennis */
#include "LaserTennis_Manager.h"
/*For.WarpGate*/
#include "WarpGate.h"

/* For.BossUFO */
#include "UFO.h"

#include "Script.h"

/* For. UFORadarSet */
#include "UFORadarSet.h"
#include "UFORadarLever.h"
#include "EndingCredit_Manager.h"

/* For. UI */
#include "HpBar.h"
#include "MinigameHpBar.h"
#include "PinBall_Handle.h"

#pragma region Ready
CCody::CCody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCharacter(pDevice, pDeviceContext)
{
}

CCody::CCody(const CCody& rhs)
	: CCharacter(rhs)
{
}

HRESULT CCody::NativeConstruct_Prototype()
{
	CCharacter::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCody::NativeConstruct(void* pArg)
{
	CCharacter::NativeConstruct(pArg);
	Ready_Component();

	//m_pModelCom->Set_Animation(ANI_C_MH);
	m_pModelCom->Set_Animation(0);
	CDataStorage::GetInstance()->Set_CodyPtr(this);
	Add_LerpInfo_To_Model();

	FAILED_CHECK_RETURN(Ready_UI(), E_FAIL);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
	m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
	m_pGameInstance->Play_Sound(TEXT("CodyB_Walk.wav"), CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);
	m_pGameInstance->Play_Sound(TEXT("CodyM_Run.wav"), CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fCody_Rope_UFO_Throw_Volume);
	m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Catch.wav"), CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fCody_Rope_UFO_Throw_Volume);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
	m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);

	m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
	m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
	m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
	m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_ROPE_UFO_CATCH);
	m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);

	return S_OK;
}

HRESULT CCody::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	CTransform::TRANSFORM_DESC PlayerTransformDesc;
	PlayerTransformDesc.dRotationPerSec = XMConvertToRadians(720.f);
	PlayerTransformDesc.dSpeedPerSec = 2.f;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &PlayerTransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.7f, 30.f, 1.f));// 적당한 시작지점
	CPlayerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eCODY, this);
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
	m_pActorCom->Set_PlayerType(CPlayerActor::PLAYER_CODY);


	//Effect 
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_Cody_Size"), nullptr, (CGameObject**)&m_pEffect_Size), E_FAIL);
	m_pEffect_Size->Set_Model(m_pModelCom);

	FAILED_CHECK_RETURN(Ready_Layer_Gauge_Circle(TEXT("Layer_CodyCircle_Gauge")), E_FAIL);


	return S_OK;
}

HRESULT CCody::Ready_UI()
{
	UI_Create(Cody, PC_Mouse_Reduction);
	UI_Create(Cody, PC_Mouse_Enlargement);
	UI_Create(Cody, PlayerMarker);

	CGameObject* pGameObject = nullptr;
	_uint iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("CodyHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pHpBar = static_cast<CHpBar*>(pGameObject);
	m_pHpBar->Set_PlayerID(Player::Cody);
	m_pHpBar->Set_ShaderOption(0);

	iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("CodySubHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pSubHpBar = static_cast<CHpBar*>(pGameObject);
	m_pSubHpBar->Set_PlayerID(Player::Cody);
	m_pSubHpBar->Set_ShaderOption(0);

	iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameCodyHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pMinigameHpBar = static_cast<CMinigameHpBar*>(pGameObject);
	m_pMinigameHpBar->Set_PlayerID(Player::Cody);
	m_pMinigameHpBar->Set_ShaderOption(0);

	iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameCodySubHpBar"), &iOption, &pGameObject), E_FAIL);
	m_pMinigameSubHpBar = static_cast<CMinigameHpBar*>(pGameObject);
	m_pMinigameSubHpBar->Set_PlayerID(Player::Cody);
	m_pMinigameSubHpBar->Set_ShaderOption(0);

	return S_OK;
}

void CCody::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Jump_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Bhv_Valve_Rotate_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_Valve_Rotate_MH, ANI_C_Bhv_Valve_Rotate_R, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_Valve_Rotate_MH, ANI_C_Bhv_Valve_Rotate_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_Valve_Rotate_R, ANI_C_Bhv_Valve_Rotate_MH, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Start, ANI_C_Bhv_GroundPound_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Start, ANI_C_Bhv_GroundPound_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land, ANI_C_Bhv_GroundPound_Land_Exit, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land, ANI_C_ChangeSize_Walk_Large_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Falling, ANI_C_Bhv_GroundPound_Land_Exit, true, 5.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_MH, true, 5.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_Jog_Start_Fwd, true, 2.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_Jog, true, 2.f);
	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_Bhv_ChangeSize_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Start, ANI_C_Bhv_ChangeSize_GroundPound_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Falling, ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit, false);
	

	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_Sprint, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit, ANI_C_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Walk_Large_Fwd, ANI_C_ChangeSize_Walk_Large_Fwd, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_Stop, true, 60.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Stop, ANI_C_Roll_To_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_To_Jog, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_DoubleJump, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_AirDash_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_DoubleJump, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_AirDash_Start, false);

	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_DoubleJump, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Bhv_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Bhv_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_SprintTurnAround, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_SprintTurnAround, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd_Exhausted, ANI_C_SprintTurnAround, true, 20.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_ChangeSize_Jump_Start, false);

	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Large_Land, ANI_C_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_ChangeSize_Jump_Large_Land_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_ChangeSize_Jump_Large_Land, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_180L, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_180R, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_Falling, true, 10.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Start_Fwd, ANI_C_Jump_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_Jump_Falling, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Jog, ANI_C_Jump_Falling, false);

	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jump_Land_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog_Start_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog_Stop_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_Jump_Land_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jump_Land_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog_Start_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog_Stop_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Falling, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land_Jog, false);

	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_MH, ANI_C_WallSlide_Jump, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_Jump, ANI_C_WallSlide_Enter, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_Enter, ANI_C_WallSlide_MH, true, 20.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_Enter, ANI_C_Grind_Grapple_ToGrind, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_ToGrind, ANI_C_Grind_Slow_MH, false);

	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Holding_Low_UFO, true, 300.f);
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Holding_Enter_UFO, true, 1000.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Holding_Enter_UFO, ANI_C_Holding_Low_UFO, true);
	m_pModelCom->Add_LerpInfo(ANI_C_Holding_Low_UFO, ANI_C_Holding_UFO, true);

	m_pModelCom->Add_LerpInfo(ANI_C_Holding_UFO, ANI_C_CutScene_BossFight_LaserRippedOff, true);

	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_Rocket_MH, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Rocket_Enter, ANI_C_Rocket_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Rocket_MH, ANI_C_Rocket_MH, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Rocket_Exit, ANI_C_Jump_Land_High, false);

	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_CodyCutSceneIntro, true, 5000.f);
	m_pModelCom->Add_LerpInfo(ANI_C_ActionMH, ANI_C_CodyCutSceneIntro, true, 5000.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_MH_Gesture_Small_Drumming, ANI_C_CodyCutSceneIntro, true, 5000.f);

	return;
}

#pragma endregion

#pragma region Overrided Function
_int CCody::Tick(_double dTimeDelta)
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
	UI_Generator->Set_TargetPos(Player::May, UI::PlayerMarker, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pCamera = (CMainCamera*)CDataStorage::GetInstance()->Get_MainCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	/* Script */
	PinBall_Script(dTimeDelta);

	//tEST
	_vector vTestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//TEST

#pragma region BasicActions
	/////////////////////////////////////////////
	KeyInput_Rail(dTimeDelta);

	if (false == m_bMoveToRail && false == m_bOnRail && false == m_bEndingCredit)
	{
		LaserTennis(dTimeDelta);
		ElectricWallJump(dTimeDelta);
		Pipe_WallJump(dTimeDelta);
		Wall_Jump(dTimeDelta);
		if (Trigger_Check(dTimeDelta))
		{
			TakeRailEnd(dTimeDelta);
			Hit_StarBuddy(dTimeDelta);
			Hit_Rocket(dTimeDelta);
			Activate_RobotLever(dTimeDelta);
			Push_Battery(dTimeDelta);
			Rotate_Valve(dTimeDelta);
			In_GravityPipe(dTimeDelta);
			Hit_Planet(dTimeDelta);
			Hook_UFO(dTimeDelta);
			Warp_Wormhole(dTimeDelta);
			Touch_FireDoor(dTimeDelta);
			Falling_Dead(dTimeDelta); 
			WallLaserTrap(dTimeDelta);
			PinBall(dTimeDelta);
			SpaceShip_Respawn(dTimeDelta);
			Holding_BossUFO(dTimeDelta);
			In_JoyStick(dTimeDelta);
			BossMissile_Control(dTimeDelta);
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
				if(m_IsSizeChanging == false)
				Move(dTimeDelta);
				if(m_eCurPlayerSize != SIZE_LARGE)
				Roll(dTimeDelta);
				Jump(dTimeDelta);
				Change_Size(dTimeDelta);
			}
			Ground_Pound(dTimeDelta);
		}
	}
	/////////////////////////////////////////////

#pragma endregion

	/* 레일 타겟을 향해 날라가기 */
	// Forward 조정
	MoveToTargetRail(dTimeDelta);

	/* 레일타기 : 타겟을 찾지 못하면 타지않음. */
	TakeRail(dTimeDelta);

	if (true == m_bOnRail || true == m_bMoveToRail)
	{
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pActorCom->Set_Position(vPlayerPos);
	}
	else
		m_pActorCom->Update(dTimeDelta); // Set Position하면 이거 할필요없다.

	m_pModelCom->Update_Animation(dTimeDelta);
	m_pEffect_Size->Update_Matrix(m_pTransformCom->Get_WorldMatrix());

	// Control RadiarBlur - 제일 마지막에 호출
	Trigger_RadiarBlur(dTimeDelta);

	/* 히트타임 */
	if (true == m_bHit)
	{
		m_dHitTime += dTimeDelta;

		if (0.5f <= m_dHitTime)
		{
			m_bHit = false;
			m_dHitTime = 0.0;
		}
	}

	return NO_EVENT;
}

_int CCody::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	Ride_Ending_Rocket(dTimeDelta);

	if (CCutScenePlayer::GetInstance()->Get_IsPlayCutScene())
	{
		if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1000.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		return NO_EVENT;
	}

	/* LateTick : 레일의 타겟 찾기*/
	Find_TargetSpaceRail();
	ShowRailTargetTriggerUI();
	Clear_TagerRailNodes();

	if (true == m_IsTouchFireDoor || true == m_IsWallLaserTrap_Touch || true == m_IsDeadLine)
		return NO_EVENT;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	{
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CCody::Render(RENDER_GROUP::Enum eGroup)
{
	if (true == m_IsDeadLine || m_IsPinBall || m_bRespawn) 
		return S_OK;

	CCharacter::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	if (true == m_bHit)
	{
		m_pModelCom->Set_DefaultVariables_Shadow();
		m_pModelCom->Render_Model(35);
	}
	else
	{
		if (eGroup == RENDER_GROUP::RENDER_NONALPHA)
		{
			m_pModelCom->Set_DefaultVariables_Shadow();
			m_pModelCom->Render_Model(0);
		}
		else if (eGroup == RENDER_GROUP::RENDER_ALPHA && false == m_IsEnding)
		{
			m_pModelCom->Render_Model(30);
			m_pModelCom->Render_Model(32);
		}
	}

	return S_OK;
}

#pragma endregion

#pragma region Rarely_Fix
CCody* CCody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCody* pInstance = new CCody(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CCody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCody::Clone_GameObject(void* pArg)
{
	CCody* pInstance = new CCody(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CCody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCody::Free()
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

	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffect_Size);

	if (nullptr != m_pTargetPtr)
		Safe_Release(m_pTargetPtr);

	CCharacter::Free();
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCody::KeyInput(_double dTimeDelta)
{
#pragma region Local variable
	_vector vCameraLook = XMVectorSetW(XMVector3Normalize(XMVector3Cross(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT), m_pTransformCom->Get_State(CTransform::STATE_UP))), 0.f);
	_vector vCameraRight = XMVectorSetW(XMVector3Normalize(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT)), 0.f);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;
#pragma endregion

#pragma region Teleport
	if (m_pGameInstance->Key_Down(DIK_C)) /* 파이프 베터리 */
		m_pActorCom->Set_Position(XMVectorSet(44.8652f, 220.9396f, 223.94134f, 1.f));

	if (m_pGameInstance->Key_Down(DIK_1)) /* 스타트 지점 */
	{
		m_pActorCom->Set_Position(XMVectorSet(60.f, 0.f, 15.f, 1.f));
		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
		m_pActorCom->Set_IsPlayerInUFO(false); // 실제로 보스방에서 이동하게 되면 On/Off 해줘야함!! 테스트용으로 임시로 만든거.
	}
	if (m_pGameInstance->Key_Down(DIK_2)) /* 2층 */
	{
		m_pActorCom->Set_Position(XMVectorSet(60.f, 127.f, 170.f, 1.f));
		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_3)) /* 2스테이지 입구 */
	{
		m_pActorCom->Set_Position(XMVectorSet(620.f, 760.f, 195.f, 1.f));
		DATABASE->Set_May_Stage(ST_RAIL);
		DATABASE->Set_Cody_Stage(ST_RAIL);
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_4)) /* 2스테이지 */
	{
		m_pActorCom->Set_Position(XMVectorSet(960.f, 720.f, 193.f, 1.f));
		DATABASE->Set_May_Stage(ST_RAIL);
		DATABASE->Set_Cody_Stage(ST_RAIL);
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_5))/* 3스테이지 */
	{
		m_pActorCom->Set_Position(XMVectorSet(-650.f, 760.f, 195.f, 1.f));
		DATABASE->Set_May_Stage(ST_PINBALL);
		DATABASE->Set_Cody_Stage(ST_PINBALL);
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_6))/* 3층 */
	{
		m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_7))/* Boss */
	{
		m_pActorCom->Set_Position(XMVectorSet(62.f, 250.f, 187.f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_8))/* Moon */
	{
		m_pActorCom->Set_Position(XMVectorSet(60.f, 760.f, 194.f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(false);
	}
	if (m_pGameInstance->Key_Down(DIK_9))/* 우주선 내부 */
	{
		m_pActorCom->Set_Position(XMVectorSet(67.9958f, 599.431f, 1002.82f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(true);
	}
	//if (m_pGameInstance->Key_Down(DIK_0))/* 우산 */
	//{
	//	m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));
	//	m_pActorCom->Set_IsPlayerInUFO(false);
	//}
	if (m_pGameInstance->Key_Down(DIK_0))/* 레이저 테니스 */
	{
		m_pActorCom->Set_Position(XMVectorSet(64.f, 730.f, 1000.f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(false);
	}

	if (m_pGameInstance->Key_Down(DIK_BACKSPACE))/* 우산 */
	{
		DATABASE->Set_May_Stage(ST_PINBALL);
		DATABASE->Set_Cody_Stage(ST_PINBALL);
		//우산앞
		//m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));
		//배터리
		m_pActorCom->Set_Position(XMVectorSet(-814.433655f, 791.810059f, 228.490845f, 1.f));
		//m_pActorCom->Set_Position(XMVectorSet(886.1079f, 728.7372f, 339.7794f, 1.f));
		m_pActorCom->Set_IsPlayerInUFO(false);
	}

	if (m_pGameInstance->Get_CurrentLevelStep() == 2)
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 1.f, 1.f), 3.f);
		//ENDINGCREDIT->Create_Environment();
		m_IsEnding = true;
	}
#pragma endregion

#pragma region 8Way_Move
	
	if (m_IsAirDash == false)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_D))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_A))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook - vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_D))
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_A))
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook - vCameraRight) / 2.f);
		}
		else
		{
			if (m_eCurPlayerSize != SIZE_LARGE)
			{
				if (m_pGameInstance->Key_Pressing(DIK_A) && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
				{
					if ((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) && m_IsTurnAround == false)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
						m_fSprintAcceleration = 15.f;
						bMove[1] = !bMove[1];
						m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
						m_IsTurnAround = true;
						m_iSavedKeyPress = LEFT;
						return;
					}
				}
				if (m_pGameInstance->Key_Pressing(DIK_D) && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
				{
					if ((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) && m_IsTurnAround == false)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
						m_fSprintAcceleration = 15.f;
						bMove[1] = !bMove[1];
						m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
						m_IsTurnAround = true;
						m_iSavedKeyPress = RIGHT;
						return;
					}
				}
				if (m_pGameInstance->Key_Pressing(DIK_W) && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
				{
					if ((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) && m_IsTurnAround == false)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
						m_fSprintAcceleration = 15.f;
						bMove[0] = !bMove[0];
						m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
						m_IsTurnAround = true;
						m_iSavedKeyPress = UP;
						return;
					}
				}
				if (m_pGameInstance->Key_Pressing(DIK_S) && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
				{
					if ((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) && m_IsTurnAround == false)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
						m_fSprintAcceleration = 15.f;
						bMove[0] = !bMove[0];
						m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
						m_IsTurnAround = true;
						m_iSavedKeyPress = DOWN;
						return;
					}
				}
			}

			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, vCameraLook);
				m_iSavedKeyPress = UP;
			}
			if (m_pGameInstance->Key_Pressing(DIK_S))
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
				m_iSavedKeyPress = DOWN;
			}

			if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
				m_iSavedKeyPress = LEFT;
			}
			if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, vCameraRight);
				m_iSavedKeyPress = RIGHT;
			}
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_SprintTurnAround)
		{
			if (m_fSprintAcceleration < 12.f)
				m_fSprintAcceleration += (_float)dTimeDelta * 20.f;
		}
		if (m_pModelCom->Is_AnimFinished(ANI_C_SprintTurnAround))
			m_IsTurnAround = false;

		if (bMove[0] || bMove[1])
			m_bMove = true;
	}

#pragma endregion

#pragma region Keyboard_Shift_Button
	if (m_pGameInstance->Key_Down(DIK_LSHIFT) && m_bRoll == false && m_bCanMove == true && m_eCurPlayerSize != SIZE_LARGE)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			EFFECT->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());
			m_fAcceleration = 5.f;
			m_pModelCom->Set_Animation(ANI_C_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Roll_Stop);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dash.wav"), CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);
			m_bAction = false;
			m_bRoll = true;

			Start_RadiarBlur(0.3f);
		}
		else
		{
			if (m_pModelCom->Get_CurAnimIndex() != ANI_C_AirDash_Start && m_iAirDashCount == 0)
			{
				CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());

				m_iAirDashCount += 1;
				m_fAcceleration = 5.f;

				if (m_eCurPlayerSize != SIZE_SMALL)
				{
					m_pActorCom->Jump_Start(1.2f);
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyM_Dash.wav"), CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);
				}
				else
				{
					m_pActorCom->Jump_Start(0.6f);
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);

					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyM_Dash.wav"), CHANNEL_CODYM_DASH, m_fCodyMDash_Volume);

					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYS_DASH_VOICE, m_fCodySDash_Voice_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyS_Dash_Voice.wav"), CHANNEL_CODYS_DASH_VOICE, m_fCodySDash_Voice_Volume);
				}
				m_pModelCom->Set_Animation(ANI_C_AirDash_Start);
				m_IsAirDash = true;

				Start_RadiarBlur(0.3f);
			}
		}

	}
#pragma endregion

#pragma region Keyboard_Space_Button
	if (m_eCurPlayerSize != SIZE_LARGE)
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 2 /*&& m_pModelCom->Get_CurAnimIndex() != ANI_C_Jump_Falling*/ && m_bCanMove == true)
		{
			m_bShortJump = true;
			m_iJumpCount += 1;
			m_IsJumping = true;
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
		}
	}
	else
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 1 /*&& m_pModelCom->Get_CurAnimIndex() != ANI_C_Jump_Falling*/ && m_bCanMove == true)
		{
			m_bShortJump = true;
			m_iJumpCount += 1;
			m_IsJumping = true;
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
		}
	}

	
#pragma endregion

#pragma region Mouse_LButton

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_LB) &&
		m_bSprint == false && m_bShortJump == false && m_IsJumping == false && m_IsSizeChanging == false) 
	{
		// 커져라
		switch (m_eCurPlayerSize)
		{
		case Client::CCody::SIZE_SMALL:
			m_eNextPlayerSize = SIZE_MEDIUM;
			m_IsSizeChanging = true;
			m_pActorCom->Set_Gravity(-9.8f);
			m_pActorCom->Set_IsPlayerSizeSmall(false);
			m_pGameInstance->Set_SoundVolume(CHANNEL_SIZE_STOM, m_fSizing_SToM_Volume);
			m_pGameInstance->Play_Sound(TEXT("Sizing_StoM.wav"), CHANNEL_SIZE_STOM, m_fSizing_SToM_Volume);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_eNextPlayerSize = SIZE_LARGE;
			m_IsSizeChanging = true;
			m_pGameInstance->Set_SoundVolume(CHANNEL_SIZE_MTOB, m_fSizing_MToB_Volume);
			m_pGameInstance->Play_Sound(TEXT("Sizing_MtoB.wav"), CHANNEL_SIZE_MTOB, m_fSizing_MToB_Volume);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
			break;
		}
	}
#pragma endregion

#pragma region Mouse_RButton

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_RB) &&
		m_bSprint == false && m_bShortJump == false && m_IsJumping == false && m_IsSizeChanging == false)
	{
		// 작아져라
		switch (m_eCurPlayerSize)
		{
		case Client::CCody::SIZE_LARGE:
			m_eNextPlayerSize = SIZE_MEDIUM;
			m_IsSizeChanging = true;
			m_pActorCom->Set_Gravity(-9.8f);
			m_pGameInstance->Set_SoundVolume(CHANNEL_SIZE_BTOM, m_fSizing_BToM_Volume);
			m_pGameInstance->Play_Sound(TEXT("Sizing_BtoM.wav"), CHANNEL_SIZE_BTOM, m_fSizing_BToM_Volume);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_eNextPlayerSize = SIZE_SMALL;
			m_IsSizeChanging = true;
			m_pActorCom->Set_IsPlayerSizeSmall(true);
			m_pGameInstance->Set_SoundVolume(CHANNEL_SIZE_MTOS, m_fSizing_MToS_Volume);
			m_pGameInstance->Play_Sound(TEXT("Sizing_MtoS.wav"), CHANNEL_SIZE_MTOS, m_fSizing_MToS_Volume);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
			m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
			break;
		}
	}
#pragma endregion

#pragma region Key_CapsLock
	if (m_pGameInstance->Key_Down(DIK_CAPSLOCK) && m_eCurPlayerSize != SIZE_LARGE && 
		m_pModelCom->Get_CurAnimIndex() != ANI_C_Jog_Exhausted_MH_Start_Fwd && m_pModelCom->Get_CurAnimIndex() != ANI_C_Jog_Stop_Fwd_Exhausted && m_pModelCom->Get_CurAnimIndex() != ANI_C_Sprint_Start_FromDash)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint)
		{
			m_pModelCom->Set_Animation(ANI_C_Jog_Exhausted_MH_Start_Fwd);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
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

#pragma region Key_LCtrl

	if (m_pGameInstance->Key_Down(DIK_LCONTROL) && m_pActorCom->Get_IsJump() == true)
	{
		m_bGroundPound = true;
		m_IsJumping = false;

		if (m_eCurPlayerSize != SIZE_LARGE)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_GROUNDPOUND_ROLL, m_fCodyM_GroundPound_Roll_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_GroundPound_Roll.wav"), CHANNEL_CODYM_GROUNDPOUND_ROLL, m_fCodyM_GroundPound_Roll_Volume);
		}
	}

#pragma endregion

}

_uint CCody::Get_CurState() const
{
	if (nullptr == m_pModelCom) return 0;

	return m_pModelCom->Get_CurAnimIndex();
}

_bool CCody::Get_IsPlayerInUFO()
{
	return m_pActorCom->Get_IsPlayerInUFO();
}

void CCody::Move(const _double dTimeDelta)
{
#pragma region Medium_Size

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);


	if (m_eCurPlayerSize == SIZE_MEDIUM)
	{
		if (m_bSprint == false && m_bMove && m_pTransformCom)
		{
			m_bAction = false;

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

				else if((fAbsY > fAbsX) && (fAbsY > fAbsZ))
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
			
			if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false)
			{
				// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
				if (m_pModelCom->Is_AnimFinished(ANI_C_Jog_Start_Fwd) == true) // JogStart -> Jog
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Jog) == true) // Jog -> Jog // 보간속도 Up
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd || m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Drumming || m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH)	// Idle To Jog Start. -> Jog 예약
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH_To_Idle || m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog_Exhausted_MH_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_bRoll = false;
					m_pModelCom->Set_Animation(ANI_C_Roll_To_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
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
				if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog) // jog 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Start_Fwd) // JogStart 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH) // IDLE 상태라면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_fIdleTime += (_float)dTimeDelta;
					if (m_bAction == false)
					{
						if (m_fIdleTime > 5.f && m_pModelCom->Is_AnimFinished(ANI_C_MH)) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
						{
							m_pModelCom->Set_Animation(ANI_C_Bhv_MH_Gesture_Small_Drumming); // 배 두들기는 애니메이션 재생
							m_fIdleTime = 0.f;
						}
					}
					else if (m_bAction == true)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
						m_pModelCom->Set_Animation(ANI_C_Idle_To_Action);
						m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH);
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Idle_To_Action) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH_To_Idle);
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
					m_bAction = false;
				}
			}
		}
	}
#pragma endregion

#pragma region Large_Size
	else if (m_eCurPlayerSize == SIZE_LARGE)
	{
		if (m_bMove && m_pTransformCom)
		{

			_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
			vDirection = XMVectorSetY(vDirection, 0.f);
			vDirection = XMVector3Normalize(vDirection);

			m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

			if (m_fJogAcceleration > 10.f)
				m_fJogAcceleration -= (_float)dTimeDelta * 50.f;
			else
				m_fJogAcceleration = 10.f;

			m_pActorCom->Move(vDirection / m_fJogAcceleration, dTimeDelta);

			if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false)
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYB_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyB_Walk.wav"), CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
				}
				// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
				if (m_pModelCom->Is_AnimFinished(ANI_C_ChangeSize_Walk_Large_Start) == true) // JogStart -> Jog
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYB_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyB_Walk.wav"), CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
					}

					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ChangeSize_Walk_Large_Fwd) == true) // Jog -> Jog // 보간속도 Up
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYB_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyB_Walk.wav"), CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
					}
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Scratch
					|| m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Fwd
					|| m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Stop)	// Idle To Jog Start. -> Jog 예약

				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYB_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyB_Walk.wav"), CHANNEL_CODYB_WALK, m_fCodyB_Walk_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Start);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
				}
			}
			m_bMove = false;
		}
		else
		{
			if (m_bShortJump == false)
			{
				m_fJogAcceleration = 25.f;
				if (m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Fwd) // jog 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Stop); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Start) // JogStart 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Stop); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH) // IDLE 상태라면
				{

					m_fIdleTime += (_float)dTimeDelta;
					if (m_bAction == false)
					{
						if (m_fIdleTime > 5.f && m_pModelCom->Is_AnimFinished(ANI_C_MH)) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
						{
							m_pModelCom->Set_Animation(ANI_C_Bhv_MH_Gesture_Small_Scratch); // 배 두들기는 애니메이션 재생
							m_fIdleTime = 0.f;
						}
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Idle_To_Action) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYB_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH_To_Idle);
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
					m_bAction = false;
				}
			}
		}
	}
#pragma endregion

#pragma region Small_Size
	else if (m_eCurPlayerSize == SIZE_SMALL)
	{
		if (m_bSprint == false && m_bMove && m_pTransformCom)
		{
			m_bAction = false;

			_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
			vDirection = XMVectorSetY(vDirection, 0.f);
			vDirection = XMVector3Normalize(vDirection);

			m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

			if (m_fJogAcceleration > 10.f)
				m_fJogAcceleration -= (_float)dTimeDelta * 50.f;
			else
				m_fJogAcceleration = 10.f;

			m_pActorCom->Move(vDirection / m_fJogAcceleration / 8.f, dTimeDelta);

			if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false)
			{
				// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
				if (m_pModelCom->Is_AnimFinished(ANI_C_Jog_Start_Fwd) == true) // JogStart -> Jog
				{
					m_pModelCom->Set_Animation(ANI_C_Jog);
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Jog) == true) // Jog -> Jog // 보간속도 Up
				{
					m_pModelCom->Set_Animation(ANI_C_Jog);
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Drumming || m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH)	// Idle To Jog Start. -> Jog 예약
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH_To_Idle)
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_pModelCom->Set_Animation(ANI_C_Jog_Exhausted_MH_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
				{
					if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
					{
						m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
						m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					}
					m_bRoll = false;
					m_pModelCom->Set_Animation(ANI_C_Roll_To_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
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
				if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog) // jog 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Start_Fwd) // JogStart 였다면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH) // IDLE 상태라면
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_fIdleTime += (_float)dTimeDelta;
					if (m_bAction == false)
					{
						if (m_fIdleTime > 5.f && m_pModelCom->Is_AnimFinished(ANI_C_MH)) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
						{
							m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
							m_pModelCom->Set_Animation(ANI_C_Bhv_MH_Gesture_Small_Drumming); // 배 두들기는 애니메이션 재생
							m_fIdleTime = 0.f;
						}
					}
					else if (m_bAction == true)
					{
						m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
						m_pModelCom->Set_Animation(ANI_C_Idle_To_Action);
						m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH);
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Idle_To_Action) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
					m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
					m_pModelCom->Set_Animation(ANI_C_ActionMH_To_Idle);
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
					m_bAction = false;
				}
			}
		}
	}
#pragma endregion
}
void CCody::Roll(const _double dTimeDelta)
{
	if ((m_bRoll && m_pTransformCom))
	{
		if (m_fAcceleration <= 0.f)
		{
			m_fAcceleration = 5.0;
			m_pModelCom->Set_Animation(ANI_C_Roll_Stop);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH_ROLL, m_fCodyMDash_Roll_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dash_Roll.wav"), CHANNEL_CODYM_DASH_ROLL, m_fCodyMDash_Roll_Volume);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH_LANDING, m_fCodyMDash_Landing_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dash_Landing.wav"), CHANNEL_CODYM_DASH_LANDING, m_fCodyMDash_Landing_Volume);

			if (m_bMove == false)
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		
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
		if (m_eCurPlayerSize == SIZE_MEDIUM)
			m_pActorCom->Move(vDirection * (m_fAcceleration / 15.f), dTimeDelta);
		else if (m_eCurPlayerSize == SIZE_SMALL)
			m_pActorCom->Move(vDirection * (m_fAcceleration / 120.f), dTimeDelta);
	}

	if (m_IsAirDash && m_bCanMove == true && m_pTransformCom)
	{
		if (m_fAcceleration <= 0.f)
		{
			//m_fAcceleration = 5.0;
			m_IsAirDash = false;
		}

		if(m_fAcceleration > 0.f)
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

		if(m_eCurPlayerSize == SIZE_MEDIUM)
			m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), dTimeDelta);
		else if (m_eCurPlayerSize == SIZE_SMALL)
			m_pActorCom->Move(vDirection * (m_fAcceleration / 120.f), dTimeDelta);
	}
}
void CCody::Sprint(const _double dTimeDelta)
{
	if (m_bSprint == true && m_bMove == true)
	{
		m_bAction = false;

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

		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

		if (m_fSprintAcceleration > 5.f)
			m_fSprintAcceleration -= (_float)dTimeDelta * 50.f;
		else
			m_fSprintAcceleration = 5.f;

		if(m_eCurPlayerSize == SIZE_MEDIUM)
			m_pActorCom->Move(vDirection / m_fSprintAcceleration, dTimeDelta);
		else if (m_eCurPlayerSize == SIZE_SMALL)
			m_pActorCom->Move(vDirection / m_fSprintAcceleration / 8.f, dTimeDelta);


		if (m_bRoll == false && m_IsJumping == false && m_IsTurnAround == false)
		{
			if (m_pModelCom->Is_AnimFinished(ANI_C_Sprint_Start_FromDash) == true) // JogStart -> Jog
			{
				m_pModelCom->Set_Animation(ANI_C_Sprint);
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_RUN) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyM_Run.wav"), CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);
				}
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_C_Sprint) == true) // Jog -> Jog // 보간속도 Up
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_RUN) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyM_Run.wav"), CHANNEL_CODYM_RUN, m_fCodyM_Sprint_Volume);
				}
				m_pModelCom->Set_Animation(ANI_C_Sprint);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || 
				m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Drumming ||
				m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd)	// Idle To Jog Start. -> Jog 예약
			{
				m_pModelCom->Set_Animation(ANI_C_Sprint_Start_FromDash);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Sprint);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_C_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
			{
				m_bRoll = false;
				if (m_bSprint == false)
				{
					m_pModelCom->Set_Animation(ANI_C_Roll_To_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else
				{
					m_pModelCom->Set_Animation(ANI_C_Sprint_Start_FromDash);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Sprint);
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
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint || m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash) // jog 였다면
			{
				m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
				m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
				m_bSprint = false;
				m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd_Exhausted); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_C_Exhausted_MH); // jog to stop 끝나면 idle 예약.
			}
		}
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash) // JogStart 였다면
	{
		m_pGameInstance->Stop_Sound(CHANNEL_CODYM_WALK);
		m_pGameInstance->Stop_Sound(CHANNEL_CODYM_RUN);
		m_bSprint = false;
		m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd_Exhausted); // jog to stop 으로 바꿔
		m_pModelCom->Set_NextAnimIndex(ANI_C_Exhausted_MH);
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH) // IDLE 상태라면
	{
		m_fIdleTime += (_float)dTimeDelta;

		if (m_fIdleTime > 3.f && m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
		{
			m_pModelCom->Set_Animation(ANI_C_Exhausted_MH_To_Idle);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_bSprint = false;
			m_fIdleTime = 0.f;
		}

	}
}
void CCody::Jump(const _double dTimeDelta)
{
	if (m_bShortJump  == true)
	{
		if (m_iJumpCount == 1)
		{
			m_IsJumping = true;
			if (m_eCurPlayerSize == SIZE_MEDIUM)
			{
				m_pActorCom->Jump_Start(2.6f);
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_JUMP, m_fCodyMJumpVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyM_Jump2.wav"), CHANNEL_CODYM_JUMP, m_fCodyMJumpVolume);
			}
			else if (m_eCurPlayerSize == SIZE_LARGE)
			{
				m_pActorCom->Jump_Start(2.8f);
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_JUMP, m_fCodyBJumpVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyB_Jump.wav"), CHANNEL_CODYB_JUMP, m_fCodyBJumpVolume);

				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_JUMP_VOICE, m_fCodyBJumpVoiceVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyB_Jump_Voice.wav"), CHANNEL_CODYB_JUMP_VOICE, m_fCodyBJumpVoiceVolume);
			}
			else if (m_eCurPlayerSize == SIZE_SMALL)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYS_JUMP_VOICE, m_fCodySJumpVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyS_Jump_Voice.wav"), CHANNEL_CODYS_JUMP_VOICE, m_fCodySJumpVolume);
				m_pActorCom->Jump_Start(0.8f);
			}
			if (m_eCurPlayerSize == SIZE_LARGE)
				m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Start); // 사이즈 클때 점프 애니메이션이 다름.
			else
			{
				m_pModelCom->Set_Animation(ANI_C_Jump_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
			}
			m_bShortJump = false;
		}
		if (m_iJumpCount == 2)
		{
			m_IsJumping = true;
			//m_pActorCom->Jump_Higher(1.4f);
			//m_pActorCom->Set_Gravity(0.f);
			if (m_eCurPlayerSize == SIZE_MEDIUM)
			{
				m_pActorCom->Jump_Start(2.6f);
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_JUMP_DOUBLE_VOICE, m_fCodyMJumpDoubleVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyM_Jump_Double_Voice.wav"), CHANNEL_CODYM_JUMP_DOUBLE_VOICE, m_fCodyMJumpDoubleVolume);
			}
			else if (m_eCurPlayerSize == SIZE_SMALL)
			{
				m_pActorCom->Jump_Start(0.8f);
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYS_JUMP_DOUBLE_VOICE, m_fCodySJumpDoubleVolume);
				m_pGameInstance->Play_Sound(TEXT("CodyS_Jump_Double_Voice.wav"), CHANNEL_CODYS_JUMP_DOUBLE_VOICE, m_fCodySJumpDoubleVolume);
			}
			m_pModelCom->Set_Animation(ANI_C_DoubleJump);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
			m_bShortJump = false;
		}
	}
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false && m_bGroundPound == false)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_JUMP_LANDING_VOICE, m_fCodyMJumpLandingVoice_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Jump_Landing_Voice.wav"), CHANNEL_CODYM_JUMP_LANDING_VOICE, m_fCodyMJumpLandingVoice_Volume);
		}
		else if (m_eCurPlayerSize == SIZE_SMALL)
		{
			// 뭔가 이상함
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DASH_LANDING, m_fCodyMDash_Landing_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dash_Landing.wav"), CHANNEL_CODYM_DASH_LANDING, m_fCodyMDash_Landing_Volume);
		}
		else
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_JUMP_LANDING, m_fCodyBJump_Landing_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyB_Jump_Landing.wav"), CHANNEL_CODYB_JUMP_LANDING, m_fCodyBJump_Landing_Volume);
		}
		m_iAirDashCount = 0;
		m_bSprint = false;
		// 착지할때 키를 누르고 있는 상태라면 Jog 로 연결
		if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		{

			if (m_eCurPlayerSize != SIZE_LARGE)
			{
				if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CODYM_WALK) == false)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyM_Walk.wav"), CHANNEL_CODYM_WALK, m_fCodyM_Jog_Volume);
				}

				m_pModelCom->Set_Animation(ANI_C_Jump_Land_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Large_Land_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
			}
		}
		else // 그게 아니라면 그냥 착지.
		{
			if (m_eCurPlayerSize != SIZE_LARGE && m_bAfterGroundPound == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_JUMP_LANDING_VOICE, m_fCodyMJumpLandingVoice_Volume);
				m_pGameInstance->Play_Sound(TEXT("CodyM_Jump_Landing_Voice.wav"), CHANNEL_CODYM_JUMP_LANDING_VOICE, m_fCodyMJumpLandingVoice_Volume);

				m_pModelCom->Set_Animation(ANI_C_Jump_Land);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
			else if (m_eCurPlayerSize == SIZE_LARGE  && m_bPlayGroundPoundOnce == false)
			{
				if (m_pModelCom->Get_CurAnimIndex() != ANI_C_ChangeSize_Jump_Falling)
				{
					m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_JUMP_LANDING, m_fCodyBJump_Landing_Volume);
					m_pGameInstance->Play_Sound(TEXT("CodyB_Jump_Landing.wav"), CHANNEL_CODYB_JUMP_LANDING, m_fCodyBJump_Landing_Volume);

					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Large_Land);
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
			}
		}
		m_IsJumping = false;
		m_iJumpCount = 0;
	}
	else if (m_IsJumping == false && m_IsFalling == true && m_bRoll == false && m_bGroundPound == false)
	{
		m_bSprint = false;

		if (m_bFallAniOnce == false)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Roll_Start || 
				m_pModelCom->Get_CurAnimIndex() == ANI_C_Roll_Stop)
			{
				m_pModelCom->Set_Animation(ANI_C_Jump_180R);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
				m_iJumpCount = 1;
			}
			m_bFallAniOnce = true;
		}
		//m_bSprint = false;
		//m_pModelCom->Set_Animation(ANI_C_Jump_Falling);

		//if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		//{
		//	if (m_eCurPlayerSize != SIZE_LARGE)
		//	{
		//		m_pModelCom->Set_Animation(ANI_C_Jump_Land_Jog);
		//		m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
		//	}
		//	else
		//	{
		//		m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Large_Land_Jog);
		//		m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
		//	}
		//}
		//else
		//{
		//	if (m_eCurPlayerSize != SIZE_LARGE)
		//	{
		//		m_pModelCom->Set_Animation(ANI_C_Jump_Land);
		//		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		//	}
		//	else
		//	{
		//		m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Large_Land);
		//		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		//	}
		//}
	}
	else if (m_IsJumping == false && m_IsFalling == false && m_bFallAniOnce == true && m_bRoll == false && m_bGroundPound == false)
	{
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT) || m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Key_Pressing(DIK_DOWN) || m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			m_pModelCom->Set_Animation(ANI_C_Jump_Land_Jog);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
		}
		else
		{
			m_pModelCom->Set_Animation(ANI_C_Jump_Land);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		}

		m_bFallAniOnce = false;
		m_IsJumping = false;
		m_iJumpCount = 0;
		m_iAirDashCount = 0;
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_IsFalling == true)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM)
		{
			m_pActorCom->Jump_Start(2.6f);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_JUMP_DOUBLE_VOICE, m_fCodyMJumpDoubleVolume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Jump_Double_Voice.wav"), CHANNEL_CODYM_JUMP_DOUBLE_VOICE, m_fCodyMJumpDoubleVolume);
		}
		else if (m_eCurPlayerSize == SIZE_SMALL)
		{
			m_pActorCom->Jump_Start(0.8f);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYS_JUMP_DOUBLE_VOICE, m_fCodySJumpDoubleVolume);
			m_pGameInstance->Play_Sound(TEXT("CodyS_Jump_Double_Voice.wav"), CHANNEL_CODYS_JUMP_DOUBLE_VOICE, m_fCodySJumpDoubleVolume);
		}
		m_bShortJump = true;
		m_IsJumping = true;
		m_iJumpCount = 1;
		return;
	}
}
void CCody::Change_Size(const _double dTimeDelta)
{
	if (m_IsSizeChanging == true)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_LARGE)
		{
			if (m_bChangeSizeEffectOnce == false)
			{
				// Radiar Blur
				Start_RadiarBlur(0.3f);

				m_pActorCom->Set_Scale(2.f, 2.f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_MIDDLE_LARGE);
				m_bChangeSizeEffectOnce = true;
				EFFECT->Add_Effect(Effect_Value::Cody_Size_ShokeWave, m_pTransformCom->Get_WorldMatrix());
			}
			if (m_vScale.x < 5.f)
			{
				m_vScale.x += (_float)dTimeDelta * 20.f;
				m_vScale.y += (_float)dTimeDelta * 20.f;
				m_vScale.z += (_float)dTimeDelta * 20.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_bChangeSizeEffectOnce = false;
				m_vScale = { 5.f, 5.f, 5.f };
				m_IsSizeChanging = false; 
				m_eCurPlayerSize = SIZE_LARGE;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

				//UI
				UI_Generator->UI_RETutorial(Player::Cody, UI::PC_Mouse_Enlargement);
				if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
				{
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
				}
				else
				{
					m_pModelCom->Set_Animation(ANI_C_MH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
			}
		}
		else if (m_eCurPlayerSize == SIZE_LARGE && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			if (m_bChangeSizeEffectOnce == false)
			{
				// Radiar Blur
				Start_RadiarBlur(0.3f);
				
				m_pActorCom->Set_Scale(0.5f, 0.5f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_LARGE_MIDDLE);
				m_bChangeSizeEffectOnce = true;
				EFFECT->Add_Effect(Effect_Value::Cody_Size_ShokeWave, m_pTransformCom->Get_WorldMatrix());
			}
			if (m_vScale.x > 1.f)
			{
				m_vScale.x -= (_float)dTimeDelta * 20.f;
				m_vScale.y -= (_float)dTimeDelta * 20.f;
				m_vScale.z -= (_float)dTimeDelta * 20.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_bChangeSizeEffectOnce = false;
				m_vScale = { 1.f, 1.f, 1.f };
				m_eCurPlayerSize = SIZE_MEDIUM;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
				m_IsSizeChanging = false;
				m_pModelCom->Set_Animation(ANI_C_MH);
			}
		}
		else if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_SMALL)
		{
			m_pActorCom->Get_Controller()->setSlopeLimit(0.02f);
			m_pActorCom->Get_Controller()->setStepOffset(0.02f);

			if (m_bChangeSizeEffectOnce == false)
			{
				// Radiar Blur
				Start_RadiarBlur(0.3f);

				m_pActorCom->Set_Scale(0.025f, 0.025f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_MIDDLE_SMALL);
				m_bChangeSizeEffectOnce = true;
			}
			if (m_vScale.x > 0.5f)
			{
				m_vScale.x -= (_float)dTimeDelta * 10.f;
				m_vScale.y -= (_float)dTimeDelta * 10.f;
				m_vScale.z -= (_float)dTimeDelta * 10.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_bChangeSizeEffectOnce = false;
				m_vScale = { 0.1f, 0.1f, 0.1f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_SMALL;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
				//UI
				UI_Generator->UI_RETutorial(Player::Cody, UI::PC_Mouse_Reduction);
				m_pModelCom->Set_Animation(ANI_C_MH);
			}
		}
		else if (m_eCurPlayerSize == SIZE_SMALL && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			if (m_bChangeSizeEffectOnce == false)
			{
				// Radiar Blur
				Start_RadiarBlur(0.3f);

				m_pActorCom->Set_Scale(0.5f, 0.5f);
				m_pActorCom->Get_Controller()->setStepOffset(0.707f);
				m_pActorCom->Get_Controller()->setSlopeLimit(0.5f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_SMALL_MIDDLE);
				m_bChangeSizeEffectOnce = true;
			}
			if (m_vScale.x < 1.f)
			{
				m_vScale.x += (_float)dTimeDelta * 10.f;
				m_vScale.y += (_float)dTimeDelta * 10.f;
				m_vScale.z += (_float)dTimeDelta * 10.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_bChangeSizeEffectOnce = false;
				m_vScale = { 1.f, 1.f, 1.f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_MEDIUM;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
				m_pModelCom->Set_Animation(ANI_C_MH);
			}
		}
	}
}
void CCody::Ground_Pound(const _double dTimeDelta)
{
	if (m_bGroundPound == true)
	{
		if (m_fGroundPoundAirDelay > 0.4f)
		{
			if (m_eCurPlayerSize != SIZE_LARGE)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Falling);	
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_GroundPound_Falling);
			}
			if (m_eCurPlayerSize == SIZE_MEDIUM || m_eCurPlayerSize == SIZE_LARGE)
				m_pActorCom->Set_Gravity(-9.8f);
			else if (m_eCurPlayerSize == SIZE_SMALL)
				m_pActorCom->Set_Gravity(-7.5f);
			m_pActorCom->Jump_Start(-20.f);
			m_fGroundPoundAirDelay = 0.f;
			m_bGroundPound = false;
		}
		else
		{
			m_IsJumping = false;
			m_bCanMove = false;
			if (m_eCurPlayerSize == SIZE_LARGE)
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_GroundPound_Start);
			else
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Start);
			}
			m_pActorCom->Set_Jump(false);
			m_pActorCom->Set_Gravity(0.f);
			m_fGroundPoundAirDelay += (_float)dTimeDelta;
		}
	}

	if (m_eCurPlayerSize != SIZE_LARGE)
	{
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_GroundPound_Falling) && m_bPlayGroundPoundOnce == false)
		{
			if (m_eCurPlayerSize == SIZE_MEDIUM)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_GROUNDPOUND, m_fCodyM_GroundPound_Volume);
				m_pGameInstance->Play_Sound(TEXT("CodyM_GroundPound.wav"), CHANNEL_CODYM_GROUNDPOUND, m_fCodyM_GroundPound_Volume);

				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_GROUNDPOUND_LANDING_VOICE, m_fCodyM_GroundPound_Landing_Voice_Volume);
				m_pGameInstance->Play_Sound(TEXT("CodyM_GroundPound_Landing_Voice.wav"), CHANNEL_CODYM_GROUNDPOUND_LANDING_VOICE, m_fCodyM_GroundPound_Landing_Voice_Volume);

			}
			else if (m_eCurPlayerSize == SIZE_SMALL)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CODYS_GROUNDPOUND_LANDING_VOICE, m_fCodyS_GroundPound_Landing_Voice_Volume);
				m_pGameInstance->Play_Sound(TEXT("CodyS_GroundPound_Landing_Voice.wav"), CHANNEL_CODYS_GROUNDPOUND_LANDING_VOICE, m_fCodyS_GroundPound_Landing_Voice_Volume);

			}
			m_bPlayGroundPoundOnce = true;
			m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Land_Exit);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		}
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_GroundPound_Land_Exit))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_bPlayGroundPoundOnce = false;
			m_IsAirDash = false;
			//m_bCanMove = true;
			m_bAfterGroundPound = true;
		}
	}
	else
	{
		if (m_pActorCom->Get_IsJump() == true && (m_pModelCom->Get_CurAnimIndex() == (ANI_C_Bhv_ChangeSize_GroundPound_Falling) && m_bPlayGroundPoundOnce == false))
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYB_GROUNDPOUND_LANDING_VOICE, m_fCodyB_GroundPound_Landing_Voice_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyB_GroundPound_Landing_Voice.wav"), CHANNEL_CODYB_GROUNDPOUND_LANDING_VOICE, m_fCodyB_GroundPound_Landing_Voice_Volume);

			m_bPlayGroundPoundOnce = true;
			m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		}
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_bPlayGroundPoundOnce = false;
			m_IsAirDash = false;
			m_bCanMove = true;
			m_iJumpCount = 0;
		}
	}

	if (m_eCurPlayerSize != SIZE_LARGE)
	{
		if (m_bAfterGroundPound == true)
			m_iAfterGroundPoundCount += 1;

		if (m_iAfterGroundPoundCount >= 10) // 1.5초 경직
		{
			m_iAfterGroundPoundCount = 0;
			m_bAfterGroundPound = false;
			m_bCanMove = true;
		}
	}
	else if (m_eCurPlayerSize == SIZE_LARGE)
	{
		if (m_bAfterGroundPound == true)
			m_iAfterGroundPoundCount += 1;

		if (m_iAfterGroundPoundCount >= 90) // 1.5초 경직
		{
			m_iAfterGroundPoundCount = 0;
			m_bAfterGroundPound = false;
			m_bCanMove = true;
		}
	}

}
void CCody::Enforce_IdleState()
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

	m_iJumpCount = 0;
	m_iAirDashCount = 0;
	m_bCanMove = true;


	m_bOnRailEnd = false;
	m_IsHitStarBuddy = false;
	m_IsHitRocket = false;
	m_IsActivateRobotLever = false;
	m_IsPushingBattery = false;
	m_IsEnterValve = false;
	m_IsInGravityPipe = false;
	m_IsHitPlanet = false;
	m_IsHookUFO = false;
	m_IsWarpNextStage = false;
	m_IsWarpDone = false;
	m_IsTouchFireDoor = false;
	m_IsBossMissile_Control = false;
	m_IsDeadLine = false;
	m_bWallAttach = false;
	m_bPipeWallAttach = false;
	m_IsControlJoystick = false;
	m_IsPinBall = false;
	m_IsWallLaserTrap_Touch = false;
	m_bRespawn = false;
	m_bElectricWallAttach = false;
	m_IsHolding_UFO = false;
	m_IsInJoyStick = false;
	m_bLaserTennis = false;
	m_IsEnding = false;


	m_pActorCom->Set_IsFalling(false);
	m_pActorCom->Set_ZeroGravity(false, false, false);
	m_pActorCom->Set_Jump(false);
	m_pModelCom->Set_Animation(ANI_C_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
}

#pragma region Shader_Variables
HRESULT CCody::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}
#pragma endregion

#pragma region Trigger
void CCody::SetTriggerID(GameID::Enum eID, _bool IsCollide, _fvector vTriggerTargetPos, _uint _iPlayerName)
{
	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	XMStoreFloat3(&m_vTriggerTargetPos, vTriggerTargetPos);

	/* For.Valve */
	m_iValvePlayerName = _iPlayerName;
}

void CCody::SetTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerTargetWorld, _uint _iPlayerName)
{
	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	XMStoreFloat4x4(&m_TriggerTargetWorld, vTriggerTargetWorld);
	XMStoreFloat3(&m_vTriggerTargetPos, vTriggerTargetWorld.r[3]);

	/* For.Valve */
	m_iValvePlayerName = _iPlayerName;
}

void CCody::SetTriggerID_Ptr(GameID::Enum eID, _bool IsCollide, CGameObject * pTargetPtr)
{
	if (nullptr != m_pTargetPtr)
		Safe_Release(m_pTargetPtr);

	m_eTargetGameID = eID;
	m_IsCollide = IsCollide;
	m_pTargetPtr = pTargetPtr;
	Safe_AddRef(m_pTargetPtr);
}

void CCody::SetCameraTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerCameraWorld)
{
	m_eCameraTriggerID = eID;
	m_IsCamTriggerCollide = IsCollide;
	XMStoreFloat4x4(&m_TriggerCameraWorld, vTriggerCameraWorld);
}

void CCody::SetCameraTriggerID_Pos(_fvector vCamTriggerPos)
{
	XMStoreFloat4(&m_vCamTriggerPos, vCamTriggerPos);
}

_bool CCody::Trigger_Check(const _double dTimeDelta)
{
	if (m_IsCollide == true)
	{
		if (m_eTargetGameID == GameID::eSTARBUDDY && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_PlanetPush_Large);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsHitStarBuddy = true;
		}
		else if (m_eTargetGameID == GameID::eMOONBABOON && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Grapple_ToGrind);
			m_IsOnGrind = true;
		}
		else if (m_eTargetGameID == GameID::eROCKET && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_RocketFirework);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsHitRocket = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTLEVER && m_pGameInstance->Key_Down(DIK_E))
		{
			if (DATABASE->Get_Cody_Stage() == ST_GRAVITYPATH)
			{
				m_pActorCom->Set_Position(XMVectorSet(70.5799332f, 21.3829994f, 174.975174f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			else if (DATABASE->Get_Cody_Stage() == ST_PINBALL)
			{
				//
			}
			else if (DATABASE->Get_Cody_Stage() == ST_RAIL)
			{
				m_pActorCom->Set_Position(XMVectorSet(1035.06592f, 740.905029f, 212.1604f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			m_pModelCom->Set_Animation(ANI_C_Bhv_Lever_Left);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsActivateRobotLever = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTBATTERY && m_pGameInstance->Key_Down(DIK_E))
		{
			if (DATABASE->Get_Cody_Stage() == ST_GRAVITYPATH)
			{
				m_pActorCom->Set_Position(XMVectorSet(71.1877518f, 23.28266802f, 179.770789f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			else if (DATABASE->Get_Cody_Stage() == ST_PINBALL)
			{
				m_pActorCom->Set_Position(XMVectorSet(-814.513655f, 791.810059f, 228.3990845f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			}
			else if (DATABASE->Get_Cody_Stage() == ST_RAIL)
			{
				m_pActorCom->Set_Position(XMVectorSet(1035.44493f, 743.288574f, 216.964385f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_Fwd);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Push_Battery_MH);
			m_IsPushingBattery = true;
		}
		else if (m_eTargetGameID == GameID::eCONTROLROOMBATTERY && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_Fwd);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Push_Battery_MH);
			m_IsPushingBattery = true;
			m_IsPipeBattery = true;

			//UI_Delete(Cody, InputButton_InterActive);
			//((CControlRoom_Battery*)DATABASE->Get_ControlRoom_Battery())->Set_UIDisable(true);
		}
		else if (m_eTargetGameID == GameID::eSPACEVALVE && m_pGameInstance->Key_Down(DIK_E) && m_iValvePlayerName == Player::Cody)
		{
			m_pActorCom->Set_Position(XMVectorSet(46.487f, 124.842f, 195.789f, 1.f));
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_MH, 0.2);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Valve_Rotate_MH);
			m_IsEnterValve = true;
		}
		else if (m_eTargetGameID == GameID::eGRAVITYPIPE)
		{
			if (m_IsInGravityPipe == false)
			{
				m_bShortJump = false;
				m_IsJumping = false;
				m_iJumpCount = 0;
				m_pModelCom->Set_Animation(ANI_C_Bhv_PlayRoom_ZeroGravity_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_PlayRoom_ZeroGravity_MH);
				m_bGoToGravityCenter = true;
				m_vTriggerTargetPos.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 2.f;
			}
			m_IsInGravityPipe = true;
		}
		else if (m_eTargetGameID == GameID::ePLANET && m_pGameInstance->Key_Down(DIK_E))
		{
			if (m_eCurPlayerSize == SIZE_SMALL)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChagneSize_PlanetPush_Small);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
			else if (m_eCurPlayerSize == SIZE_MEDIUM)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_PlanetPush_Medium);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
			else if (m_eCurPlayerSize == SIZE_LARGE)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_PlanetPush_Large);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);

				m_IsHitPlanet = true;
				m_IsHitPlanet_Effect = true;
				m_IsHitPlanet_Once = true;
			}
		}
		else if (m_eTargetGameID == GameID::eHOOKUFO && m_pGameInstance->Key_Down(DIK_F) && m_IsHookUFO == false)
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
				m_vHookUFOAxis = vRight;


				//m_pTransformCom->Rotate_ToTarget(XMLoadFloat3(&m_vTriggerTargetPos));
				_vector vTestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector vTargetPos = m_vHookUFOOffsetPos;

				_vector vDir = vTargetPos - vTestPos;
				_float  fDist = XMVectorGetX(XMVector3Length(vDir));

				_vector vFixUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);/*m_vHookUFOAxis*/;
				_vector vTriggerToPlayer = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_vHookUFOOffsetPos);
				m_fRopeAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vFixUp, vTriggerToPlayer));
				m_faArmLength = fDist;

				//_vector vPosition = XMVectorSet(XMVectorGetX(vTestPos), m_faArmLength * cos(m_fRopeAngle), m_faArmLength * sin(m_fRopeAngle), 1.f) + XMVectorSetW(XMLoadFloat3(&m_vTriggerTargetPos), 1.f);
				XMStoreFloat3(&m_vStartPosition, XMVectorSet(XMVectorGetX(vTestPos), XMVectorGetY(vTestPos), XMVectorGetZ(vTestPos), 1.f)/* + (XMLoadFloat3(&m_vTriggerTargetPos)*/);

			}
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_UFO_THROW, m_fCody_Rope_UFO_Throw_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Throw.wav"), CHANNEL_CHARACTER_UFO_THROW, m_fCody_Rope_UFO_Throw_Volume);

			m_pModelCom->Set_Animation(ANI_C_Bhv_Swinging_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Swinging_Fwd);
			m_IsHookUFO = true;
			m_bGoToHooker = true;
			m_pActorCom->Set_ZeroGravity(true, false, true);
		}
		else if (GameID::eWARPGATE == m_eTargetGameID && false == m_IsWarpNextStage)
		{
			// 코디 전용 포탈로 이동(웜홀)
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_fWarpTimer = 0.f;
			m_IsWarpNextStage	= true;
			m_IsWarpDone		= true;
			XMStoreFloat4x4(&m_TriggerTargetWorld, static_cast<CWarpGate*>(m_pTargetPtr)->Get_NextPortal_Matrix());
			m_pCamera->Set_StartPortalMatrix(static_cast<CWarpGate*>(m_pTargetPtr)->Get_Transform()->Get_WorldMatrix());

			if (5 == static_cast<CWarpGate*>(m_pTargetPtr)->Get_StageValue())
			{
				/* 레이저테니스 UI 지우셈 */
				UI_Delete(Default, Minigame_Score);
				UI_Delete(Default, Minigame_Title);
			}
		}
		else if (GameID::eFIREDOOR == m_eTargetGameID && false == m_IsTouchFireDoor)
		{
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			Enforce_IdleState();
			m_IsTouchFireDoor = true;
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DEAD_BURN, m_fCodyM_Dead_Burn_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dead_Burn.wav"), CHANNEL_CODYM_DEAD_BURN, m_fCodyM_Dead_Burn_Volume);
		}
		else if (GameID::eBOSSMISSILE == m_eTargetGameID && m_pGameInstance->Key_Down(DIK_E) && m_IsBossMissile_Control == false)
		{
			m_pModelCom->Set_Animation(ANI_C_Jump_Start, 28.f);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Rocket_Enter);
			m_IsBossMissile_Control = true;
			m_IsMoveToRocket = true;
			m_pActorCom->Jump_Start(3.f);
		}
		else if (m_eTargetGameID == GameID::eDUMMYWALL && m_bWallAttach == false && m_fWallJumpingTime <= 0.f)
		{
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
			m_bWallAttach = true;
		}
		else if (m_eTargetGameID == GameID::eDEADLINE && false == m_IsDeadLine)
		{
			/* 데드라인 */
			m_pModelCom->Set_Animation(ANI_C_Bhv_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Death_Fall_MH);

			m_pActorCom->Set_ZeroGravity(true, true, true);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_bRespawnCheck = false;
			m_IsDeadLine = true;

			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DEAD_FALL, m_fCodyM_Dead_Fall_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dead_Fall.wav"), CHANNEL_CODYM_DEAD_FALL, m_fCodyM_Dead_Fall_Volume);
		}
		else if (m_eTargetGameID == GameID::eDUMMYWALL && m_pActorCom->Get_IsWallCollide() == true && m_bWallAttach == false
			&& m_IsJumping == true && m_IsFalling == false)
		{
			/*PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
			_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);*/

			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bWallAttach = true;
		}
		else if (m_eTargetGameID == GameID::ePIPEJUMPWALL && m_pActorCom->Get_IsWallCollide() == true && m_bPipeWallAttach == false
			&& m_IsJumping == true && m_IsFalling == false)
		{
			/*PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
			_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);*/

			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bPipeWallAttach = true;
		}
		else if (m_eTargetGameID == GameID::eDUMMYWALLCAMERATRIGGER)
		{
			m_eCameraWorkState = STATE_DUMMYWALL_JUMP;
		}
		else if (m_eTargetGameID == GameID::ePIPEWALLCAMERATRIGGER)
		{
			m_eCameraWorkState = STATE_PIPEWALL_JUMP;
		}
		else if (m_eTargetGameID == GameID::eSAVEPOINT)
		{
			/* 세이브포인트->트리거와 충돌시 세이브포인트 갱신 */
			m_vSavePoint = m_vTriggerTargetPos;
		}
		else if (m_eTargetGameID == GameID::eUMBRELLABEAMJOYSTICK && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_MH);
			m_IsControlJoystick = true;
			CUmbrellaBeam_Joystick* pJoystick = (CUmbrellaBeam_Joystick*)DATABASE->Get_Umbrella_JoystickPtr();
			pJoystick->Set_ControlActivate(true);
			UI_CreateOnlyOnce(Cody, InputButton_Cancle);
			UI_CreateOnlyOnce(Cody, Arrowkeys_All);
			UI_Delete(Cody, InputButton_InterActive);
		}
		else if (m_eTargetGameID == GameID::eWALLLASERTRAP && false == m_IsWallLaserTrap_Touch)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);
			m_pGameInstance->Play_Sound(TEXT("Character_Dead_ElectricShock.wav"), CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);

			m_pModelCom->Set_Animation(ANI_C_Bhv_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			Enforce_IdleState();
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_IsWallLaserTrap_Touch = true;
		}
		else if (m_eTargetGameID == GameID::ePINBALL && false == m_IsPinBall)
		{
			/* 핀볼모드 ON */
			m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_DOOR);
			m_pGameInstance->Play_Sound(TEXT("Pinball_Enter_Ball.wav"), CHANNEL_PINBALL_DOOR);

			m_pActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
			m_IsPinBall = true;
		}
		else if (m_eTargetGameID == GameID::ePINBALLDOOR && m_pGameInstance->Key_Down(DIK_E))
		{
			/* 핀볼 문열기 */
			m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_DOOR);
			m_pGameInstance->Play_Sound(TEXT("Pinball_Door_Open.wav"), CHANNEL_PINBALL_DOOR);

			((CPinBall_Door*)(CDataStorage::GetInstance()->Get_Pinball_Door()))->Set_DoorState(false);
		}
		else if (m_eTargetGameID == GameID::eHOOKAHTUBE)
		{
			/* 튜브*/
			if(2 == ((CHookahTube*)m_pTargetPtr)->Get_Option())
				m_pActorCom->Jump_Start(6.f);
			else
				m_pActorCom->Jump_Start(4.f);

			_uint iRandom = rand() % 4;
			switch (iRandom)
			{
			case 0:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_01);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
				m_IsJumping = true;
				break;
			case 1:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_02);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
				m_IsJumping = true;
				break;
			case 2:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_03);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
				m_IsJumping = true;
				break;
			case 3:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_04);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Falling);
				m_IsJumping = true;
				break;
			default:
				break;
			}
			m_IsCollide = false;
		}
		else if ((m_eTargetGameID == GameID::ePRESS || m_eTargetGameID == GameID::ePEDAL) && false == m_bRespawn)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_DEAD_FALL, m_fCodyM_Dead_Fall_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Dead_Fall.wav"), CHANNEL_CODYM_DEAD_FALL, m_fCodyM_Dead_Fall_Volume);

			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pActorCom->Update(dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bRespawnCheck = false;
			m_bRespawn = true;
		}
		else if ((m_eTargetGameID == GameID::eROTATIONFAN) && false == m_bRespawn)
		{
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pActorCom->Update(dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bRespawnCheck = false;
			m_bRespawn = true;
		}
		else if (m_eTargetGameID == GameID::eELECTRICBOX && false == m_bRespawn)
		{
			if (true == ((CElectricBox*)m_pTargetPtr)->Get_Electric())
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);
				m_pGameInstance->Play_Sound(TEXT("Character_Dead_ElectricShock.wav"), CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);

				CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
				m_pActorCom->Update(dTimeDelta);
				m_pActorCom->Set_ZeroGravity(true, false, true);
				m_bRespawnCheck = false;
				m_bRespawn = true;
			}
		}
		else if (m_eTargetGameID == GameID::eELECTRICWALL && false == m_bRespawn && m_pActorCom->Get_IsWallCollide() == true && m_bElectricWallAttach == false
 			&& m_IsJumping == true && m_IsFalling == false)
		{
			if (true == ((CElectricWall*)m_pTargetPtr)->Get_Electric())
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);
				m_pGameInstance->Play_Sound(TEXT("Character_Dead_ElectricShock.wav"), CHANNEL_CHARACTER_DEAD_ELECTRICSHOCK, m_fCodyM_Dead_Electric_Shock);

 				CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
				m_pActorCom->Update(dTimeDelta);
				m_pActorCom->Set_ZeroGravity(true, false, true);
				m_bRespawnCheck = false;
				m_bRespawn = true;
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
				m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
				m_pActorCom->Set_ZeroGravity(true, false, true);
				m_bElectricWallAttach = true;
			}
		}
		else if (m_eTargetGameID == GameID::eBOSSUFO && m_pGameInstance->Key_Down(DIK_E))
		{
			m_IsHolding_UFO = true;
		}
		else if (m_eTargetGameID == GameID::eBOSSENTERUFO)
		{
			//m_IsCodyEnter = true;
		}
		else if (m_eTargetGameID == GameID::eLASERTENNISPOWERCOORD && m_pGameInstance->Key_Down(DIK_E) && false == m_bLaserTennis)
		{
			m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
			m_pGameInstance->Play_Sound(TEXT("StartButton_Touch&Detach.wav"), CHANNEL_LASERPOWERCOORD);

			LASERTENNIS->Increase_PowerCoord();

			UI_Generator->Delete_InterActive_UI(Player::Cody, UI::PowerCoord);

			m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
			m_pActorCom->Set_Position(XMVectorSet(m_vTriggerTargetPos.x, XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), m_vTriggerTargetPos.z - 3.f, 1.f));

			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);

			m_bLaserTennis = true;
		}
		else if (m_eTargetGameID == GameID::eLASER_LASERTENNIS)
		{
			/* Hit Effect 생성 */
			EFFECT->Add_Effect(Effect_Value::Hit_BossLaser_Particle_Star, m_pTransformCom->Get_WorldMatrix());

			m_bHit = true;

			/* HP 감소 */
			/* 순서 바꾸면 안됨 ㅇㅇ */
			Set_MinigameHpBarReduction(30);
			LASERTENNIS->Set_MayCount();

			m_IsCollide = false;
		}
		else if (m_IsEnding == true && m_bSetEndingOffSetOnce == false)
		{
			m_bSetEndingOffSetOnce = true;
			m_pModelCom->Set_Animation(ANI_C_Rocket_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Rocket_MH);
		}
	}

	if (m_pGameInstance->Key_Down(DIK_F9) && m_IsInJoyStick == false && m_eCurPlayerSize == SIZE_SMALL)
	{
		m_pActorCom->Set_ZeroGravity(true, true, true);
		m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_MH);
		_vector vTargetPosition = XMVectorSet(64.0174942f, 601.063843f + 0.076f, 1011.77844f, 1.f);
		_vector vOffSetPosition = XMVectorSet(64.0174942f + 0.04f, 601.063843f + 0.076f, 1011.77844f - 0.04f - 0.5f, 1.f);

		m_pActorCom->Set_Position(vOffSetPosition);
		m_pTransformCom->Rotate_ToTargetOnLand(vTargetPosition);
		m_IsInJoyStick = true;
	}

	// Trigger 여따가 싹다모아~
	if (m_bOnRailEnd || m_IsHitStarBuddy || m_IsHitRocket || m_IsActivateRobotLever || m_IsPushingBattery || m_IsEnterValve || m_IsInGravityPipe
		|| m_IsHitPlanet || m_IsHookUFO || m_IsWarpNextStage || m_IsWarpDone || m_IsTouchFireDoor || m_IsBossMissile_Control || m_IsDeadLine
		|| m_bWallAttach || m_bPipeWallAttach || m_IsControlJoystick || m_IsPinBall || m_IsWallLaserTrap_Touch || m_bRespawn || m_bElectricWallAttach || m_IsHolding_UFO
		|| m_bLaserTennis || m_IsInJoyStick || m_IsEnding)
		return true;

	return false;
}

_bool CCody::Trigger_End(const _double dTimeDelta)
{
	if ((m_pModelCom->Get_CurAnimIndex() == ANI_C_Jump_Land 
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ChangeSize_PlanetPush_Large
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_RocketFirework
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Lever_Left
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Push_Exit
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Valve_Rotate_MH
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_PlayRoom_ZeroGravity_MH
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Slide_Enter_Dash))
	{
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
	}

	if (m_pModelCom->Is_AnimFinished(ANI_C_Jump_Land_High))
	{
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_bLandHigh = false;
	}

	if (ANI_C_Jump_Land_High == m_pModelCom->Get_CurAnimIndex())
	{
		m_bLandHigh = true;
	}

	return false;
}
#pragma endregion

void CCody::Hit_StarBuddy(const _double dTimeDelta)
{
	if (m_IsHitStarBuddy == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));

		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_ChangeSize_PlanetPush_Large))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_IsHitStarBuddy = false;
			m_IsCollide = false;
		}
	}
}

void CCody::Hit_Rocket(const _double dTimeDelta)
{
	if (m_IsHitRocket == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_RocketFirework))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_IsHitRocket = false;
			m_IsCollide = false;
		}
	}
}

void CCody::Activate_RobotLever(const _double dTimeDelta)
{
	if (m_IsActivateRobotLever == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_Lever_Left))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsActivateRobotLever = false;	
			m_IsCollide = false;

		}

	}

}

void CCody::Push_Battery(const _double dTimeDelta)
{
	// May가 배터리 들어온 상태에서 Lever 치고 컷씬이 등장하면 그때 -> ANI_C_MH
	if (m_IsPushingBattery == true)
	{
		if (true == m_IsPipeBattery)
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_MH);
			m_IsCollide = false;

			if (m_pGameInstance->Key_Down(DIK_Q))
			{
				m_IsPushingBattery = false;
				m_IsPipeBattery = false;
				m_pModelCom->Set_Animation(ANI_C_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
		}

		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_Push_Battery_MH))
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_MH);
	}
	if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_GRAVITYPATH)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Push_Battery_Fwd)
		{
			m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK) / 95.f, dTimeDelta );
		}

		if (DATABASE->Get_GravityStageClear() == true)
		{
			m_IsPushingBattery = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsStGravityCleared = true;
		}

	}

	else if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_RAIL)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Push_Battery_Fwd)
		{
			m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK) / 95.f, dTimeDelta);
		}
		if (DATABASE->Get_RailStageClear() == true)
		{
			m_IsPushingBattery = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsStRailCleared = true;
		}
	}

	else if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_PINBALL)
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Push_Battery_Fwd)
		{
			m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK) / 95.f, dTimeDelta);
		}
		if (DATABASE->Get_PinBallStageClear() == true)
		{
			m_IsPushingBattery = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsStPinBallCleared = true;
			m_pActorCom->Set_Scale(0.5f, 0.5f);
			m_vScale = { 1.f, 1.f, 1.f };
			m_eCurPlayerSize = SIZE_MEDIUM;
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
		}
	}

	
}

void CCody::Rotate_Valve(const _double dTimeDelta)
{
	if (m_IsEnterValve == true)
	{
		if (DATABASE->Get_ValveCount() == 6)
		{
			m_bStruggle = false;
			m_iRotateCount = 0;
			m_IsEnterValve = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_C_MH);
			DATABASE->Add_ValveCount_Cody(true);

			UI_Create(Cody, Arrowkeys_Side);
			UI_Create(May, StickIcon);
		}

		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pGameInstance->Key_Down(DIK_D) && m_pModelCom->Get_CurAnimIndex() != ANI_C_Bhv_Valve_Rotate_R && m_bStruggle == false)
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_R);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Valve_Rotate_MH);
			m_iRotateCount += 1;
			DATABASE->Add_ValveCount_Cody(true);
		}
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_Valve_Rotate_R))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_MH, 0.2);
			//m_IsEnterValve = false;
			//m_IsCollide = false;
			if (m_iRotateCount == 3)
			{
				m_bStruggle = true;
				m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_StruggleRightEnter);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Valve_StruggleRight);
				DATABASE->Set_ValveCount(m_iRotateCount);
			}
		}
	}
}

void CCody::In_GravityPipe(const _double dTimeDelta)
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
			if (m_pGameInstance->Key_Pressing(DIK_SPACE))
			{
				m_pActorCom->Set_ZeroGravity(true, true, false);

				if (m_bGravityPipe_FirstIn == false)
				{
					SCRIPT->Render_Script(0, CScript::HALF, 2.f);
					m_pGameInstance->Set_SoundVolume(CHANNEL_VOICE_CODY_1, m_fCody_GravityPipe_Voice_Volume);
					m_pGameInstance->Play_Sound(TEXT("01.wav"), CHANNEL_VOICE_CODY_1, m_fCody_GravityPipe_Voice_Volume);
					m_bGravityPipe_FirstIn = true;
				}
			}

			if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				m_pActorCom->Set_ZeroGravity(true, false, false);
			}

			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Key_Pressing(DIK_S))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
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
			m_pActorCom->Jump_Start(1.9f);
			m_pActorCom->Set_Jump(true);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pActorCom->Set_Gravity(-9.8f);
			m_IsInGravityPipe = false;
			//m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_Animation(ANI_C_Jump_180R);
		}
	}
}

void CCody::Hit_Planet(const _double dTimeDelta)
{
	if (m_IsHitPlanet == true)
	{
		if (0.2f <= m_pModelCom->Get_ProgressAnim())
		{
			if (true == m_IsHitPlanet_Once)
			{
				((CHangingPlanet*)(m_pTargetPtr))->Hit_Planet(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				/* Sound */
				m_pGameInstance->Stop_Sound(CHANNEL_HANGINGPLANET);
				m_pGameInstance->Play_Sound(TEXT("HangingPlanet_Push.wav"), CHANNEL_HANGINGPLANET);
				m_IsHitPlanet_Once = false;
			}
		}

		if (0.38175f <= m_pModelCom->Get_ProgressAnim())
		{
			if (true == m_IsHitPlanet_Effect)
			{
				EFFECT->Add_Effect(Effect_Value::Hit_Planet_Smoke);
				EFFECT->Add_Effect(Effect_Value::Hit_Planet_Smoke);
				EFFECT->Add_Effect(Effect_Value::Hit_Planet_Particle);
				EFFECT->Add_Effect(Effect_Value::Hit_Planet_Particle);
				EFFECT->Add_Effect(Effect_Value::Hit_Planet_Particle);
				m_IsHitPlanet_Effect = false;
			}
		}


		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_ChangeSize_PlanetPush_Large))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_IsHitPlanet = false;
			m_IsCollide = false;
			m_IsHitPlanet_Effect = false;
		}
	}
}

void CCody::Hook_UFO(const _double dTimeDelta)
{
	if (m_IsHookUFO == true)
	{
		//if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_UFO_MOVE) == false)
		//{
		//	m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_UFO_MOVE, m_fCody_Rope_UFO_Move_Volume);
		//	m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Move"), CHANNEL_CHARACTER_UFO_MOVE, m_fCody_Rope_UFO_Move_Volume);
		//}

		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_UFO_THROW) == false && m_bUFOCatchSoundOnce == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fCody_Rope_UFO_Throw_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Catch.wav"), CHANNEL_CHARACTER_ROPE_UFO_CATCH, m_fCody_Rope_UFO_Throw_Volume);
			m_bUFOCatchSoundOnce = true;
		}
		// 이동
		_float Gravity = -0.3f;

		m_faAcceleration = (-1.f * Gravity / m_faArmLength) * sin(m_fRopeAngle);

		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_faAcceleration += (_float)dTimeDelta;
		if (m_pGameInstance->Key_Pressing(DIK_S))
			m_faAcceleration -= (_float)dTimeDelta;
		m_faVelocity += m_faAcceleration;
		m_faVelocity *= m_faDamping;
		m_fRopeAngle += m_faVelocity / 50.f;
		
		_vector vPosition =	XMVectorSet( (m_vHookUFOOffsetPos.m128_f32[0] - m_vStartPosition.x ) * sin(-m_fRopeAngle), (m_vHookUFOOffsetPos.m128_f32[1] - m_vStartPosition.y) * cos(m_fRopeAngle), ((m_vHookUFOOffsetPos.m128_f32[2] - m_vStartPosition.z) * sin(-m_fRopeAngle)), 1.f);
		m_pActorCom->Set_Position(m_vHookUFOOffsetPos + vPosition);


		// 회전
		_vector vTriggerToPlayer = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION),0.f) - XMVectorSetY(m_vHookUFOOffsetPos, 0.f));
		vTriggerToPlayer = XMVectorSetW(vTriggerToPlayer, 1.f);
		m_pTransformCom->RotateYawDirectionOnLand(-vTriggerToPlayer, (_float)dTimeDelta / 2.f);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (m_pGameInstance->Key_Down(DIK_SPACE)) // 로프 놓기
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_UFO_RELEASE, m_fCody_Rope_UFO_Release_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_Rope_UFO_Release.wav"), CHANNEL_CHARACTER_UFO_RELEASE, m_fCody_Rope_UFO_Release_Volume);

			m_bUFOCatchSoundOnce = false;
			m_bGoToHooker = false; 
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			m_pModelCom->Set_Animation(ANI_C_Bhv_Swinging_ExitFwd);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_180R);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pActorCom->Set_Gravity(-9.8f);
			m_pActorCom->Set_IsFalling(true);
			m_pActorCom->Jump_Start(3.5f);
			m_pActorCom->Set_Jump(true);
			m_IsHookUFO = false;
			m_IsCollide = false;
			((CHookUFO*)DATABASE->Get_HookUFO())->Set_CodyUIDisable();
		}
	}
}

void CCody::Wall_Jump(const _double dTimeDelta)
{
	if (true == m_bWallAttach && false == m_IsWallJumping)
	{
		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_WALLJUMP_SLIDE) == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
		}

		m_pActorCom->Move((-m_pTransformCom->Get_State(CTransform::STATE_UP) / 50.f), dTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_WallJump.wav"), CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);

			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_IsWallJumping = true;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Jump);
			m_pActorCom->Jump_Start(2.7f);
			m_pActorCom->Set_WallCollide(false);
		}
	}

	if (m_IsWallJumping == true)
	{
		if(m_fWallToWallSpeed <= 50.f)
			m_fWallToWallSpeed += (_float)dTimeDelta * 57.f;

		PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
		_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
		m_pActorCom->Move(XMVector3Normalize(vWallUp) / m_fWallToWallSpeed, dTimeDelta);
		m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);

		if (m_pModelCom->Is_AnimFinished(ANI_C_WallSlide_Jump))
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
			m_bWallAttach = false;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.55f;
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(179.f));
		}
		if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == true)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bWallAttach = true;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.55f;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
		}
		else if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == false)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_bWallAttach = false;
			m_IsWallJumping = false;
			m_fWallJumpingTime = 0.f;
			m_fWallToWallSpeed = 0.5f;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_pActorCom->Set_WallCollide(false);
		}

		m_fWallJumpingTime += (_float)dTimeDelta;
	}
}

void CCody::Pipe_WallJump(const _double dTimeDelta)
{
	if (true == m_bPipeWallAttach && false == m_IsPipeWallJumping)
	{
		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_WALLJUMP_SLIDE) == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
		}

		m_fPipeWallToWallSpeed = 55.f;
		m_pActorCom->Move((-m_pTransformCom->Get_State(CTransform::STATE_UP) / 50.f), dTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_WallJump.wav"), CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);

			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_IsPipeWallJumping = true;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Jump);
			m_pActorCom->Jump_Start(1.1f);
			m_pActorCom->Set_WallCollide(false);
		}
	}

	if (m_IsPipeWallJumping == true)
	{
		if (m_fPipeWallToWallSpeed <= 200.f)
			m_fPipeWallToWallSpeed += (_float)dTimeDelta * 40.f;

		PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
		_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
		m_pActorCom->Move(XMVector3Normalize(vWallUp) / m_fPipeWallToWallSpeed * 0.8f, dTimeDelta);
		m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);

		if (m_pModelCom->Is_AnimFinished(ANI_C_WallSlide_Jump))
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
			m_bPipeWallAttach = false;
			m_IsPipeWallJumping = false;
			m_fPipeWallJumpingTime = 0.f;
			m_fPipeWallToWallSpeed = 55.f;
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(179.f));
		}
		if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == true)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bPipeWallAttach = true;
			m_IsPipeWallJumping = false;
			m_fPipeWallToWallSpeed = 55.f;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
		}
		else if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == false)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_bPipeWallAttach = false;
			m_IsPipeWallJumping = false;
			m_fPipeWallToWallSpeed = 55.f;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_pActorCom->Set_WallCollide(false);
		}

	}
}

void CCody::ElectricWallJump(const _double dTimeDelta)
{
	if (true == m_bElectricWallAttach && false == m_IsElectricWallJumping)
	{
		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_CHARACTER_WALLJUMP_SLIDE) == false)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
			m_pGameInstance->Play_Sound(TEXT("Character_WallJump_Slide.wav"), CHANNEL_CHARACTER_WALLJUMP_SLIDE, m_fCody_WallJump_Slide_Volume);
		}
		m_fElectricWallToWallSpeed = 55.f;
		m_pActorCom->Move((-m_pTransformCom->Get_State(CTransform::STATE_UP) / 50.f), dTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_CHARACTER_WALLJUMP_SLIDE);
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_WallJump.wav"), CHANNEL_CODYM_WALLJUMP, m_fCodyM_WallJump_Volume);

			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_IsElectricWallJumping = true;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Jump);
			m_pActorCom->Jump_Start(1.1f);
			m_pActorCom->Set_WallCollide(false);
		}
	}

	if (m_IsElectricWallJumping == true)
	{
		if (m_fElectricWallToWallSpeed <= 200.f)
			m_fElectricWallToWallSpeed += (_float)dTimeDelta * 40.f;

		PxVec3 vNormal = m_pActorCom->Get_CollideNormal();
		_vector vWallUp = { vNormal.x, vNormal.y, vNormal.z, 0.f };
		m_pActorCom->Move(XMVector3Normalize(vWallUp) / m_fElectricWallToWallSpeed * 1.1f, dTimeDelta);
		m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);

		if (m_pModelCom->Is_AnimFinished(ANI_C_WallSlide_Jump))
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
			m_bElectricWallAttach = false;
			m_IsElectricWallJumping = false;
			m_fElectricWallJumpingTime = 0.f;
			m_fElectricWallToWallSpeed = 55.f;
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(179.f));
		}
		if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == true)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_bElectricWallAttach = true;
			m_IsElectricWallJumping = false;
			m_fElectricWallToWallSpeed = 55.f;
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
		}
		else if (m_pActorCom->Get_IsWallCollide() == true && m_IsCollide == false)
		{
			PxExtendedVec3 vPhysxContactPos = m_pActorCom->Get_ContactPos();
			_vector vContactPos = XMVectorSet((_float)vPhysxContactPos.x, (_float)vPhysxContactPos.y, (_float)vPhysxContactPos.z, 1.f);
			vWallUp.m128_f32[2] = 0.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vContactPos + (vWallUp));
			//m_pTransformCom->RotateYawDirectionOnLand(-vWallUp, dTimeDelta);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_bElectricWallAttach = false;
			m_IsElectricWallJumping = false;
			m_fElectricWallToWallSpeed = 55.f;
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_pActorCom->Set_WallCollide(false);
		}
	}
}

void CCody::BossMissile_Control(const _double dTimeDelta)
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

			if (fLength <= 0.12f)
			{
				m_pActorCom->Set_ZeroGravity(true, false, true);
				m_IsMoveToRocket = false;
				m_pActorCom->Set_Position(m_vRocketOffSetPos);
			}
			else
			{
				m_pTransformCom->RotateYawDirectionOnLand(XMVectorSetY(vRocketLook, 0.f), dTimeDelta);
				m_pActorCom->Move((vNormDir) / 10.f * fLength, dTimeDelta);
			}

		}

		if (m_pModelCom->Is_AnimFinished(ANI_C_Rocket_Enter))
		{
			m_pModelCom->Set_Animation(ANI_C_Rocket_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Rocket_MH);
		}

		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Rocket_MH)
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
		m_pActorCom->Jump_Start(2.2f);
		m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
		m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Land_High);
	}

}

void CCody::Ride_Ending_Rocket(const _double dTimeDelta)
{
	if (m_IsEnding == true)
	{
		/* 3초후 시작 */
		m_dStartTime += dTimeDelta;
		//////////if (3.f <= m_dStartTime && false == m_bEndingCheck)
		//////////{
		//////////	ENDINGCREDIT->Start_EndingCredit();
		//////////	m_bEndingCheck = true;
		//////////}

		m_pModelCom->Set_Animation(ANI_C_Rocket_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_Rocket_MH);

		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Rocket_MH)
		{
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_pTransformCom->Set_WorldMatrix(m_matEndingRocketMatrix);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vEndingRocketOffSetPos);
			m_pActorCom->Set_Position(m_vEndingRocketOffSetPos);
		}
	}
}

void CCody::Warp_Wormhole(const _double dTimeDelta)
{
	if (false == m_IsWarpNextStage && false == m_IsWarpDone)
		return;

	m_fWarpTimer += (_float)dTimeDelta;

	if (true == m_IsWarpNextStage)
	{
		if (m_fWarpTimer_InWormhole/*2.f*/ <= m_fWarpTimer)
		{
			_float4 vWormhole = m_vWormholePos;
			vWormhole.z -= 1.f;
			m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat4(&vWormhole));

			m_pModelCom->Set_Animation(ANI_C_SpacePortal_Travel);
			m_pModelCom->Set_NextAnimIndex(ANI_C_SpacePortal_Travel);

			m_pActorCom->Set_Position(XMLoadFloat4(&m_vWormholePos));
		}

		if (m_fWarpTimer_InWormhole + m_fWarpTimer_Max <= m_fWarpTimer)
		{
			m_pModelCom->Set_Animation(ANI_C_SpacePortal_Travel);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
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

void CCody::Touch_FireDoor(const _double dTimeDelta) // eFIREDOOR
{
	if (false == m_IsTouchFireDoor)
		return;

	m_dDeadTime += dTimeDelta;
	if (m_dDeadTime >= 2.f && m_dDeadTime <= 2.4f)
	{
		_float fMyPosZ = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];
		_float fTriggerPosZ = m_vTriggerTargetPos.z;

		_vector vSavePosition = XMLoadFloat3(&m_vSavePoint);
		//if (fTriggerPosZ < fMyPosZ)
		//{
		//	vSavePosition.m128_f32[1] += 0.7f;
		//	vSavePosition = XMVectorSetW(vSavePosition, 1.f);
		//}
		//else
			vSavePosition = XMVectorSet(64.f, 0.9f, 25.f, 1.f);
	
		m_pActorCom->Set_Position(vSavePosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);

		m_dDeadTime = 2.5f;
	}
	else if (m_dDeadTime >= 2.5f && m_dDeadTime <= 2.75f)
	{

	}
	else if (m_dDeadTime >= 2.75f)
	{
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_dDeadTime = 0.f;
		m_bCanMove = true;
		m_IsCollide = false;
		m_IsTouchFireDoor = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);

		m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
		m_pGameInstance->Play_Sound(TEXT("CodyM_Resurrection.wav"), CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
	}
}

void CCody::Set_OnParentRotate(_matrix ParentMatrix)
{
	_matrix matWorld, matRotY, matTrans;
	matTrans = XMMatrixTranslation(-0.38f, 0.7f, -0.4f);
	matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));

	matWorld = matRotY * matTrans * ParentMatrix;
	m_pTransformCom->Set_WorldMatrix(matWorld);
}

void CCody::Set_ControlJoystick(_bool IsCheck)
{
	m_IsControlJoystick = IsCheck;
}

void CCody::Set_AnimationRotate(_float fAngle)
{
	m_pTransformCom->RotateYaw_Angle(fAngle);
}

void CCody::Set_ActiveHpBar(_bool IsCheck)
{
	if (nullptr == m_pHpBar)
		return;

	m_pHpBar->Set_Active(IsCheck);
}

void CCody::Set_Change_Size_After_UmbrellaCutScene()
{
	m_IsPushingBattery = false;
	m_IsCollide = false;
	m_pModelCom->Set_Animation(ANI_C_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
	m_IsStPinBallCleared = true;
	m_pActorCom->Set_Scale(0.5f, 0.5f);
	m_vScale = { 1.f, 1.f, 1.f };
	m_eCurPlayerSize = SIZE_MEDIUM;
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
}
void CCody::Set_HpBarReduction(_float fDamage)
{
	if (nullptr == m_pHpBar || nullptr == m_pSubHpBar)
		return;

	m_pHpBar->Set_Hp(fDamage);
	m_pSubHpBar->Set_Active(true);
	m_pSubHpBar->Set_Hp(fDamage);
}

void CCody::Set_ActiveMinigameHpBar(_bool IsCheck)
{
	if (nullptr == m_pMinigameHpBar)
		return;

	m_pMinigameHpBar->Set_Active(IsCheck);
}

void CCody::Set_MinigameHpBarReduction(_float fDamage)
{
	if (nullptr == m_pMinigameHpBar || nullptr == m_pMinigameSubHpBar)
		return;

	m_pMinigameHpBar->Set_Hp(fDamage);
	m_pMinigameSubHpBar->Set_Active(true);
	m_pMinigameSubHpBar->Set_Hp(fDamage);
}

void CCody::Set_MinigameHpBarReset()
{
	m_pMinigameHpBar->Set_ResetHp();
	m_pMinigameSubHpBar->Set_ResetHp();
	m_pMinigameSubHpBar->Set_Active(false);
}

void CCody::WallLaserTrap(const _double dTimeDelta)
{
	if (false == m_IsWallLaserTrap_Touch)
		return;

	m_IsWallLaserTrap_Effect = true;
	m_dDeadTime += dTimeDelta;
	if (m_dDeadTime >= 2.f)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
		m_pGameInstance->Play_Sound(TEXT("CodyM_Resurrection.wav"), CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);

		_float fMyPosY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		_float fTriggerY = m_vTriggerTargetPos.y;

		_vector vRespawnPos = XMVectorSet(-803.f, 768.f, 193.f, 1.f);
		if(fMyPosY > fTriggerY)
			vRespawnPos = XMVectorSet(-805.f, 792.f, 198.f, 1.f);

		m_pActorCom->Set_Position(vRespawnPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vRespawnPos);
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_dDeadTime = 0.f;
		m_IsCollide = false;
		m_IsWallLaserTrap_Touch = false;
		m_IsWallLaserTrap_Effect = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);
	}
}

void CCody::Falling_Dead(const _double dTimeDelta)
{
	/* 데드라인과 충돌시 1초후에 리스폰 */
	if (m_IsDeadLine == true)
	{
		m_dDeadTime += dTimeDelta;
		if (m_dDeadTime >= 1.f)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
			m_pGameInstance->Play_Sound(TEXT("CodyM_Resurrection.wav"), CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);

			_vector vSavePosition = XMLoadFloat3(&m_vSavePoint);
			vSavePosition = XMVectorSetW(vSavePosition, 1.f);

			m_pActorCom->Set_Position(vSavePosition);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_dDeadTime = 0.f;
			m_IsCollide = false;
			m_IsDeadLine = false;
			m_bRespawnCheck = true;
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

#pragma region Path
void CCody::Set_SpaceRailNode(CSpaceRail_Node* pRail)
{
	if (nullptr == pRail) return;
	m_vecTargetRailNodes.push_back(pRail);
}
void CCody::KeyInput_Rail(_double dTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F) && false == m_bOnRail &&
		m_eCurPlayerSize == SIZE_MEDIUM && false == m_IsDeadLine)
	{
		Start_SpaceRail();
		UI_Delete(Cody, InputButton_InterActive_Rail);
	}

	if (m_bOnRail)
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			m_pGameInstance->Stop_Sound(CHANNEL_CODY_RAIL);

			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
			Loop_RadiarBlur(false);

			//m_iAirDashCount = 0;
			m_iJumpCount = 0;
			m_bShortJump = true;

			m_pTargetRail = nullptr;
			m_pSearchTargetRailNode = nullptr;
			m_pTargetRailNode = nullptr;

			m_vecTargetRailNodes.clear();

			m_bMoveToRail = false;
			m_bOnRail = false;
			m_bOnRailEnd = false;
			m_bOnRail_Effect = false;
		}
	}
}
void CCody::Clear_TagerRailNodes()
{
	m_vecTargetRailNodes.clear();
}
void CCody::Find_TargetSpaceRail()
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
			EFFECT->Add_Effect(Effect_Value::Cody_Rail, m_pTransformCom->Get_WorldMatrix());
		}
	}

	if (false == isSearch)
		m_pSearchTargetRailNode = nullptr;
}
void CCody::Start_SpaceRail()
{
	if (nullptr == m_pSearchTargetRailNode) return;

	if (m_pSearchTargetRailNode) {
		// 타겟 지정시, 연기이펙트
		EFFECT->Add_Effect(Effect_Value::Landing_Smoke, m_pSearchTargetRailNode->Get_WorldMatrix());
		// R-Blur
		Loop_RadiarBlur(true);

		// 타겟을 찾았다면, 레일 탈 준비
		m_pTargetRailNode = m_pSearchTargetRailNode;
		m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_Enter); // 줄던지고 댕겨서 날라가기
		m_bMoveToRail = true;
	}
}
void CCody::MoveToTargetRail(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom || false == m_bMoveToRail || nullptr == m_pTargetRailNode || true == m_bOnRail) return;

	m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Grapple_Enter); // 줄던지고 댕겨서 날라가기

	_float fMoveToSpeed = 5.f;
	_float fDist = m_pTransformCom->Move_ToTargetRange(m_pTargetRailNode->Get_Position(), 0.1f, dTimeDelta * fMoveToSpeed);
	if (fDist < 0.15f)
	{
		/* 타는 애니메이션으로 변경 */
		m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_ToGrind); // 레일 착지+
		m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Slow_MH);

		/* 타야할 Path 지정 */
		m_pTargetRail = (CSpaceRail*)DATABASE->Get_SpaceRail(m_pTargetRailNode->Get_RailTag());

		/* 예외처리 - 레일 태그 틀렸을 경우 */
		if (nullptr == m_pTargetRail)
		{
			m_pTargetRailNode = nullptr;
			m_bOnRail = false;
			m_bMoveToRail = false;
			m_bOnRail_Effect = false;
			return;
		}

		/* Edge Type에 따라서 Process / Rewind 결정 */
		CSpaceRail::EDGE eEdgeState = (CSpaceRail::EDGE)m_pTargetRailNode->Get_EdgeState();

		CPath::STATE ePathState = CPath::STATE_END;
		if (CSpaceRail::EDGE_FIRST == eEdgeState || CSpaceRail::EDGE_FIRST_END == eEdgeState)
			ePathState = CPath::STATE_FORWARD;
		else if (CSpaceRail::EDGE_LAST == eEdgeState || CSpaceRail::EDGE_LAST_END == eEdgeState)
			ePathState = CPath::STATE_BACKWARD;

		m_pTargetRail->Start_Path(CSpaceRail::SUBJ_CODY, ePathState, m_pTargetRailNode->Get_FrameIndex(), true);

		/* 카메라가 레일타는 방향으로 세팅 */
		//m_pCamera->Get_Transform()->Set_State();

		/* 레일 앞까지 도착, 레일 타기 시작 */
		m_pTargetRailNode = nullptr;
		m_bOnRail = true;
		m_bMoveToRail = false;
		if (false == m_bOnRail_Effect)
			EFFECT->Add_Effect(Effect_Value::Cody_Rail, m_pTransformCom->Get_WorldMatrix());
		m_bOnRail_Effect = true;

		m_pGameInstance->Set_SoundVolume(CHANNEL_CODY_RAIL, m_fRailSoundVolume);
		m_pGameInstance->Play_Sound(TEXT("Rail_Ride.wav"), CHANNEL_CODY_RAIL, m_fRailSoundVolume, true);
	}
}
void CCody::TakeRail(_double dTimeDelta)
{
	if (nullptr == m_pTargetRail || false == m_bOnRail || true == m_bOnRailEnd) return;

	/* 타는 애니메이션으로 변경 */
	if(m_pGameInstance->Key_Pressing(DIK_A))
		m_pModelCom->Set_Animation(ANI_C_Grind_Slow_MH_Left);
	else if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pModelCom->Set_Animation(ANI_C_Grind_Slow_MH_Right);
	else
		m_pModelCom->Set_Animation(ANI_C_Grind_Slow_MH); // 메이 blend 수치값 잡아야함.

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_bOnRail = m_pTargetRail->Take_Path(CSpaceRail::SUBJ_CODY, dTimeDelta, WorldMatrix);
	if (m_bOnRail) {
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}
	else
	{
		m_pGameInstance->Stop_Sound(CHANNEL_CODY_RAIL);
		m_pGameInstance->Set_SoundVolume(CHANNEL_CODY_RAIL, m_fRailSoundVolume);
		m_pGameInstance->Play_Sound(TEXT("Rail_End.wav"), CHANNEL_CODY_RAIL, m_fRailSoundVolume);

		m_pTargetRail = nullptr;
		m_pTargetRailNode = nullptr;
		m_pSearchTargetRailNode = nullptr;
		m_pModelCom->Set_Animation(ANI_C_Jump_Falling);
		m_bOnRailEnd = true;
	}
}

void CCody::TakeRailEnd(_double dTimeDelta)
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

void CCody::ShowRailTargetTriggerUI()
{
	// Show UI
	if (m_pSearchTargetRailNode && nullptr == m_pTargetRailNode && false == m_bOnRail)
	{
		m_pGauge_Circle->Set_Active(true);
		m_pGauge_Circle->Set_TargetPos(m_pSearchTargetRailNode->Get_Position());
	}
	else {
		m_pGauge_Circle->Set_Active(false);
		UI_Delete(Cody, InputButton_InterActive_Rail);
		m_pGauge_Circle->Set_DefaultSetting();

	}
}
#pragma endregion

HRESULT CCody::Ready_Layer_Gauge_Circle(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	_uint iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Gauge_Circle"), &iOption, &pGameObject), E_FAIL);
	m_pGauge_Circle = static_cast<CGauge_Circle*>(pGameObject);
	m_pGauge_Circle->Set_SwingPointPlayerID(Player::Cody);
	// 범위 설정
	m_pGauge_Circle->Set_Range(20.f);

	return S_OK;
}

void CCody::In_JoyStick(_double dTimeDelta)
{
	if (true == m_IsInJoyStick)
	{
		if (m_pGameInstance->Key_Up(DIK_W) || m_pGameInstance->Key_Up(DIK_S) || m_pGameInstance->Key_Up(DIK_A) || m_pGameInstance->Key_Up(DIK_D))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_MH);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_Bck)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Fwd);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_MH && m_pModelCom->Get_ProgressAnim() > 0.2f)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_Fwd);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Fwd);
			}
		} 
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_Right)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Left);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_MH && m_pModelCom->Get_ProgressAnim() > 0.2f)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_Left);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Left);
			}
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_Fwd)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Bck);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_MH && m_pModelCom->Get_ProgressAnim() > 0.2f)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_Bck);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Bck);
			}
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_Fwd)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_MH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Right);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ArcadeScreenLever_MH && m_pModelCom->Get_ProgressAnim() > 0.2f)
			{
				m_pModelCom->Set_Animation(ANI_C_Bhv_ArcadeScreenLever_Right);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ArcadeScreenLever_Right);
			}
		}
		if (m_pGameInstance->Key_Down(DIK_F10))
		{
			m_IsInJoyStick = false;
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_C_MH);
		}
	}
}

#pragma region RadiarBlur
void CCody::Start_RadiarBlur_FullScreen(_double dBlurTime)
{	
	//if (m_bRadiarBlur) return;

	m_bRadiarBlur_FullScreen = true;

	m_bRadiarBlur_Trigger = true;
	m_dRadiarBlurTime = dBlurTime;
	m_dRadiarBlurDeltaT = 0.0;

	Set_RadiarBlur(true);
}

void CCody::Start_RadiarBlur(_double dBlurTime)
{
	//if (m_bRadiarBlur) return;
	m_bRadiarBlur_FullScreen = false;

	m_bRadiarBlur_Trigger = true;
	m_dRadiarBlurTime = dBlurTime;
	m_dRadiarBlurDeltaT = 0.0;

	Set_RadiarBlur(true);
}

void CCody::Loop_RadiarBlur(_bool bLoop)
{
	m_bRadiarBlur_FullScreen = false;

	m_bRadiarBlur_Loop = bLoop;

	if(m_bRadiarBlur_Loop)
		Set_RadiarBlur(true);
	else
		Set_RadiarBlur(false);
}

void CCody::Trigger_RadiarBlur(_double dTimeDelta)
{
	if (m_bRadiarBlur_Loop)
	{
		Set_RadiarBlur(true);
	}
	else if(m_bRadiarBlur_Trigger)
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

void CCody::Set_RadiarBlur(_bool bActive)
{
	_matrix CombineViewMatrix, CombineProjMatrix;

	CombineViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINVIEW);
	CombineProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINPROJ);

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

	D3D11_VIEWPORT Viewport;
	
	if (m_bRadiarBlur_FullScreen)
		Viewport = m_pGameInstance->Get_ViewportInfo(0);
	else
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

	vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f);
	vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f);

	_float2 vFocusPos = { vConvertPos.x / g_iWinCX , vConvertPos.y / g_iWinCY };
	vFocusPos.y -= 0.08f; // Offset 0.04f

	if (m_bRadiarBlur_FullScreen) 
	{
		m_pGameInstance->Set_RadiarBlur_FullScreen(bActive, vFocusPos);
		if (false == bActive) m_bRadiarBlur_FullScreen = false;
	}
	else
		m_pGameInstance->Set_RadiarBlur_Main(bActive, vFocusPos);
}
#pragma endregion

void CCody::PinBall(const _double dTimeDelta)
{
	if (true == m_IsPinBall)
		m_pActorCom->Set_Position(((CDynamic_Env*)(CDataStorage::GetInstance()->Get_Pinball()))->Get_Position());
}

void CCody::PinBall_Script(const _double dTimeDelta)
{
	if (false == m_bPinBallScript)
		return;

	m_dScriptTime += dTimeDelta;

	if (0.5f <= m_dScriptTime && 0 == m_iScriptCount)
	{
		/* 첫번째 */
		/* 메이가 잘못했을 때 */
		if (false == ((CPinBall*)DATABASE->Get_Pinball())->Get_DeadType())
			SCRIPT->Render_Script(41, CScript::HALF, 2.f);

		/* 코디가 잘못했을 때 */
		else
			SCRIPT->Render_Script(39, CScript::HALF, 2.f);

		++m_iScriptCount;
	}
	if (2.8f <= m_dScriptTime && 1 == m_iScriptCount)
	{
		/* 두번째 */
		/* 메이가 잘못했을 때 */
		if (false == ((CPinBall*)DATABASE->Get_Pinball())->Get_DeadType())
			SCRIPT->Render_Script(42, CScript::HALF, 2.f);

		/* 코디가 잘못했을 때 */
		else
			SCRIPT->Render_Script(40, CScript::HALF, 2.f);

		m_iScriptCount = 0;
		m_dScriptTime = 0.0;
		m_bPinBallScript = false;
	}
}

void CCody::Holding_BossUFO(const _double dTimeDelta)
{
	if (false == m_IsHolding_UFO)
		return;

	if (true == m_pModelCom->Is_AnimFinished(ANI_C_CutScene_BossFight_LaserRippedOff))
		m_IsHolding_UFO = false;

	if (CUFO::PHASE_1 == ((CUFO*)DATABASE->Get_BossUFO())->Get_BossPhase() && m_pGameInstance->Key_Down(DIK_E) &&
		m_eCurPlayerSize == CCody::SIZE_LARGE && false == m_IsHolding_Low_UFO)
	{
		m_pActorCom->Set_Position(XMVectorSet(57.8480f, 342.83f, 199.5068f, 1.f));
		m_pModelCom->Set_Animation(ANI_C_Holding_Enter_UFO);
		m_pModelCom->Set_NextAnimIndex(ANI_C_Holding_Low_UFO);
		m_pTransformCom->RotateYaw_Angle(190.f);
		((CUFO*)DATABASE->Get_BossUFO())->Set_UFOAnimation(UFO_CodyHolding_Enter, UFO_CodyHolding_low);
		m_IsHolding_Low_UFO = true;
	}
	else if (CUFO::PHASE_1 == ((CUFO*)DATABASE->Get_BossUFO())->Get_BossPhase() && m_pGameInstance->Key_Down(DIK_E) &&
		m_eCurPlayerSize == CCody::SIZE_MEDIUM && false == m_IsHolding_Low_UFO)
	{
		/* 점프해서 손 흔드는 애니메이션 재생시켜주자 ㅇㅇ;; */
	}

	if (true == m_IsHolding_Low_UFO && false == m_IsHolding_High_UFO)
	{
		if (m_pGameInstance->Key_Down(DIK_E))
			++m_iKeyDownCount;

		if (10 <= m_iKeyDownCount)
		{
			m_pModelCom->Set_Animation(ANI_C_Holding_UFO);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Holding_UFO);
			((CUFO*)DATABASE->Get_BossUFO())->Set_UFOAnimation(UFO_CodyHolding, UFO_CodyHolding);
			((CMay*)DATABASE->GetMay())->Set_LaserRippedOff();
			m_IsHolding_High_UFO = true;
		}
	}
}

void CCody::LaserTennis(const _double dTimeDelta)
{
	if (false == m_bLaserTennis)
		return;

	if (true == LASERTENNIS->Get_PushCoord())
	{
		if (false == m_bCheckAnim)
		{
			m_pActorCom->Jump_Start(2.f);

			m_pModelCom->Set_Animation(ANI_C_Bhv_RocketFirework);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_bCheckAnim = true;
		}
		else if (0.7f <= m_pModelCom->Get_ProgressAnim())
		{
			LASERTENNIS->Push_Coord();
			m_bLaserTennis = false;
			m_bCheckAnim = false;
			return;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
		m_pGameInstance->Play_Sound(TEXT("StartButton_Touch&Detach.wav"), CHANNEL_LASERPOWERCOORD);

		LASERTENNIS->Decrease_PowerCoord();
		m_bLaserTennis = false;
	}

	if (m_pGameInstance->Key_Down(DIK_E))
	{
		UI_Generator->Delete_InterActive_UI(Player::Cody, UI::PowerCoord);
		LASERTENNIS->KeyCheck(CLaserTennis_Manager::TARGET_CODY);
	}
}

void CCody::PinBall_Respawn(const _double dTimeDelta)
{
	m_pActorCom->Set_Position(XMVectorSet(-650.f, 760.f, 195.f, 1.f));
	m_pActorCom->Update(dTimeDelta);

	m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
	m_pGameInstance->Play_Sound(TEXT("CodyM_Resurrection.wav"), CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);

	CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
	m_pActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	m_pModelCom->Set_Animation(ANI_C_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_C_MH);

	if (false == ((CPinBall_Handle*)(DATABASE->Get_Pinball_Handle()))->Get_Goal())
	{
		/* Sound */
		/* 메이가 잘못했을 때 */
		if (false == ((CPinBall*)DATABASE->Get_Pinball())->Get_DeadType())
			m_pGameInstance->Play_Sound(TEXT("22.wav"), CHANNEL_PINBALLVOICE);
		/* 코디가 잘못했을 때 */
		else
			m_pGameInstance->Play_Sound(TEXT("21.wav"), CHANNEL_PINBALLVOICE);
		m_bPinBallScript = true;
	}

	m_IsPinBall = false;
	m_IsCollide = false;
}

void CCody::SpaceShip_Respawn(const _double dTimeDelta)
{
	if (false == m_bRespawn)
		return;

	m_dRespawnTime += dTimeDelta;
	if (2.f <= m_dRespawnTime)
	{
		m_pGameInstance->Set_SoundVolume(CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);
		m_pGameInstance->Play_Sound(TEXT("CodyM_Resurrection.wav"), CHANNEL_CODYM_RESURRECTION, m_fCodyM_Revive_Volume);

		m_pActorCom->Set_ZeroGravity(false, false, false);
		m_pActorCom->Set_Position(XMLoadFloat3(&m_vSavePoint));
		m_pActorCom->Update(dTimeDelta);
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);

		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);

		m_bRespawnCheck = true;
		m_bFirstCheck = false;
		m_bRespawn = false;
		m_IsCollide = false;
		m_dRespawnTime = 0.0;
	}
}
