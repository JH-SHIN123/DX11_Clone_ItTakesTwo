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
/* For. PinBall */
#include "PinBall.h"
#include "PinBall_Door.h"
#include "HangingPlanet.h"

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


 	UI_Create(Cody, PC_Mouse_Reduction);
 	UI_Create(Cody, PC_Mouse_Enlargement);
 	//UI_Create(Default, LoadingBook);
 	//UI_Create(May, Arrowkeys_Side);
 	//UI_Create(May, StickIcon);
 
 	UI_Create(Cody, PlayerMarker);
	UI_Create_Active(Cody, InputButton_InterActive, false);
	 
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

	//Effect 
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_Cody_Size"), nullptr, (CGameObject**)&m_pEffect_Size), E_FAIL);
	m_pEffect_Size->Set_Model(m_pModelCom);
	return S_OK;
}

void CCody::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Jump_Start, false);
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
	//m_pModelCom->Add_LerpInfo(ANI_C_Roll_Stop, ANI_C_Roll_To_Jog, true, 30.f);
	//m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_To_Jog, true, 30.f);

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

	//m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_MH, true, 10.f);
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
	//m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_MH, ANI_C_WallSlide_Jump, true, 10.f);
	//ANI_C_Roll_Start, ANI_C_Roll_Stop;
	return;
}

#pragma endregion

#pragma region Overrided Function
_int CCody::Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);

	/* UI */
	UI_Generator->Set_TargetPos(Player::May, UI::PlayerMarker, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pCamera = (CMainCamera*)CDataStorage::GetInstance()->Get_MainCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	//tEST
	_vector vTestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(m_pGameInstance->Key_Pressing(DIK_J))
		m_pActorCom->Set_ZeroGravity(true, true, false);
	if (m_pGameInstance->Key_Pressing(DIK_K))
		m_pActorCom->Set_ZeroGravity(true, false, false);
	if(m_pGameInstance->Key_Pressing(DIK_L))
		m_pActorCom->Set_ZeroGravity(true, true, true);

#pragma region BasicActions
	/////////////////////////////////////////////
	KeyInput_Rail(dTimeDelta);

	if (false == m_bMoveToRail && false == m_bOnRail)
	{
		Wall_Jump(dTimeDelta);
		if (Trigger_Check(dTimeDelta))
		{
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
			Boss_Missile_Hit(dTimeDelta);
			Boss_Missile_Control(dTimeDelta);
			Falling_Dead(dTimeDelta);
			WallLaserTrap(dTimeDelta);
			PinBall(dTimeDelta);
		}
		else
		{
			// 트리거 끝나고 애니메이션 초기화
			m_IsFalling = m_pActorCom->Get_IsFalling();
			m_pActorCom->Set_GroundPound(m_bGroundPound);

			if ((m_bRoll == false || m_bSprint == true))
			{
				KeyInput(dTimeDelta);
			}
			if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false)
			{
				Sprint(dTimeDelta);
				if (m_IsSizeChanging == false)
					Move(dTimeDelta);
				if (m_eCurPlayerSize != SIZE_LARGE)
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

	Attack_BossMissile_After(dTimeDelta); // 미사일 공격이 끝나고 정상적인 회전갑승로 만들어주자

	m_pModelCom->Update_Animation(dTimeDelta);
	m_pEffect_Size->Update_Matrix(m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CCody::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	/* LateTick : 레일의 타겟 찾기*/
	Find_TargetSpaceRail();
	ShowRailTargetTriggerUI();
	Clear_TagerRailNodes();

	if (true == m_IsTouchFireDoor || true == m_IsWallLaserTrap_Touch || true == m_IsDeadLine)
		return NO_EVENT;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CCody::Render(RENDER_GROUP::Enum eGroup)
{
	if (true == m_IsDeadLine || m_IsPinBall)
		return S_OK;

	CCharacter::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

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

	//Safe_Release(m_pCamera);
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
	if (m_pGameInstance->Key_Down(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_F5))
		DATABASE->Set_GravityStageClear(true);
	if (m_pGameInstance->Key_Down(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_F6))
		DATABASE->Set_RailStageClear(true);
	if (m_pGameInstance->Key_Down(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_F7))
		DATABASE->Set_PinBallStageClear(true);

	if (m_pGameInstance->Key_Down(DIK_C)) /* 파이프 베터리 */
		m_pActorCom->Set_Position(XMVectorSet(44.8652f, 220.9396f, 223.94134f, 1.f));

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
	if (m_pGameInstance->Key_Down(DIK_6))/* 3층 */
		m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_7))/* Boss */
		m_pActorCom->Set_Position(XMVectorSet(62.f, 250.f, 187.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_8))/* Moon */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 760.f, 194.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_9))/* 우주선 내부 */
		m_pActorCom->Set_Position(XMVectorSet(63.f, 600.f, 1005.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_0))/* 우산 */
		m_pActorCom->Set_Position(XMVectorSet(-795.319824f, 766.982971f, 189.852661f, 1.f));
	if(m_pGameInstance->Key_Down(DIK_Z)) /* 파이프 베터리 */
		m_pActorCom->Set_Position(XMVectorSet(44.8652f, 220.9396f, 223.94134f, 1.f));
#pragma endregion

#pragma region 8Way_Move
	
	if (m_IsAirDash == false)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_D))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_A))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook - vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_D))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook + vCameraRight) / 2.f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_A))
		{
			bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook - vCameraRight) / 2.f);
		}
		else
		{
			if (m_eCurPlayerSize != SIZE_LARGE)
			{
				if (m_pGameInstance->Key_Pressing(DIK_A) && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
				{
					if (((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)*/ /*|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash*/) && m_IsTurnAround == false)
					{
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
					if (((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)*/ /*|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash*/) && m_IsTurnAround == false)
					{
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
					if (((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)*/ /*|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash*/) && m_IsTurnAround == false)
					{
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
					if (((m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)*/ || m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash) && m_IsTurnAround == false)
					{
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
		{
			m_IsTurnAround = false;
		}


		if (bMove[0] || bMove[1])
		{
			m_bMove = true;
		}
	}

#pragma endregion


#pragma region Keyboard_Shift_Button
	if (m_pGameInstance->Key_Down(DIK_LSHIFT) && m_bRoll == false && m_bCanMove == true && m_eCurPlayerSize != SIZE_LARGE)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());

			m_fAcceleration = 5.f;
			m_pModelCom->Set_Animation(ANI_C_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Roll_Stop);

			m_bAction = false;
			m_bRoll = true;
		}
		else
		{
			if (m_pModelCom->Get_CurAnimIndex() != ANI_C_AirDash_Start && m_iAirDashCount == 0)
			{
				CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());

				m_iAirDashCount += 1;
				m_fAcceleration = 5.f;
				m_pActorCom->Jump_Start(1.2f);
				m_pModelCom->Set_Animation(ANI_C_AirDash_Start);
				m_IsAirDash = true;
			}
		}
	}
#pragma endregion

#pragma region Keyboard_Space_Button
	if (m_eCurPlayerSize != SIZE_LARGE)
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 2)
		{
			m_bShortJump = true;
			m_iJumpCount += 1;
			m_IsJumping = true;
		}
	}
	else
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 1)
		{
			m_bShortJump = true;
			m_iJumpCount += 1;
			m_IsJumping = true;
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
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_eNextPlayerSize = SIZE_LARGE;
			m_IsSizeChanging = true;
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
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_eNextPlayerSize = SIZE_SMALL;
			m_IsSizeChanging = true;
			m_pActorCom->Set_Gravity(-7.5f);
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
	}

