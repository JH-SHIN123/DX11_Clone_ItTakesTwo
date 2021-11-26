#include "stdafx.h"
#include "..\public\Cody.h"
#include "MainCamera.h"
#include "UI_Generator.h"
#include "UIObject.h"
#include "May.h"
#include"DataStorage.h"
#include "MathHelper.h"

#include "Effect_Generator.h"
#include "Effect_Cody_Size.h"

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
 	UI_Create(Default, LoadingBook);
 	UI_Create(May, Arrowkeys_Side);
 	UI_Create(May, StickIcon);
 
 	UI_Create(Cody, PlayerMarker);
 
 	//UI_Create(Cody, InputButton_InterActive);
	 
	m_vPoints[0] = {72.f, 0.f, -120.f };
	m_vPoints[1] = { 0.f, 0.f, 0.f };
	m_vPoints[2] = {42.f, 0.f, 60};
	m_vPoints[3] = {84.f, 0.f, 20.f};
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
	
	CControllableActor::ARG_DESC ArgDesc;

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);

	//Effect 
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_Cody_Size"), nullptr, (CGameObject**)&m_pEffect_Size), E_FAIL);
	m_pEffect_Size->Set_Model(m_pModelCom);
	return S_OK;
}

void CCody::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Jump_Start, false);
	//m_pModelCom->Add_LerpInfo(1, 2, true, 8.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_Sprint, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_MH, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Walk_Large_Fwd, ANI_C_ChangeSize_Walk_Large_Fwd, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_Enter, ANI_C_Grind_Grapple_ToGrind, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_ToGrind, ANI_C_Grind_Slow_MH, false);

	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_Stop, true);
	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Stop, ANI_C_Roll_To_Jog, true, 30.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_To_Jog, true, 30.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_DoubleJump, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_AirDash_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_DoubleJump, false);
	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Bhv_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Bhv_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_SprintTurnAround, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_SprintTurnAround, true, 20.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd_Exhausted, ANI_C_SprintTurnAround, true, 20.f);



	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jump_Land_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_MH, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_180R, true, 2.f);

	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Start_Fwd, ANI_C_Jump_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_Jump_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jog, ANI_C_Jump_Falling, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Falling, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land, false);
	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land_Jog, false);
	//ANI_C_Roll_Start, ANI_C_Roll_Stop;
	return;
}

#pragma endregion

#pragma region Overrided Function
_int CCody::Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);

	m_pCamera = (CMainCamera*)CDataStorage::GetInstance()->Get_MainCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

#pragma region BasicActions
	/////////////////////////////////////////////
	if (Trigger_Check(dTimeDelta))
	{
		Go_Grind(dTimeDelta);
		Hit_StarBuddy(dTimeDelta);
		Hit_Rocket(dTimeDelta);
		Activate_RobotLever(dTimeDelta);
		Push_Battery(dTimeDelta);
		Rotate_Valve(dTimeDelta);
		In_GravityPipe(dTimeDelta);
		Hit_Planet(dTimeDelta);
		Hook_UFO(dTimeDelta);
	}
	else
	{
		// 트리거 끝나고 애니메이션 초기화
		Trigger_End(dTimeDelta);
		m_IsFalling = m_pActorCom->Get_IsFalling();
		m_pActorCom->Set_GroundPound(m_bGroundPound);

		if ((m_bRoll == false || m_bSprint == true))
			KeyInput(dTimeDelta);
		if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false)
		{
			Sprint(dTimeDelta);
			Move(dTimeDelta);
			if (m_eCurPlayerSize != SIZE_LARGE)
				Roll(dTimeDelta);
			Jump(dTimeDelta);
			Change_Size(dTimeDelta);
		}
		Ground_Pound(dTimeDelta);
	}
	/////////////////////////////////////////////

	
