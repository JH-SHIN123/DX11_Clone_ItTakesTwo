#include "stdafx.h"
#include "..\Public\May.h"
#include "SubCamera.h"
#include "UI_Generator.h"
#include "UIObject.h"
#include "PlayerActor.h"

#include "Effect_Generator.h"
#include "Effect_May_Boots.h"

// m_pGameInstance->Get_Pad_LStickX() > 44000 (Right)
// m_pGameInstance->Get_Pad_LStickX() < 20000 (Left)
// m_pGameInstance->Get_Pad_LStickY() > 20000 (Down)
// m_pGameInstance->Get_Pad_LStickY() < 44000 (Up)

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

// 	UI_Create(May, PlayerMarker);
	 

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

	//Effect 
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_May_Boots"), nullptr, (CGameObject**)&m_pEffect_GravityBoots), E_FAIL);
	m_pEffect_GravityBoots->Set_Model(m_pModelCom);

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

	m_pModelCom->Add_LerpInfo(ANI_M_DoubleJump, ANI_M_GroundPound_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Jump_Start, ANI_M_AirDash_Start, false);
	m_pModelCom->Add_LerpInfo(ANI_M_DoubleJump, ANI_M_AirDash_Start, false);

	return;
}

_int CMay::Tick(_double dTimeDelta)
{
	//s
	CCharacter::Tick(dTimeDelta);


	m_pCamera = (CSubCamera*)CDataStorage::GetInstance()->Get_SubCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	if (Trigger_Check(dTimeDelta))
	{
		Go_Grind(dTimeDelta);
		Hit_StarBuddy(dTimeDelta);
		Hit_Rocket(dTimeDelta);
		Activate_RobotLever(dTimeDelta);
		Pull_VerticalDoor(dTimeDelta);
		Rotate_Valve(dTimeDelta);
		In_GravityPipe(dTimeDelta);
		Warp_Wormhole(dTimeDelta);
		Touch_FireDoor(dTimeDelta);
	}
	else
	{
		// 트리거 끝나고 애니메이션 초기화
		Trigger_End(dTimeDelta);
		m_IsFalling = m_pActorCom->Get_IsFalling();
		m_pActorCom->Set_GroundPound(m_bGroundPound);

		if (m_bRoll == false || m_bSprint == true)
			KeyInput(dTimeDelta);
		if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false)
		{
			Sprint(dTimeDelta);
			Move(dTimeDelta);
			Roll(dTimeDelta);
			Jump(dTimeDelta);
		}
		Ground_Pound(dTimeDelta);
	}

	//UI_Generator->Set_TargetPos(Player::Cody, UI::PlayerMarker, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);
	m_pEffect_GravityBoots->Update_Matrix(m_pTransformCom->Get_WorldMatrix());
	return NO_EVENT;
}