#pragma endregion

#pragma region Effet Test
	if (m_pGameInstance->Key_Down(DIK_O))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
	if (m_pGameInstance->Key_Down(DIK_I))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
#pragma  endregion
}

_uint CCody::Get_CurState() const
{
	if (nullptr == m_pModelCom) return 0;

	return m_pModelCom->Get_CurAnimIndex();
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
					m_pModelCom->Set_Animation(ANI_C_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Jog) == true) // Jog -> Jog // 보간속도 Up
				{
					m_pModelCom->Set_Animation(ANI_C_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd || m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Drumming || m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH)	// Idle To Jog Start. -> Jog 예약
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH_To_Idle || m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Stop_Fwd_Exhausted)
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Exhausted_MH_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
				{
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
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Start_Fwd) // JogStart 였다면
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH) // IDLE 상태라면
				{

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
						m_pModelCom->Set_Animation(ANI_C_Idle_To_Action);
						m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH);
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Idle_To_Action) == true && m_bAction == true)
				{
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
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
				// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
				if (m_pModelCom->Is_AnimFinished(ANI_C_ChangeSize_Walk_Large_Start) == true) // JogStart -> Jog
				{
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ChangeSize_Walk_Large_Fwd);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ChangeSize_Walk_Large_Fwd) == true) // Jog -> Jog // 보간속도 Up
				{
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Scratch
					|| m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Fwd
					|| m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Stop)	// Idle To Jog Start. -> Jog 예약
				{
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
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Stop); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_ChangeSize_Walk_Large_Start) // JogStart 였다면
				{
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
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
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

			m_pActorCom->Move(vDirection / m_fJogAcceleration / 4.f, dTimeDelta);

			if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false)
			{
				// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
				if (m_pModelCom->Is_AnimFinished(ANI_C_Jog_Start_Fwd) == true) // JogStart -> Jog
					m_pModelCom->Set_Animation(ANI_C_Jog);
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Jog) == true) // Jog -> Jog // 보간속도 Up
					m_pModelCom->Set_Animation(ANI_C_Jog);
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Drumming || m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH)	// Idle To Jog Start. -> Jog 예약
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH_To_Idle)
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Exhausted_MH_Start_Fwd);
					m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
				{
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
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // jog to stop 끝나면 idle 예약.
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Jog_Start_Fwd) // JogStart 였다면
				{
					m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd); // jog to stop 으로 바꿔
					m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH) // IDLE 상태라면
				{

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
						m_pModelCom->Set_Animation(ANI_C_Idle_To_Action);
						m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH);
					}
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_Idle_To_Action) == true && m_bAction == true)
				{
					m_pModelCom->Set_Animation(ANI_C_ActionMH);
					m_pModelCom->Set_NextAnimIndex(ANI_C_ActionMH_To_Idle);
				}
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ActionMH) == true && m_bAction == true)
				{
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
			m_pActorCom->Move(vDirection * (m_fAcceleration / 40.f), dTimeDelta);
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
			m_pActorCom->Move(vDirection * (m_fAcceleration / 40.f), dTimeDelta);
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
			m_pActorCom->Move(vDirection / m_fSprintAcceleration / 4.f, dTimeDelta);

		if (m_bRoll == false && m_IsJumping == false && m_IsTurnAround == false)
		{
			if (m_pModelCom->Is_AnimFinished(ANI_C_Sprint_Start_FromDash) == true) // JogStart -> Jog
				m_pModelCom->Set_Animation(ANI_C_Sprint);
			else if (m_pModelCom->Is_AnimFinished(ANI_C_Sprint) == true) // Jog -> Jog // 보간속도 Up
			{
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
				m_bSprint = false;
				m_pModelCom->Set_Animation(ANI_C_Jog_Stop_Fwd_Exhausted); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_C_Exhausted_MH); // jog to stop 끝나면 idle 예약.
			}
		}
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint_Start_FromDash) // JogStart 였다면
	{
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
	if (m_bShortJump == true)
	{
		if (m_iJumpCount == 1)
		{
			m_IsJumping = true;
			if (m_eCurPlayerSize == SIZE_MEDIUM)
				m_pActorCom->Jump_Start(2.6f);
			else if (m_eCurPlayerSize == SIZE_LARGE)
				m_pActorCom->Jump_Start(2.8f);
			else
				m_pActorCom->Jump_Start(1.5f);

			if (m_eCurPlayerSize == SIZE_LARGE)
				m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Start); // 사이즈 클때 점프 애니메이션이 다름.
			else
				m_pModelCom->Set_Animation(ANI_C_Jump_Start);

			m_bShortJump = false;
		}
		if (m_iJumpCount == 2)
		{
			m_IsJumping = true;
			//m_pActorCom->Jump_Higher(1.4f);
			//m_pActorCom->Set_Gravity(0.f);
			if (m_eCurPlayerSize == SIZE_MEDIUM)
				m_pActorCom->Jump_Start(2.6f);
			else if (m_eCurPlayerSize == SIZE_SMALL)
				m_pActorCom->Jump_Start(1.5f);

			m_pModelCom->Set_Animation(ANI_C_DoubleJump);
			m_bShortJump = false;
		}
	}
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false && m_bGroundPound == false)
	{
		m_iAirDashCount = 0;
		m_bSprint = false;
		// 착지할때 키를 누르고 있는 상태라면 Jog 로 연결
		if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		{
			if (m_eCurPlayerSize != SIZE_LARGE)
			{
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
				m_pModelCom->Set_Animation(ANI_C_Jump_Land);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
			else if (m_eCurPlayerSize == SIZE_LARGE  && m_bPlayGroundPoundOnce == false)
			{
				if (m_pModelCom->Get_CurAnimIndex() != ANI_C_ChangeSize_Jump_Falling)
				{
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
				m_pActorCom->Set_Scale(2.f, 2.f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_MIDDLE_LARGE);
				m_bChangeSizeEffectOnce = true;
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
				m_pActorCom->Set_Scale(0.5f, 0.5f);
				m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_LARGE_MIDDLE);
				m_bChangeSizeEffectOnce = true;
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
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_MEDIUM;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
				m_pModelCom->Set_Animation(ANI_C_MH);
			}
		}
		else if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_SMALL)
		{
			m_pActorCom->Get_Controller()->setSlopeLimit(0.02f);
			m_pActorCom->Get_Controller()->setStepOffset(0.02f);

			if (m_bChangeSizeEffectOnce == false)
			{
				m_pActorCom->Set_Scale(0.05f, 0.05f);
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
				m_vScale = { 0.2f, 0.2f, 0.2f };
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
				m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Falling);
			else
				m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_GroundPound_Falling);
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
				m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Start);
			m_pActorCom->Set_Jump(false);
			m_pActorCom->Set_Gravity(0.f);
			m_fGroundPoundAirDelay += (_float)dTimeDelta;
		}
	}

	if (m_eCurPlayerSize != SIZE_LARGE)
	{
		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_GroundPound_Falling) && m_bPlayGroundPoundOnce == false)
		{
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

		if (m_iAfterGroundPoundCount >= 90) // 1.5초 경직
		{
			m_iAfterGroundPoundCount = 0;
			m_bAfterGroundPound = false;
			m_bCanMove = true;
		}
	}

}