#pragma endregion

	CMay* pMay = (CMay*)DATABASE->GetMay();
	UI_Generator->Set_TargetPos(Player::Cody, UI::PlayerMarker, pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	//테스트////////////////////////////////////////////////
	if(m_pGameInstance->Key_Down(DIK_3))
		UI_Create(Cody, RespawnCircle);

	if (m_pGameInstance->Key_Down(DIK_4))
		UI_Delete(Cody, RespawnCircle);
	///////////////////////////////////////////////////////

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);
	m_pEffect_Size->Update_Matrix(m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CCody::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CCody::Render(RENDER_GROUP::Enum eGroup)
{
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
	//Safe_Release(m_pCamera);
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffect_Size);
	CCharacter::Free();
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCody::KeyInput(_double dTimeDelta)
{
#pragma region Local variable
	_vector vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;
#pragma endregion

#pragma region Teleport
	if (m_pGameInstance->Key_Down(DIK_1)) /* 스타트 지점 */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 0.f, 15.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_2)) /* 2층 */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 125.f, 170.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_3)) /* 2스테이지 입구 */
		m_pActorCom->Set_Position(XMVectorSet(620.f, 760.f, 195.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_4)) /* 2스테이지 */
		m_pActorCom->Set_Position(XMVectorSet(960.f, 720.f, 193.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_5))/* 3스테이지 */
		m_pActorCom->Set_Position(XMVectorSet(-610.f, 760.f, 195.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_6))/* 3층 */
		m_pActorCom->Set_Position(XMVectorSet(70.f, 220.f, 207.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_7))/* Boss */
		m_pActorCom->Set_Position(XMVectorSet(62.f, 250.f, 187.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_8))/* Moon */
		m_pActorCom->Set_Position(XMVectorSet(60.f, 760.f, 194.f, 1.f));
	if (m_pGameInstance->Key_Down(DIK_9))/* 우주선 내부 */
		m_pActorCom->Set_Position(XMVectorSet(63.f, 600.f, 1005.f, 1.f));
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

		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Dash, m_pTransformCom->Get_WorldMatrix());

		if (m_IsJumping == false)
		{
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
		}
	}
	else
	{
		if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 1)
		{
			m_bShortJump = true;
			m_iJumpCount += 1;
		}
	}

	
#pragma endregion

#pragma region Mouse_LButton

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
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

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_RB))
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
	}

#pragma endregion

#pragma region Effet Test
	if (m_pGameInstance->Key_Down(DIK_O))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
	if (m_pGameInstance->Key_Down(DIK_I))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Revive, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