_int CMay::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CMay::Render(RENDER_GROUP::Enum eGroup)
{
	CCharacter::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

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
	//Safe_Release(m_pCamera);
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
#pragma region Local variable
	_vector vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;

#pragma endregion

#pragma region 8Way_Move

	if (m_IsAirDash == false)
	{
		// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
		// 오른쪽윈
		if (m_pGameInstance->Get_Pad_LStickX() > 44000 && m_pGameInstance->Get_Pad_LStickY() < 34000)
		{
			//bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook + vCameraRight) / 2.f);
		}
		// 왼쪽위
		else if (m_pGameInstance->Get_Pad_LStickX() < 20000 && m_pGameInstance->Get_Pad_LStickY() < 34000)
		{
			//bMove[0] = !bMove[0];
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (vCameraLook - vCameraRight) / 2.f);
		}
		// 오른쪽아래
		else if (m_pGameInstance->Get_Pad_LStickX() > 44000 && m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			bMove[0] = !bMove[0];
			//bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook + vCameraRight) / 2.f);
		}
		// 왼쪽아래
		else if (m_pGameInstance->Get_Pad_LStickX() < 20000 && m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			bMove[0] = !bMove[0];
			//bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, (-vCameraLook - vCameraRight) / 2.f);
		}
		else
		{
			if (m_pGameInstance->Get_Pad_LStickX() <= 25000 && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickX() >= 41000 && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_fSprintAcceleration = 15.f;
					bMove[1] = !bMove[1];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickY() <= 25000 && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}
			if (m_pGameInstance->Get_Pad_LStickY() >= 41000 && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
			{
				if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) /*|| (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)*/) && m_IsTurnAround == false)
				{
					m_fSprintAcceleration = 15.f;
					bMove[0] = !bMove[0];
					m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
					m_IsTurnAround = true;
					return;
				}
			}

			if (m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Get_Pad_LStickY() < 20000)
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, vCameraLook);
				m_iSavedKeyPress = UP;
			}
			if (m_pGameInstance->Key_Pressing(DIK_DOWN) || m_pGameInstance->Get_Pad_LStickY() > 44000)
			{
				bMove[0] = !bMove[0];
				XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
				m_iSavedKeyPress = DOWN;
			}

			if (m_pGameInstance->Key_Pressing(DIK_LEFT) || m_pGameInstance->Get_Pad_LStickX() < 20000)
			{
				bMove[1] = !bMove[1];
				XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
				m_iSavedKeyPress = LEFT;
			}
			if (m_pGameInstance->Key_Pressing(DIK_RIGHT) || m_pGameInstance->Get_Pad_LStickX() > 44000)
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
	if (m_pGameInstance->Pad_Key_Down(DIP_X) && m_bRoll == false && m_bCanMove == true)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			m_fAcceleration = 5.f;
			m_pModelCom->Set_Animation(ANI_M_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Roll_Stop);

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
				m_pModelCom->Set_Animation(ANI_M_AirDash_Start);
				m_IsAirDash = true;
			}
		}
	}
#pragma endregion

#pragma region PAD X
	if (m_pGameInstance->Pad_Key_Down(DIP_B) && m_iJumpCount < 2)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
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

	if (m_pGameInstance->Pad_Key_Down(DIP_A) && m_pActorCom->Get_IsJump() == true)
	{
		m_fAcceleration = 5.0f;
		m_fJogAcceleration = 25.f;
		m_fSprintAcceleration = 35.f;
		m_bGroundPound = true;
	}

#pragma endregion 

#pragma region PAD RB
	//m_IsActivate = false;
	if (m_pGameInstance->Pad_Key_Down(DIP_RB))
	{
		//if (m_eTargetGameID == GameID::eVERTICALDOOR && false == m_IsPullVerticalDoor)
		//	m_IsPullVerticalDoor = true;
		//else
		//	m_IsPullVerticalDoor = false;
	}


#pragma endregion

#pragma region Effet Test
	if (m_pGameInstance->Key_Down(DIK_P))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);
	if (m_pGameInstance->Key_Pressing(DIK_TAB))
		m_pEffect_GravityBoots->Set_IsActivate_GravityBoots();
	if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
		m_pEffect_GravityBoots->Add_WalkingParticle(true);