void CCody::Attack_BossMissile_After(_double dTimeDelta)
{
	if (true == m_IsBossMissile_RotateYawRoll_After)
	{
		m_fBossMissile_HeroLanding_Time += (_float)dTimeDelta;

		if (2.f >= m_fBossMissile_HeroLanding_Time)
		{
		// 개꿀잼 회전값 보정을 원한다면
		//	_float fRotateRoll_Check = m_pTransformCom->Get_State(CTransform::STATE_RIGHT).m128_f32[1];
		//	m_pTransformCom->RotateRoll(dTimeDelta * fRotateRoll_Check * -1.f);
		//	fRotateRoll_Check = m_pTransformCom->Get_State(CTransform::STATE_LOOK).m128_f32[1];
		//	m_pTransformCom->RotatePitch(dTimeDelta * fRotateRoll_Check * -1.f);

		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		vLook.m128_f32[1] = 0.f;
		vRight.m128_f32[1] = 0.f;

		vLook = XMVector3Normalize(vLook) * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
		vRight = XMVector3Normalize(vRight) * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

		_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * m_pTransformCom->Get_Scale(CTransform::STATE_UP);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
		}
		else
		{
			m_IsBossMissile_RotateYawRoll_After = false;
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

	m_bAction = false;

	m_IsJumping = false;
	m_IsAirDash = false;
	m_IsFalling = false;
	m_bFallAniOnce = false;

	m_bPlayGroundPoundOnce = false;

	m_fIdleTime = 0.f;

	m_iJumpCount = 0;
	m_iAirDashCount = 0;

	m_pActorCom->Set_Jump(false);
	m_pModelCom->Set_Animation(ANI_C_MH);
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

_bool CCody::Trigger_Check(const _double dTimeDelta)
{
	if (m_IsCollide == true)
	{
		if (m_eTargetGameID == GameID::eSTARBUDDY && m_pGameInstance->Key_Down(DIK_F))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_PlanetPush_Large);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsHitStarBuddy = true;
		}
		else if (m_eTargetGameID == GameID::eMOONBABOON && m_pGameInstance->Key_Down(DIK_F))
		{
			m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Grapple_ToGrind);
			m_IsOnGrind = true;
		}
		else if (m_eTargetGameID == GameID::eROCKET && m_pGameInstance->Key_Down(DIK_F))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_RocketFirework);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsHitRocket = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTLEVER && m_pGameInstance->Key_Down(DIK_F))
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
				m_pActorCom->Set_Position(XMVectorSet(71.1877518f, 23.28266802f, 179.620789f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			}
			else if (DATABASE->Get_Cody_Stage() == ST_PINBALL)
			{
				m_pActorCom->Set_Position(XMVectorSet(-814.433655f, 791.810059f, 228.490845f, 1.f));
				m_pTransformCom->Set_RotateAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			}
			else if (DATABASE->Get_Cody_Stage() == ST_RAIL)
			{
				m_pActorCom->Set_Position(XMVectorSet(1035.42493f, 743.288574f, 216.862808f, 1.f));
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
		}
		else if (m_eTargetGameID == GameID::eSPACEVALVE && m_pGameInstance->Key_Down(DIK_F) && m_iValvePlayerName == Player::Cody)
		{
			m_pActorCom->Set_Position(XMVectorSet(46.487f, 125.842f, 195.789f, 1.f));
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_MH);
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
		else if (m_eTargetGameID == GameID::ePLANET && m_pGameInstance->Key_Down(DIK_F))
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
			}
		}
		else if (m_eTargetGameID == GameID::eHOOKUFO && m_pGameInstance->Key_Down(DIK_F) && m_IsHookUFO == false)
		{
			// 최초 1회 OffSet 조정
			if (m_IsHookUFO == false)
			{
				m_vTriggerTargetPos.y = m_vTriggerTargetPos.y - 5.f;
				_vector vPlayerPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
				_vector vTriggerPos = XMVectorSetY(XMLoadFloat3(&m_vTriggerTargetPos), 0.f);
				_vector vPlayerToTrigger = XMVector3Normalize(vTriggerPos - vPlayerPos);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vRight = XMVector3Cross(vPlayerToTrigger, vUp);
				m_vHookUFOAxis = vRight;


				//m_pTransformCom->Rotate_ToTarget(XMLoadFloat3(&m_vTriggerTargetPos));
				_vector vTestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector vTargetPos = XMLoadFloat3(&m_vTriggerTargetPos);

				_vector vDir = vTargetPos - vTestPos;
				_float  fDist = XMVectorGetX(XMVector3Length(vDir));

				_vector vFixUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);/*m_vHookUFOAxis*/;
				_vector vTriggerToPlayer = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&m_vTriggerTargetPos));
				m_fRopeAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vFixUp, vTriggerToPlayer));
				m_faArmLength = fDist;

				//_vector vPosition = XMVectorSet(XMVectorGetX(vTestPos), m_faArmLength * cos(m_fRopeAngle), m_faArmLength * sin(m_fRopeAngle), 1.f) + XMVectorSetW(XMLoadFloat3(&m_vTriggerTargetPos), 1.f);
				XMStoreFloat3(&m_vStartPosition, XMVectorSet(XMVectorGetX(vTestPos), XMVectorGetY(vTestPos), XMVectorGetZ(vTestPos), 1.f)/* + (XMLoadFloat3(&m_vTriggerTargetPos)*/);

			}
			m_pModelCom->Set_Animation(ANI_C_Bhv_Swinging_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Swinging_Fwd);
			m_IsHookUFO = true;
			m_bGoToHooker = true;
			m_pActorCom->Set_ZeroGravity(true, false, true);
		}
		else if (GameID::eWARPGATE == m_eTargetGameID && false == m_IsWarpNextStage)
		{
			// 코디 전용 포탈로 이동(웜홀)
			
			m_pModelCom->Set_Animation(ANI_C_SpacePortal_Travel);
			m_pModelCom->Set_NextAnimIndex(ANI_C_SpacePortal_Travel);

			m_pActorCom->Set_Position(XMLoadFloat4(&m_vWormholePos));
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_fWarpTimer = 0.f;
			m_IsWarpNextStage	= true;
			m_IsWarpDone		= true;
		}
		else if (GameID::eFIREDOOR == m_eTargetGameID && false == m_IsTouchFireDoor)
		{
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pActorCom->Set_ZeroGravity(true, false, true);
			Enforce_IdleState();
			m_IsTouchFireDoor = true;
		}
		else if (GameID::eBOSSMISSILE_COMBAT == m_eTargetGameID && false == m_IsBossMissile_Hit)
		{
			m_IsBossMissile_Hit = true;
		}
		else if (GameID::eBOSSMISSILE_PLAYABLE == m_eTargetGameID && false == m_IsBossMissile_Control)
		{
			m_IsBossMissile_Control = true;
			m_IsBossMissile_Rodeo = false;
			m_IsBossMissile_Rodeo_Ready = false;
		}
		else if (m_eTargetGameID == GameID::eDUMMYWALL && m_bWallAttach == false && m_fWallJumpingTime <= 0.f)
		{
			m_pModelCom->Set_Animation(ANI_C_WallSlide_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_C_WallSlide_MH);
			m_bWallAttach = true;
		}
		else if (m_eTargetGameID == GameID::eDEADLINE && false == m_IsDeadLine)
		{
			Enforce_IdleState();
			/* 데드라인 */
			m_pModelCom->Set_Animation(ANI_C_Bhv_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Death_Fall_MH);

			m_pActorCom->Set_ZeroGravity(true, false, true);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_IsDeadLine = true;
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
		else if (m_eTargetGameID == GameID::eSAVEPOINT)
		{
			/* 세이브포인트->트리거와 충돌시 세이브포인트 갱신 */
			m_vSavePoint = m_vTriggerTargetPos;
		}
		else if (m_eTargetGameID == GameID::eUMBRELLABEAMJOYSTICK && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_M_ArcadeScreenLever_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_M_ArcadeScreenLever_MH);
			m_IsControlJoystick = true;
			CUmbrellaBeam_Joystick* pJoystick = (CUmbrellaBeam_Joystick*)DATABASE->Get_Umbrella_JoystickPtr();
			pJoystick->Set_ControlActivate(true);
		}
		else if (m_eTargetGameID == GameID::eWALLLASERTRAP && false == m_IsWallLaserTrap_Touch)
		{
			m_pModelCom->Set_Animation(ANI_M_Death_Fall_MH);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			Enforce_IdleState();
			m_pActorCom->Set_ZeroGravity(true, false, true);
			m_IsWallLaserTrap_Touch = true;
		}
		else if (m_eTargetGameID == GameID::ePINBALL && false == m_IsPinBall)
		{
			/* 핀볼모드 ON */
			m_pActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
			m_IsPinBall = true;
		}
		else if (m_eTargetGameID == GameID::ePINBALLDOOR && m_pGameInstance->Key_Down(DIK_F))
		{
			/* 핀볼 문열기 */
			((CPinBall_Door*)(CDataStorage::GetInstance()->Get_Pinball_Door()))->Set_DoorState(false);
		}
		else if (m_eTargetGameID == GameID::eHOOKAHTUBE)
		{
			/* 튜브*/
			m_pActorCom->Jump_Start(4.f);

			_uint iRandom = rand() % 4;
			switch (iRandom)
			{
			case 0:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_01);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Bounce_01);
				break;
			case 1:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_02);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Bounce_02);
				break;
			case 2:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_03);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Bounce_03);
				break;
			case 3:
				m_pModelCom->Set_Animation(ANI_C_Bhv_Bounce_04);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Bounce_04);
				break;
			default:
				break;
			}

			m_IsCollide = false;
		}
	}

	// Trigger 여따가 싹다모아~
	if (m_IsHitStarBuddy || m_IsHitRocket || m_IsActivateRobotLever || m_IsPushingBattery || m_IsEnterValve || m_IsInGravityPipe
		|| m_IsHitPlanet || m_IsHookUFO || m_IsWarpNextStage || m_IsWarpDone || m_IsTouchFireDoor || m_IsBossMissile_Hit || m_IsBossMissile_Control || m_IsDeadLine 
		|| m_bWallAttach || m_IsControlJoystick || m_IsPinBall || m_IsWallLaserTrap_Touch)
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
	// 애니메이션 시작할때 WorldPos 저장. -> 끝나는 순간 마지막 위치로 WorldPos 변경 해야 함.
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
				m_pModelCom->Set_Animation(ANI_C_ActionMH);
				m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Push_Battery_Fwd);
			}

			return;
		}

		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_Push_Battery_MH))
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_MH);
	}
	if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_GRAVITYPATH)
	{
		m_IsPushingBattery = false;
		m_IsCollide = false;
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_IsStGravityCleared = true;
	}

	else if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_RAIL)
	{
		m_IsPushingBattery = false;
		m_IsCollide = false;
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_IsStRailCleared = true;
	}

	else if (m_IsPushingBattery == true && DATABASE->Get_Cody_Stage() == ST_PINBALL)
	{
		m_IsPushingBattery = false;
		m_IsCollide = false;
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_IsStPinBallCleared = true;
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
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_MH);
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
			((CHangingPlanet*)(m_pTargetPtr))->Hit_Planet(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_ChangeSize_PlanetPush_Large))
		{
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_IsHitPlanet = false;
			m_IsCollide = false;
		}
	}
}