#pragma  endregion
}
void CCody::Move(const _double dTimeDelta)
{
#pragma region Medium_Size
	if (m_eCurPlayerSize == SIZE_MEDIUM)
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
					m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Fwd);
				else if (m_pModelCom->Is_AnimFinished(ANI_C_ChangeSize_Walk_Large_Fwd) == true) // Jog -> Jog // 보간속도 Up
				{
					//m_pModelCom->Set_Animation(ANI_C_ChangeSize_Walk_Large_Fwd);
				}
				else if (m_pModelCom->Get_CurAnimIndex() == ANI_C_MH || m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_MH_Gesture_Small_Scratch || m_pModelCom->Get_CurAnimIndex() == ANI_C_ActionMH)	// Idle To Jog Start. -> Jog 예약
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
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * m_fAcceleration);
		if (m_eCurPlayerSize == SIZE_MEDIUM)
			m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), dTimeDelta);
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
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);
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
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

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

			m_pModelCom->Set_Animation(ANI_C_Jump_Land_Still_Jump);
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
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false)
	{
		m_iAirDashCount = 0;
		m_bSprint = false;
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
		else
		{
			if (m_eCurPlayerSize != SIZE_LARGE)
			{
				m_pModelCom->Set_Animation(ANI_C_Jump_Land);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			}
			else
			{
				m_pModelCom->Set_Animation(ANI_C_ChangeSize_Jump_Large_Land);
				m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
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
		m_iJumpCount = 2;
		return;
	}
}
void CCody::Change_Size(const _double dTimeDelta)
{
	if (m_IsSizeChanging == true)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_LARGE)
		{
			m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_MIDDLE_LARGE);

			if (m_vScale.x < 5.f)
			{
				m_vScale.x += (_float)dTimeDelta * 20.f;
				m_vScale.y += (_float)dTimeDelta * 20.f;
				m_vScale.z += (_float)dTimeDelta * 20.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 5.f, 5.f, 5.f };
				m_IsSizeChanging = false; 
				m_eCurPlayerSize = SIZE_LARGE;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

				//UI
				UI_Generator->UI_RETutorial(Player::Cody, UI::PC_Mouse_Enlargement);
			}
		}
		else if (m_eCurPlayerSize == SIZE_LARGE && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_LARGE_MIDDLE);

			if (m_vScale.x > 1.f)
			{
				m_vScale.x -= (_float)dTimeDelta * 20.f;
				m_vScale.y -= (_float)dTimeDelta * 20.f;
				m_vScale.z -= (_float)dTimeDelta * 20.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 1.f, 1.f, 1.f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_MEDIUM;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
		}
		else if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_SMALL)
		{
			m_pActorCom->Set_Scale(0.05f, 0.05f);
			m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_MIDDLE_SMALL);

			if (m_vScale.x > 0.5f)
			{
				m_vScale.x -= (_float)dTimeDelta * 10.f;
				m_vScale.y -= (_float)dTimeDelta * 10.f;
				m_vScale.z -= (_float)dTimeDelta * 10.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 0.2f, 0.2f, 0.2f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_SMALL;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

				//UI
				UI_Generator->UI_RETutorial(Player::Cody, UI::PC_Mouse_Reduction);
			}
		}
		else if (m_eCurPlayerSize == SIZE_SMALL && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			m_pActorCom->Set_Scale(0.5f, 0.5f);
			m_pEffect_Size->Change_Size(CEffect_Cody_Size::TYPE_SMALL_MIDDLE);

			if (m_vScale.x < 1.f)
			{
				m_vScale.x += (_float)dTimeDelta * 10.f;
				m_vScale.y += (_float)dTimeDelta * 10.f;
				m_vScale.z += (_float)dTimeDelta * 10.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 1.f, 1.f, 1.f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_MEDIUM;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
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
			m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Falling);
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
			m_bCanMove = false;
			m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Start);
			m_pActorCom->Set_Jump(false);
			m_pActorCom->Set_Gravity(0.f);
			m_fGroundPoundAirDelay += (_float)dTimeDelta;
		}
	}

	if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_GroundPound_Falling) && m_bPlayGroundPoundOnce == false)
	{
		m_bPlayGroundPoundOnce = true;
		m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Land);
		m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit);
	}
	if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_GroundPound_Land))
	{
		m_pModelCom->Set_Animation(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit);
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
	}
	if (m_pModelCom->Is_AnimFinished(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit))
	{
		m_bPlayGroundPoundOnce = false;
		m_IsAirDash = false;
		m_bCanMove = true;
	}

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
			m_pModelCom->Set_Animation(ANI_C_Bhv_Lever_Left);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_IsActivateRobotLever = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTBATTERY && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_Fwd);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Push_Battery_MH);
			m_IsPushingBattery = true;
		}
		else if (m_eTargetGameID == GameID::eSPACEVALVE && m_pGameInstance->Key_Down(DIK_E) && m_iValvePlayerName == Player::Cody)
		{
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
			}
		}
		else if (m_eTargetGameID == GameID::eHOOKUFO && m_pGameInstance->Key_Down(DIK_E) && m_IsHookUFO == false)
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
	}

	// Trigger 여따가 싹다모아~
	if (m_IsOnGrind || m_IsHitStarBuddy || m_IsHitRocket || m_IsActivateRobotLever || m_IsPushingBattery || m_IsEnterValve || m_IsInGravityPipe
		|| m_IsHitPlanet || m_IsHookUFO)
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
		|| m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_ChangeSize_PlanetPush_Large))
	{
		m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
	}
	return false;
}
#pragma endregion