#pragma  endregion
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
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta);

		if (m_fJogAcceleration > 10.f)
			m_fJogAcceleration -= (_float)dTimeDelta * 50.f;
		else
			m_fJogAcceleration = 10.f;

		m_pActorCom->Move(vDirection / m_fJogAcceleration, dTimeDelta);

		if (m_bRoll == false && m_IsJumping == false && m_IsFalling == false && ANI_M_Jump_Land_Jog != m_pModelCom->Get_CurAnimIndex())
		{
			// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
			if (m_pModelCom->Is_AnimFinished(ANI_M_Jog_Start) == true) // JogStart -> Jog
			{
				m_pModelCom->Set_Animation(ANI_M_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Jog) == true) // Jog -> Jog // 보간속도 Up
			{
				m_pModelCom->Set_Animation(ANI_M_Jog);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop || m_pModelCom->Get_CurAnimIndex() == ANI_M_MH || m_pModelCom->Get_CurAnimIndex() == ANI_M_MH_Gesture_Small_Stretch || m_pModelCom->Get_CurAnimIndex() == ANI_M_ActionMH)	// Idle To Jog Start. -> Jog 예약
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if ((m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH_To_Idle ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted))
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Exhausted_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
			{
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
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH); // jog to stop 끝나면 idle 예약.
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Start) // JogStart 였다면
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_MH) // IDLE 상태라면
			{

				m_fIdleTime += (_float)dTimeDelta;
				if (m_bAction == false)
				{
					if (m_fIdleTime > 5.f && m_pModelCom->Is_AnimFinished(ANI_M_MH)) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
					{
						m_pModelCom->Set_Animation(ANI_M_MH_Gesture_Small_Stretch); // 배 두들기는 애니메이션 재생
						m_fIdleTime = 0.f;
					}
				}
				else if (m_bAction == true)
				{
					m_pModelCom->Set_Animation(ANI_M_MH_To_Action);
					m_pModelCom->Set_NextAnimIndex(ANI_M_ActionMH);
				}
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_MH_To_Action) == true && m_bAction == true)
			{
				m_pModelCom->Set_Animation(ANI_M_ActionMH);
				m_pModelCom->Set_NextAnimIndex(ANI_M_ActionMH_To_Idle);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_ActionMH) == true && m_bAction == true)
			{
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
			m_fAcceleration = 5.0;
			m_pModelCom->Set_Animation(ANI_M_Roll_Stop);
			if(m_bMove == false)
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);

			m_bRoll = false;
			m_bAction = true;
			return;
		}

		m_fAcceleration -= (_float)dTimeDelta * 10.f;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), dTimeDelta);
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
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);
		m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), dTimeDelta);
	}
	
}
void CMay::Sprint(const _double dTimeDelta)
{
	// 내려찍기 전까지 커밋!
	if (m_bSprint == true && m_bMove == true)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_SprintTurnAround)
		{
			m_pTransformCom->MoveDirectionOnLand(vDirection, dTimeDelta * 3.f);
		}
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
				m_pModelCom->Set_Animation(ANI_M_Sprint);
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Sprint) == true) // Jog -> Jog // 보간속도 Up
				m_pModelCom->Set_Animation(ANI_M_Sprint);
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_MH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_MH_Gesture_Small_Stretch ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_ActionMH ||
				m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop)	// Idle To Jog Start. -> Jog 예약
			{
				m_pModelCom->Set_Animation(ANI_M_Sprint_Start_FromDash);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Sprint);
			}
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Roll_Start)) // 구르고 나서 바로 움직이면 Roll to Jog
			{
				m_bRoll = false;
				if (m_bSprint == false)
				{
					m_pModelCom->Set_Animation(ANI_M_Roll_Jog);
					m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
				}
				else
				{
					m_pModelCom->Set_Animation(ANI_M_Sprint_Start_FromDash);
					m_pModelCom->Set_NextAnimIndex(ANI_M_Sprint);
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
				m_bSprint = false;
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop_Exhausted); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_ExhaustedMH); // jog to stop 끝나면 idle 예약.
			}
		}
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint_Start_FromDash) // JogStart 였다면
	{
		m_bSprint = false;
		m_pModelCom->Set_Animation(ANI_M_Jog_Stop_Exhausted); // jog to stop 으로 바꿔
		m_pModelCom->Set_NextAnimIndex(ANI_M_ExhaustedMH);
	}
	else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH) // IDLE 상태라면
	{

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
			m_pModelCom->Set_Animation(ANI_M_Jump_Start);
			m_bShortJump = false;
		}
		if (m_iJumpCount == 2)
		{
			m_IsJumping = true;
			m_pActorCom->Jump_Start(2.6f);
			m_pModelCom->Set_Animation(ANI_M_DoubleJump);
			m_bShortJump = false;
		}
	}
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false)
	{
		m_bSprint = false;
		m_iAirDashCount = 0;


		if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
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
			}
			m_bFallAniOnce = true;
		}
	}
	else if (m_IsJumping == false && m_IsFalling == false && m_bFallAniOnce == true && m_bRoll == false && m_bGroundPound == false)
	{
		if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Get_Pad_LStickY() > 44000)
		{
			m_pModelCom->Set_Animation(ANI_M_Jump_Land_Jog);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
		}
		else
		{
			m_pModelCom->Set_Animation(ANI_M_Jump_Land);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		}

		m_bFallAniOnce = false;
		m_IsJumping = false;
		m_iJumpCount = 0;
		m_iAirDashCount = 0;
	}
	if (m_pGameInstance->Pad_Key_Down(DIP_B) && m_IsFalling == true)
	{
		m_bShortJump = true;
		m_IsJumping = true;
		m_iJumpCount = 2;
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
			m_pActorCom->Set_Gravity(0.f);
			m_fGroundPoundAirDelay += (_float)dTimeDelta;
		}
	}

	if (m_pModelCom->Is_AnimFinished(ANI_M_GroundPound_Falling) && m_bPlayGroundPoundOnce == false)
	{
		m_bPlayGroundPoundOnce = true;
		m_pModelCom->Set_Animation(ANI_M_Jump_Land);
	}
	if (m_pModelCom->Is_AnimFinished(ANI_M_Jump_Land))
	{
		m_IsAirDash = false;
		m_bPlayGroundPoundOnce = false;
		m_bCanMove = true;
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
	}

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