void CCody::Hook_UFO(const _double dTimeDelta)
{
	if (m_IsHookUFO == true)
	{
		_float Gravity = -0.3f;

		// ZY
		m_faAcceleration = (-1.f * Gravity / m_faArmLength) * sin(m_fRopeAngle);
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_faAcceleration += (_float)dTimeDelta;
		if (m_pGameInstance->Key_Pressing(DIK_S))
			m_faAcceleration -= (_float)dTimeDelta;
		m_faVelocity += m_faAcceleration;
		m_faVelocity *= m_faDamping;
		m_fRopeAngle += m_faVelocity / 15.f;
		

		_vector vPosition = XMVectorSet((m_vTriggerTargetPos.x-m_vStartPosition.x )/**2.f*/ * sin(-m_fRopeAngle), 
			/*m_faArmLength **/(m_vTriggerTargetPos.y - m_vStartPosition.y) *2.f* cos(m_fRopeAngle)
			, (/*m_faArmLength*/(m_vTriggerTargetPos.z - m_vStartPosition.z)/**2.f*/ * sin(-m_fRopeAngle)), 0.f)/* + XMLoadFloat3(&m_vTriggerTargetPos)*/;
		m_pActorCom->Set_Position(XMLoadFloat3(&m_vTriggerTargetPos) + vPosition);

		_vector vTriggerToPlayer = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION),0.f) - XMVectorSetY(XMLoadFloat3(&m_vTriggerTargetPos), 0.f));
		vTriggerToPlayer = XMVectorSetW(vTriggerToPlayer, 1.f);
		m_pTransformCom->RotateYawDirectionOnLand(-vTriggerToPlayer, (_float)dTimeDelta / 2.f);
		//m_pTransformCom->Set_RotateAxis(m_vHookUFOAxis, sin(-m_fRopeAngle));

		////////////////////////////////////////


		if (m_pGameInstance->Key_Down(DIK_SPACE)) // 로프 놓기
		{
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
		}
	}
}

