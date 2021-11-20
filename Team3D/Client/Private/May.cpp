#include "stdafx.h"
#include "..\public\May.h"
#include "GameInstance.h"
#include "SubCamera.h"
#include "UI_Generator.h"
#include "UIObject.h"

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

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

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

	UI_Create(May, PlayerMarker);
	 

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

	CControllableActor::ARG_DESC ArgDesc;

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

void CMay::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_M_Roll_Start, ANI_M_Roll_Jog, false);
	m_pModelCom->Add_LerpInfo(ANI_M_Roll_Jog, ANI_M_Jog, true, 10.f);

	m_pModelCom->Add_LerpInfo(ANI_M_ExhaustedMH, ANI_M_Jog_Exhausted_Start, true, 10.f);
	m_pModelCom->Add_LerpInfo(ANI_M_Jog_Exhausted_Start, ANI_M_Jog, true, 20.f);

	m_pModelCom->Add_LerpInfo(ANI_M_GroundPound_Land, ANI_M_GroundPound_Land_Exit, false, 1.8f);
	m_pModelCom->Add_LerpInfo(ANI_M_Sprint, ANI_M_SprintTurnAround, true, 20.f);
	return;
}

_int CMay::Tick(_double dTimeDelta)
{
	//s
	CCharacter::Tick(dTimeDelta);


	m_pCamera = (CSubCamera*)CDataStorage::GetInstance()->Get_SubCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	KeyInput(dTimeDelta);
	if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false)
	{
		TriggerCheck(dTimeDelta);
		StateCheck(dTimeDelta);
		Sprint(dTimeDelta);
		Move(dTimeDelta);
		Roll(dTimeDelta);
		Jump(dTimeDelta);
	}
	Ground_Pound(dTimeDelta);

	UI_Generator->Set_TargetPos(Player::Cody, UI::PlayerMarker, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);
	return NO_EVENT;
}

_int CMay::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CMay::Render()
{
	CCharacter::Render();
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;

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
	CCharacter::Free();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMay::KeyInput(_double TimeDelta)
{
#pragma region Local variable
	_vector vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;
#pragma endregion

#pragma region 8Way_Move


	// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
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
			if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) || (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)) && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT) && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) || (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)) && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_UP) && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) || (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)) && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[0] = !bMove[0];
				m_pModelCom->Set_Animation(ANI_M_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_DOWN) && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (((m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint) || (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)) && m_IsTurnAround == false)
			{
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
			m_fSprintAcceleration += (_float)TimeDelta * 20.f;
	}
	if (m_pModelCom->Is_AnimFinished(ANI_M_SprintTurnAround))
	{
		m_IsTurnAround = false;
	}


	if (bMove[0] || bMove[1])
	{
		m_bMove = true;
	}

#pragma endregion

#pragma region Keyboard_Right_Shift_Button
	if (m_pGameInstance->Key_Down(DIK_RSHIFT) && m_bRoll == false && m_bCanMove == true)
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
			if (m_pModelCom->Get_CurAnimIndex() != ANI_C_AirDash_Start)
			{
				m_fAcceleration = 5.f;
				m_pActorCom->Jump_Start(1.2f);
				m_pModelCom->Set_Animation(ANI_M_AirDash_Start);
				m_IsAirDash = true;
			}
		}
	}
#pragma endregion

#pragma region Keyboard_RCtrl
	if (m_pGameInstance->Key_Down(DIK_RCONTROL) && m_iJumpCount < 2)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
	}
#pragma endregion

#pragma region Key_M
	if (m_pGameInstance->Key_Down(DIK_M))
	{
		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Sprint)
		{
			//ANI_C_Jog_Exhausted_MH_Start_Fwd
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

#pragma region Key_RAlt

	if (m_pGameInstance->Key_Down(DIK_RALT) && m_pActorCom->Get_IsJump() == true)
	{
		m_fAcceleration = 5.0f;
		m_fJogAcceleration = 25.f;
		m_fSprintAcceleration = 35.f;
		m_bGroundPound = true;
	}

#pragma endregion 

}

void CMay::Move(const _double TimeDelta)
{
	if (m_bSprint == false && m_bMove && m_pTransformCom)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta);

		if (m_fJogAcceleration > 10.f)
			m_fJogAcceleration -= (_float)TimeDelta * 50.f;
		else
			m_fJogAcceleration = 10.f;

		m_pActorCom->Move(vDirection / m_fJogAcceleration, TimeDelta);

		if (m_bRoll == false && m_IsJumping == false)
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
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH || m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH_To_Idle || m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Stop_Exhausted)
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

				m_fIdleTime += (_float)TimeDelta;
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
void CMay::Roll(const _double TimeDelta)
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

		m_fAcceleration -= (_float)TimeDelta * 10.f;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), TimeDelta);
	}

	if (m_IsAirDash && m_bCanMove == true && m_pTransformCom)
	{
		if (m_fAcceleration <= 0.f)
		{
			m_fAcceleration = 5.0;
			m_IsAirDash = false;
		}

		m_fAcceleration -= (_float)TimeDelta * 10.f;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);
		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), TimeDelta);
	}
	
}
void CMay::Sprint(const _double TimeDelta)
{
	// 내려찍기 전까지 커밋!
	if (m_bSprint == true && m_bMove == true)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		if (m_pModelCom->Get_CurAnimIndex() == ANI_M_SprintTurnAround)
			m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * 2.f);
		else
			m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta);

		if (m_fSprintAcceleration > 5.f)
			m_fSprintAcceleration -= (_float)TimeDelta * 50.f;
		else
			m_fSprintAcceleration = 5.f;

		m_pActorCom->Move(vDirection / m_fSprintAcceleration, TimeDelta);

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
		m_fIdleTime += (_float)TimeDelta;

		if (m_fIdleTime > 3.f && m_pModelCom->Get_CurAnimIndex() == ANI_M_ExhaustedMH) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
		{
			m_pModelCom->Set_Animation(ANI_M_ExhaustedMH_To_Idle);
			m_pModelCom->Set_NextAnimIndex(ANI_M_MH);
			m_bSprint = false;
			m_fIdleTime = 0.f;
		}

	}
}
void CMay::Jump(const _double TimeDelta)
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

		if (m_pGameInstance->Key_Pressing(DIK_RIGHT) || m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Key_Pressing(DIK_DOWN) || m_pGameInstance->Key_Pressing(DIK_LEFT))
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
}
void CMay::Ground_Pound(const _double TimeDelta)
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
			m_fGroundPoundAirDelay += (_float)TimeDelta;
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
void CMay::StateCheck(_double TimeDelta)
{
	// 변경해준 m_iNextState 에 따라 애니메이션을 세팅해주고 NextState를 CurState 로 바꿔준다.
	// 애니메이션 인덱스 == 상태 인덱스 같음 ㅇㅇ.
	//if (m_iCurState != m_iNextState)
	//{
	//	//m_pModelCom->Set_Animation(m_iNextState, m_pTransformCom);
	//	m_iCurState = m_iNextState;
	//}
}

void CMay::TriggerCheck(_double TimeDelta)
{
	//m_bMove = false;
	//m_bRoll = false;
}