_bool CMay::Trigger_Check(const _double dTimeDelta)
{
	if (m_IsCollide == true)
	{
		if (m_eTargetGameID == GameID::eSTARBUDDY && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_M_BruteCombat_Attack_Var1);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsHitStarBuddy = true;
		}
		else if (m_eTargetGameID == GameID::eMOONBABOON && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_M_Grind_Grapple_Enter);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Grind_Grapple_ToGrind);
			m_IsOnGrind = true;
		}
		else if (m_eTargetGameID == GameID::eROCKET && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_M_RocketFirework);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsHitRocket = true;
		}
		else if (m_eTargetGameID == GameID::eROBOTLEVER && m_pGameInstance->Key_Down(DIK_E))
		{
			m_pModelCom->Set_Animation(ANI_M_Lever_Left);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_IsActivateRobotLever = true;
		}
 		else if (m_eTargetGameID == GameID::eVERTICALDOOR && m_pGameInstance->Pad_Key_Down(DIP_RB)) // 패드입력
 		{
 			m_pModelCom->Set_Animation(ANI_M_Bounce2); // Trees/DoorInteraction 추출해야함.
 			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
 			m_IsPullVerticalDoor = true;
 		}
		else if (m_eTargetGameID == GameID::eSPACEVALVE && m_pGameInstance->Key_Down(DIK_END) && m_iValvePlayerName == Player::May)
		{
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
		else if (GameID::eWARPGATE == m_eTargetGameID && false == m_IsWarpNextStage)
		{
			if (false == m_IsWarpNextStage)
			{
				_vector vWormholePos = XMVectorZero();


			}
		}
	}

	// Trigger 여따가 싹다모아~
	if (m_IsOnGrind || m_IsHitStarBuddy || m_IsHitRocket || m_IsActivateRobotLever || m_IsPullVerticalDoor || m_IsEnterValve || m_IsInGravityPipe 
		|| m_IsWarpNextStage || m_IsWarpDone || m_IsTouchFireDoor)
		return true;

	return false;
}
_bool CMay::Trigger_End(const _double dTimeDelta)
{
	if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jump_Land || 
		m_pModelCom->Get_CurAnimIndex() == ANI_M_RocketFirework || 
		m_pModelCom->Get_CurAnimIndex() == ANI_M_BruteCombat_Attack_Var1 ||
		m_pModelCom->Get_CurAnimIndex() == ANI_M_Lever_Left ||
		m_pModelCom->Get_CurAnimIndex() == ANI_M_Valve_Rotate_MH)
	{
		m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
		m_IsCollide = false;
	}
	return false;
}
#pragma endregion

void CMay::Go_Grind(const _double dTimeDelta)
{
}

void CMay::Hit_StarBuddy(const _double dTimeDelta)
{
	if (m_IsHitStarBuddy == true)
	{
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));

		if (m_pModelCom->Is_AnimFinished(ANI_M_BruteCombat_Attack_Var1))
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
		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
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
	if (m_pGameInstance->Pad_Key_Down(DIP_LB) || m_pGameInstance->Key_Down(DIK_E))
		IsTriggerEnd = true;

	if (m_IsPullVerticalDoor == true)
	{
		m_pModelCom->Set_Animation(ANI_M_RocketFirework);

		_vector vSwitchPos = XMLoadFloat3(&m_vTriggerTargetPos);
		vSwitchPos.m128_f32[3] = 1.f;

		m_pActorCom->Set_ZeroGravity(true, false, true);
		m_pActorCom->Set_Position(vSwitchPos);

		if (true == IsTriggerEnd)
		{
			m_pActorCom->Set_ZeroGravity(false, false, false);
			m_pModelCom->Set_Animation(ANI_M_MH);
			m_IsCollide = false;
			m_IsPullVerticalDoor = false;
		}
	}
}