void CCody::Wall_Jump(const _double dTimeDelta)
{
	if (true == m_bWallAttach && false == m_IsWallJumping)
	{
		m_pActorCom->Move((-m_pTransformCom->Get_State(CTransform::STATE_UP) / 50.f), dTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
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

void CCody::Warp_Wormhole(const _double dTimeDelta)
{
	if (false == m_IsWarpNextStage && false == m_IsWarpDone)
		return;

	_float4 vWormhole = m_vWormholePos;
	vWormhole.z -= 1.f;
	m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat4(&vWormhole));

	m_fWarpTimer += (_float)dTimeDelta;

	if (true == m_IsWarpNextStage)
	{
		if (m_fWarpTimer_Max <= m_fWarpTimer)
		{
			m_pModelCom->Set_Animation(ANI_C_SpacePortal_Travel);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsWarpNextStage = false;
			
			_vector vNextStage_Pos = XMLoadFloat3(&m_vTriggerTargetPos);
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
		if (m_fWarpTimer_Max + 0.25f >= m_fWarpTimer)
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
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		m_fDeadTime = 0.f;
		m_bCanMove = true;
		m_IsCollide = false;
		m_IsTouchFireDoor = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);
	}
}

void CCody::Boss_Missile_Hit(const _double dTimeDelta)
{
	if (false == m_IsBossMissile_Hit)
		return;

	m_IsBossMissile_Hit = false;
	m_IsCollide = false;
	// 아악 아프다
}

void CCody::Boss_Missile_Control(const _double dTimeDelta)
{
	if (false == m_IsBossMissile_Control)
		return;

	if (false == m_IsBossMissile_Rodeo)
	{
		if (false == m_IsBossMissile_Rodeo_Ready)
		{
			if (m_pGameInstance->Key_Down(DIK_F)) // 탑승 하세요
			{
				m_IsBossMissile_Rodeo_Ready = true;
				m_IsBoss_Missile_Explosion = false;
			}
		}

		if (true == m_IsBossMissile_Rodeo_Ready) // 탑승
		{
			m_fBossMissile_HeroLanding_Time = 0.f;
			m_pActorCom->Set_ZeroGravity(true, false, false);

			_matrix TriggerMatrix = XMLoadFloat4x4(&m_TriggerTargetWorld);
			for (_int i = 0; i < 3; ++i)
				TriggerMatrix.r[i] = XMVector3Normalize(TriggerMatrix.r[i]);

			// 회전
			_vector vRotatePos = TriggerMatrix.r[3] + (TriggerMatrix.r[1] * 10.f);
			m_pTransformCom->Rotate_ToTarget(vRotatePos);

			// 이동
			TriggerMatrix.r[3] += TriggerMatrix.r[1] * 1.f;
			_vector vNewLook = XMVector3Cross(TriggerMatrix.r[1], XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), TriggerMatrix.r[1]));
			TriggerMatrix.r[3] += vNewLook * 0.8f;
			//_vector vDir = XMVector3Normalize(TriggerMatrix.r[3] - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pActorCom->Set_Position(TriggerMatrix.r[3]);

			//m_IsCollide = false;
			m_IsBossMissile_Rodeo = true;
			m_fLandTime = 0.f;
			//m_IsBossMissile_Control = false;

		}
	}
	else if (true == m_IsBossMissile_Rodeo && false == m_IsBoss_Missile_Explosion)
	{
		m_fLandTime += (_float)dTimeDelta;
		if(0.25f >= m_fLandTime)
			m_pTransformCom->RotatePitch(dTimeDelta * -0.25);

		else
		{
			m_fLandTime = 0.25f;

			if (m_pGameInstance->Key_Down(DIK_1)) /* 스타트 지점 */
				m_pActorCom->Set_Position(XMVectorSet(60.f, 0.f, 15.f, 1.f));

			if (m_pGameInstance->Key_Pressing(DIK_W)) // 아래로 밀어
				m_pTransformCom->RotatePitch(dTimeDelta * 0.7);
			else if (m_pGameInstance->Key_Pressing(DIK_S)) // 위로 들어
				m_pTransformCom->RotatePitch(dTimeDelta * -0.7);

			if (m_pGameInstance->Key_Pressing(DIK_A)) // 좌
				m_pTransformCom->RotateYaw(dTimeDelta * -1);
			else if (m_pGameInstance->Key_Pressing(DIK_D)) // 우
				m_pTransformCom->RotateYaw(dTimeDelta * 1);

			_vector vUp		= m_pTransformCom->Get_State(CTransform::STATE_UP);
			_vector vAxisY	= XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_float vRadian = XMVector3Dot(vUp, vAxisY).m128_f32[0];
			
			_float fRotateRoll_Check = m_pTransformCom->Get_State(CTransform::STATE_RIGHT).m128_f32[1];
			m_pTransformCom->RotateRoll(dTimeDelta * fRotateRoll_Check * -1.f);

			_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			m_pActorCom->Move(vDir * 0.2f , dTimeDelta);

		}
	}
	else if (true == m_IsBoss_Missile_Explosion)
	{
		m_pActorCom->Set_ZeroGravity(false, false, false);
		
		m_IsBossMissile_Control = false;
		m_IsCollide = false;
		m_IsBossMissile_RotateYawRoll_After = true;
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

void CCody::WallLaserTrap(const _double dTimeDelta)
{
	if (false == m_IsWallLaserTrap_Touch)
		return;

	m_IsWallLaserTrap_Effect = true;
	m_fDeadTime += (_float)dTimeDelta;
	if (m_fDeadTime >= 2.f)
	{
		_float fMyPosY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		_float fTriggerY = m_vTriggerTargetPos.y;

		_vector vRespawnPos = XMVectorSet(-803.f, 768.f, 193.f, 1.f);
		if(fMyPosY > fTriggerY)
			vRespawnPos = XMVectorSet(-805.f, 792.f, 198.f, 1.f);

		m_pActorCom->Set_Position(vRespawnPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vRespawnPos);
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
		m_pModelCom->Set_Animation(ANI_C_MH);
		m_fDeadTime = 0.f;
		m_IsCollide = false;
		m_IsWallLaserTrap_Touch = false;
		m_IsWallLaserTrap_Effect = false;
		m_pActorCom->Set_ZeroGravity(false, false, false);
	}
}

void CCody::Set_BossMissile_Attack()
{
	m_IsBoss_Missile_Explosion = true;
}

void CCody::Falling_Dead(const _double dTimeDelta)
{
	/* 데드라인과 충돌시 2초후에 리스폰 */
	if (m_IsDeadLine == true)
	{
		m_fDeadTime += (_float)dTimeDelta;
		if (m_fDeadTime >= 2.f)
		{
			_vector vSavePosition = XMLoadFloat3(&m_vSavePoint);
			vSavePosition = XMVectorSetW(vSavePosition, 1.f);

			m_pActorCom->Set_Position(vSavePosition);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSavePosition);
			CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
			m_pModelCom->Set_Animation(ANI_C_MH);
			m_fDeadTime = 0.f;
			m_IsCollide = false;
			m_IsDeadLine = false;
			m_pActorCom->Set_ZeroGravity(false, false, false);
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
	if (m_pGameInstance->Key_Down(DIK_F) && false == m_bOnRail)
	{
		Start_SpaceRail();
	}

	if (m_bOnRail)
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));

			m_iJumpCount = 1;
			m_bShortJump = true;

			m_pTargetRail = nullptr;
			m_pSearchTargetRailNode = nullptr;
			m_pTargetRailNode = nullptr;

			m_bMoveToRail = false;
			m_bOnRail = false;
			Clear_TagerRailNodes();
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
	if (m_vecTargetRailNodes.empty()) return;

	CTransform* pCamTransform = m_pCamera->Get_Transform();
	if (nullptr == pCamTransform) return;
	_vector vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);
	_vector vCamUp = pCamTransform->Get_State(CTransform::STATE_UP);

	// 거리가 아닌, 카메라 Look 벡터와의 각도차로 계산
	vCamLook = XMVector3Normalize(vCamLook);
	vCamUp = XMVector3Normalize(vCamUp);

	_bool isSearch = false;
	_vector vToTarget = XMVectorZero();
	_vector vNodePosition = XMVectorZero();
	_float fMinDegree = 360.f;
	for (auto& pNode : m_vecTargetRailNodes)
	{
		if (nullptr == pNode) continue;
		
		vNodePosition = pNode->Get_Position();
		vToTarget = XMVector3Normalize(vNodePosition - vCamPos);
		
		/* 외적 : 방향 체크 */
		_bool bEdge = false;
		_uint iEdgeState = pNode->Get_EdgeState();
		_int iCCW = MH_CrossCCW(vCamLook, vToTarget, vCamUp); /* @Return CCW(1) CW(-1) Else(0) */
		switch (iEdgeState)
		{
		case CSpaceRail::EDGE_START:  // Edge Start 트리거노드가 존재하면, MID 노드는 체크하지 않는다.
			if (1 == iCCW) continue; // 반시계방향에 존재하는 노드 건너뛰기
			bEdge = true;
		break;
		case CSpaceRail::EDGE_END:
			if (-1 == iCCW) continue; // 시계 방향에 존재하는 노드 건너뛰기
			bEdge = true;
		break;
		}

		/* 각도 계산  */
		_float fDegree = XMConvertToDegrees(XMVectorGetX(XMVector3Dot(vCamLook, vToTarget)));
		if (fDegree > 60.f) continue; /* 일정각도(90) 이상인 노드는 제외한다. */

		/* 가장 각도가 적은 타겟노드 찾기 */
		if (fMinDegree > fDegree)
		{
			m_pSearchTargetRailNode = pNode;
			//m_pTargetRailNode = pNode;
			fMinDegree = fDegree;
			isSearch = true;

			if (bEdge) 
				break;
		}
	}

	if (false == isSearch)
		m_pSearchTargetRailNode = nullptr;
}
void CCody::Start_SpaceRail()
{
	if (nullptr == m_pSearchTargetRailNode) return;

	if (m_pSearchTargetRailNode) {
		// 타겟을 찾았다면, 레일 탈 준비
		m_pTargetRailNode = m_pSearchTargetRailNode;
		m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_Enter); // 줄던지고 댕겨서 날라가기
		m_bMoveToRail = true;
	}
}
void CCody::MoveToTargetRail(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom || false == m_bMoveToRail || nullptr == m_pTargetRailNode || true == m_bOnRail) return;

	_float fMoveToSpeed = 10.f;
	_float fDist = m_pTransformCom->Move_ToTargetRange(m_pTargetRailNode->Get_Position(), 0.1f, dTimeDelta * fMoveToSpeed);
	if (fDist < 0.2f)
	{
		/* 타는 애니메이션으로 변경 */
		m_pModelCom->Set_Animation(ANI_C_Grind_Grapple_ToGrind); // 레일 착지
		m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Slow_MH);

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

		/* 패스 지정 -> X */
		CPath::STATE ePathState = CPath::STATE_END;
		//if(m_pGameInstance->Key_Down(DIK_W))
		//	ePathState = CPath::STATE_FORWARD;
		//else if(m_pGameInstance->Key_Down(DIK_S))
		//	ePathState = CPath::STATE_BACKWARD;

		/* 외적으로 방향 구하기 */
		CTransform* pCamTransform = m_pCamera->Get_Transform();
		if (nullptr == pCamTransform) return;
		_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);
		_vector vCamUp = pCamTransform->Get_State(CTransform::STATE_UP);
		_vector vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);

		_vector vToTarget = m_pTargetRailNode->Get_Position() - vCamPos;
		_int iCCW = MH_CrossCCW(vCamLook, vToTarget, vCamUp);  /* @Return CCW(1) CW(-1) Else(0) */

		switch (iCCW)
		{
		case 1:		// 반시계
		{
			ePathState = CPath::STATE_BACKWARD;
			break;
		}
		case -1:	// 시계
			ePathState = CPath::STATE_FORWARD;
			break;
		case 0:		// 일직선
			ePathState = CPath::STATE_FORWARD;
			break;
		}

		/* Edge State 지정 */
		// START / END 일경우, 방향 전환
		_uint iEdgeState = m_pTargetRailNode->Get_EdgeState();
		switch (iEdgeState)
		{
		case CSpaceRail::EDGE_START:
			if(CPath::STATE_END == ePathState)
				ePathState = CPath::STATE_FORWARD;
			break;
		case CSpaceRail::EDGE_END:
			if (CPath::STATE_FORWARD == ePathState)
				ePathState = CPath::STATE_BACKWARD;
			else if (CPath::STATE_BACKWARD == ePathState)
				ePathState = CPath::STATE_FORWARD;
			else if (CPath::STATE_END == ePathState)
				ePathState = CPath::STATE_BACKWARD;
			break;
		case CSpaceRail::EDGE_MID:
		{
			CTransform* pCamTransform = m_pCamera->Get_Transform();
			if (nullptr == pCamTransform) return;
			_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);
			_vector vCamUp = pCamTransform->Get_State(CTransform::STATE_UP);
			_vector vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);

			_vector vToTarget = m_pTargetRailNode->Get_Position() - vCamPos;
			_int iCCW = MH_CrossCCW(vCamLook, vToTarget, vCamUp);  /* @Return CCW(1) CW(-1) Else(0) */

			switch (iCCW)
			{
			case 1:		// 반시계
			{
				if (CPath::STATE_FORWARD == ePathState)
					ePathState = CPath::STATE_BACKWARD;
				else if (CPath::STATE_BACKWARD == ePathState)
					ePathState = CPath::STATE_FORWARD;
				else if (CPath::STATE_END == ePathState)
					ePathState = CPath::STATE_BACKWARD;
				break;
			}
			case -1:	// 시계
				if (CPath::STATE_END == ePathState)
					ePathState = CPath::STATE_FORWARD;
				break;
			case 0:		// 일직선
				if (CPath::STATE_END == ePathState)
					ePathState = CPath::STATE_FORWARD;
				break;
			}
		}
			break;
		}

		m_pTargetRail->Start_Path(ePathState, m_pTargetRailNode->Get_FrameIndex());

		/* 레일 앞까지 도착, 레일 타기 시작 */
		m_pTargetRailNode = nullptr;
		m_bOnRail = true;
		m_bMoveToRail = false;
	}
}
void CCody::TakeRail(_double dTimeDelta)
{
	if (nullptr == m_pTargetRail || false == m_bOnRail) return;

	/* 타는 애니메이션으로 변경 */
	m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Slow_MH);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_bOnRail = m_pTargetRail->Take_Path(dTimeDelta, WorldMatrix);
	if (m_bOnRail)
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	else
	{
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH); // 자유낙하 애니메이션으로 변경해야함.
		m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
	}
}
void CCody::ShowRailTargetTriggerUI()
{
	// Show UI
	if (m_pSearchTargetRailNode && nullptr == m_pTargetRailNode)
	{
		UI_Generator->Set_Active(Player::Cody, UI::InputButton_InterActive, true);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pSearchTargetRailNode->Get_Position());
	}
	else
		UI_Generator->Set_Active(Player::Cody, UI::InputButton_InterActive, false);
}
#pragma endregion

void CCody::PinBall(const _double dTimeDelta)
{
	if (true == m_IsPinBall)
		m_pActorCom->Set_Position(((CDynamic_Env*)(CDataStorage::GetInstance()->Get_Pinball()))->Get_Position());
}

void CCody::PinBall_Respawn(_double dTimeDelta)
{
	m_pActorCom->Set_Position(XMVectorSet(-650.f, 760.f, 195.f, 1.f));
	m_pActorCom->Update(dTimeDelta);

	CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
	m_pActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	m_pModelCom->Set_Animation(ANI_C_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_C_MH);

	m_IsPinBall = false;
	m_IsCollide = false;
}