void CCody::Go_Grind(const _double dTimeDelta)
{
#pragma region Grind_Actions
	if (m_IsOnGrind == true)
	{
		//
		if (m_pModelCom->Is_AnimFinished(ANI_C_Grind_Grapple_ToGrind))
		{
			m_pModelCom->Set_Animation(ANI_C_Grind_Slow_MH);
			if (m_dTestTime < 1.0)
			{
				m_pModelCom->Set_NextAnimIndex(ANI_C_Grind_Slow_MH);
			}
		}
		if (m_dTestTime >= 1.0)
		{
			_vector vPos = XMVectorSetW(XMVectorCatmullRom(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2]), XMLoadFloat3(&m_vPoints[3]), (_float)m_dTestTime), 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)));

			m_pModelCom->Set_Animation(ANI_C_Grind_Jump);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jump_Land);
			m_pActorCom->Jump_Start(2.6f);

			m_dTestTime = 0.0; //Points 1과 2사이를 곡선으로 보간 0과 3은 곡선이 어떤 형태를 띌지 수치 조절.
			m_IsOnGrind = false;
			return;
		}
		m_dTestTime += (_float)dTimeDelta / 5.f;

		_vector vPos = XMVectorSetW(XMVectorCatmullRom(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2]), XMLoadFloat3(&m_vPoints[3]), (_float)m_dTestTime), 1.f);
		_vector vPhsixPos = XMVectorSet((_float)m_pActorCom->Get_Controller()->getPosition().x, (_float)m_pActorCom->Get_Controller()->getPosition().y, (_float)m_pActorCom->Get_Controller()->getPosition().z, 1.f);
		_vector vDir = XMVectorSetW(XMVector3Normalize(vPos - vPhsixPos), 0.f);

		m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)));
		m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta / 10.f);
	}
	//_vector vCurPos = XMVectorSetW(XMVectorLerp(XMLoadFloat3(vCurNode), XMLoadFloat3(&m_vPoints[iCurNode + 1]), dProgress),1.f);
	//_vector vPhsXPos = XMVectorSet(m_pActorCom->Get_Controller()->getPosition().x, m_pActorCom->Get_Controller()->getPosition().y, m_pActorCom->Get_Controller()->getPosition().z, 1.f);
	//_vector vDir = XMVector4Normalize(vCurPos - vPhsXPos);


#pragma endregion
}

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
		//m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Bhv_Push_Battery_Fwd)
		{
		}
		if(m_pModelCom->Is_AnimFinished(ANI_C_Bhv_Push_Battery_MH))
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Push_Battery_MH);
			m_IsPushingBattery = false;
			m_IsCollide = false;
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
		}

		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pGameInstance->Key_Down(DIK_D) && m_pModelCom->Get_CurAnimIndex() != ANI_C_Bhv_Valve_Rotate_R && m_bStruggle == false)
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_Valve_Rotate_R);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Bhv_Valve_Rotate_MH);
			m_iRotateCount += 1;
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
				/*m_pTransformCom->Rotate_Axis(XMVector3Normalize(XMVectorSet(1.f, 0.f, 0.f, 0.f)), dTimeDelta * 0.1f);
				m_pTransformCom->Rotate_Axis(XMVector3Normalize(XMVectorSet(0.f, 0.f, 1.f, 0.f)), dTimeDelta * 0.1f);

				_float3 MoveDir = { 0.f, -1.f, 0.f };
				_vector vDirection = XMVector3Normalize(XMLoadFloat3(&MoveDir));
				m_pActorCom->Move(vDirection * 1.2f, dTimeDelta);*/
				/*m_pTransformCom->Go_Down(dTimeDelta);
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)));*/
			}

			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
				//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)));
				//m_pModelCom->Set_Animation(ANI_C_Bhv_PlayRoom_ZeroGravity_Fwd);
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
			m_pActorCom->Jump_Start(1.5f);
			m_pActorCom->Set_Jump(true);
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pActorCom->Set_Gravity(-9.8f);
			m_IsInGravityPipe = false;
			m_pModelCom->Set_Animation(ANI_C_MH);
		}
	}
}

void CCody::Hit_Planet(const _double dTimeDelta)
{
	if (m_IsHitPlanet == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));

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