void CMay::Rotate_Valve(const _double dTimeDelta)
{

	if (m_IsEnterValve == true)
	{
		if (DATABASE->Get_ValveCount() == 6)
		{
			m_bStruggle = false;
			m_iRotateCount = 0;
			m_IsEnterValve = false;
			m_IsCollide = false;
			m_pModelCom->Set_Animation(ANI_M_MH);
		}

		m_pTransformCom->Rotate_ToTargetOnLand(XMLoadFloat3(&m_vTriggerTargetPos));
		if (m_pGameInstance->Key_Down(DIK_RIGHT) && m_pModelCom->Get_CurAnimIndex() != ANI_M_Valve_Rotate_R && m_bStruggle == false)
		{
			m_pModelCom->Set_Animation(ANI_M_Valve_Rotate_R);
			m_pModelCom->Set_NextAnimIndex(ANI_M_Valve_Rotate_MH);
			m_iRotateCount += 1;
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
			if (m_pGameInstance->Key_Pressing(DIK_RCONTROL))
			{
				m_pActorCom->Set_ZeroGravity(true, true, false);
			}

			if (m_pGameInstance->Key_Pressing(DIK_RALT))
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
			// m_pGameInstance->Get_Pad_LStickX() > 44000 (Right)
			// m_pGameInstance->Get_Pad_LStickX() < 20000 (Left)
			// m_pGameInstance->Get_Pad_LStickY() > 20000 (Down)
			// m_pGameInstance->Get_Pad_LStickY() < 44000 (Up)
			if (m_pGameInstance->Get_Pad_LStickY() < 44000)
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
				//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)));
				//m_pModelCom->Set_Animation(ANI_C_Bhv_PlayRoom_ZeroGravity_Fwd);
			}
			if (m_pGameInstance->Get_Pad_LStickX() < 20000)
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Get_Pad_LStickY() > 20000)
			{
				_vector vDir = XMVector3Normalize(XMVectorSetY(m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f, 0.f));
				m_pTransformCom->MoveDirectionOnLand(vDir, dTimeDelta / 2.f);
				m_pActorCom->Move(vDir / 20.f, dTimeDelta);
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta / 4.f);
			}
			if (m_pGameInstance->Get_Pad_LStickX() > 44000)
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
			m_pModelCom->Set_Animation(ANI_M_MH);
		}
	}
}

void CMay::Warp_Wormhole(const _double dTimeDelta)
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
			m_IsWarpNextStage = false;

			_vector vNextStage_Pos = XMLoadFloat3(&m_vTriggerTargetPos);
			vNextStage_Pos.m128_f32[3] = 1.f;

			m_pActorCom->Set_Position(vNextStage_Pos);

			_matrix PortalMatrix = XMLoadFloat4x4(&m_TriggerTargetWorld);
			_vector vTriggerPos = PortalMatrix.r[3];
			_vector vLook = PortalMatrix.r[2];
			vTriggerPos += vLook * 20.f;
			m_pTransformCom->Rotate_ToTargetOnLand(vTriggerPos);
		}
	}
	else
	{
		_matrix PortalMatrix = XMLoadFloat4x4(&m_TriggerTargetWorld);
		_vector vTriggerPos = PortalMatrix.r[3];
		_vector vLook = PortalMatrix.r[2];
		vTriggerPos += vLook * 20.f;
		m_pTransformCom->Rotate_ToTargetOnLand(vTriggerPos);

		// 슈루룩
		if (m_fWarpTimer_Max + 0.25f >= m_fWarpTimer)
		{
			_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vDir = XMVector3Normalize(vDir);
			m_pActorCom->Move(vDir, dTimeDelta * 0.4f);
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

	CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::Cody_Dead_Fire, m_pTransformCom->Get_WorldMatrix());
	m_IsTouchFireDoor = false;
	m_IsCollide = false;
	// Get리스폰
}





