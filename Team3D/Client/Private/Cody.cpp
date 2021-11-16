#include "stdafx.h"
#include "..\public\Cody.h"
#include "GameInstance.h"
#include "MainCamera.h"
#include "Transform.h"
#include "DataBase.h"

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

	m_pModelCom->Set_Animation(ANI_C_MH);
	CDataBase::GetInstance()->Set_CodyPtr(this);	
	Add_LerpInfo_To_Model();
	 

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
	
	
	PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.height = 0.5f;
	CapsuleControllerDesc.radius = 0.5f;
	CapsuleControllerDesc.material = m_pGameInstance->Create_PxMaterial(0.5f, 0.5f, 0.5f);
	CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CapsuleControllerDesc.contactOffset = 0.02f;
	CapsuleControllerDesc.stepOffset = 0.5f;
	CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	CapsuleControllerDesc.slopeLimit = 0.707f;
	CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	////CapsuleControllerDesc.reportCallback = NULL;
	////CapsuleControllerDesc.behaviorCallback = NULL;
	//CapsuleControllerDesc.density = 10.f;
	//CapsuleControllerDesc.scaleCoeff = 0.8f;
	//CapsuleControllerDesc.invisibleWallHeight = 0.f;
	//CapsuleControllerDesc.maxJumpHeight = 10.f;
	//CapsuleControllerDesc.volumeGrowth = 1.5f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom, CapsuleControllerDesc, -50.f)), E_FAIL);

	return S_OK;
}

void CCody::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_Jump_Start, false);
	//m_pModelCom->Add_LerpInfo(1, 2, true, 8.f);
	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_Sprint, false);
	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_MH, true, 10.f);
	return;
}

#pragma endregion

#pragma region Overrided Function
_int CCody::Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);

	m_pCamera = (CMainCamera*)CDataBase::GetInstance()->Get_MainCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	KeyInput(dTimeDelta);
	if (m_bGroundPound == false && m_bPlayGroundPoundOnce == false)
	{
		TriggerCheck(dTimeDelta);
		Sprint(dTimeDelta);
		Move(dTimeDelta);
		Roll(dTimeDelta);
		Jump(dTimeDelta);
		Change_Size(dTimeDelta);
	}
	Ground_Pound(dTimeDelta);

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);
	return NO_EVENT;
}

_int CCody::Late_Tick(_double dTimeDelta)
{
	CCharacter::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CCody::Render()
{
	CCharacter::Render();
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

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
	CCharacter::Free();
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCody::KeyInput(_double TimeDelta)
{
#pragma region Local variable
	_vector vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;
#pragma endregion

#pragma region 8Way_Move
	

	// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
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
		if (m_pGameInstance->Key_Pressing(DIK_A) && m_iSavedKeyPress == RIGHT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_D) && m_iSavedKeyPress == LEFT)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_W) && m_iSavedKeyPress == DOWN)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
				m_IsTurnAround = true;
				return;
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_S) && m_iSavedKeyPress == UP)// 이전에 눌렀엇던 키가 DIK_D였다면?)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint && m_IsTurnAround == false)
			{
				m_fSprintAcceleration = 15.f;
				bMove[1] = !bMove[1];
				m_pModelCom->Set_Animation(ANI_C_SprintTurnAround);
				m_IsTurnAround = true;
				return;
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
		if(m_fSprintAcceleration < 12.f)
			m_fSprintAcceleration += TimeDelta * 20.f;
	}
	if (m_pModelCom->Is_AnimFinished(ANI_C_SprintTurnAround))
	{
		m_IsTurnAround = false;
	}


	if (bMove[0] || bMove[1])
	{
		m_bMove = true;
	}

#pragma endregion

#pragma region Keyboard_Shift_Button
	if (m_pGameInstance->Key_Down(DIK_LSHIFT) && m_bRoll == false)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (m_IsJumping == false)
		{
			m_pModelCom->Set_Animation(ANI_C_Roll_Start);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Roll_Stop);

			m_bAction = false;
			m_bRoll = true;
		}
		else
		{
			if (m_pModelCom->Get_CurAnimIndex() != ANI_C_AirDash_Start)
			{
				m_pActorCom->Jump_Start(1.2f);
				m_pModelCom->Set_Animation(ANI_C_AirDash_Start);
				m_IsAirDash = true;
			}
		}
	}
#pragma endregion

#pragma region Keyboard_Space_Button
	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iJumpCount < 2)
	{
		m_bShortJump = true;
		m_iJumpCount += 1;
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
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_eNextPlayerSize = SIZE_SMALL;
			m_IsSizeChanging = true;
			break;
		}
	}
#pragma endregion

#pragma region Key_CapsLock
	if (m_pGameInstance->Key_Down(DIK_CAPSLOCK))
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

}
void CCody::Move(const _double TimeDelta)
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

			m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta);


			PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

			if (m_fJogAcceleration > 10.f)
				m_fJogAcceleration -= (_float)TimeDelta * 50.f;
			else
				m_fJogAcceleration = 10.f;

			m_pActorCom->Move(vDirection / m_fJogAcceleration, TimeDelta);
			
			if (m_bRoll == false && m_IsJumping == false)
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

					m_fIdleTime += (_float)TimeDelta;
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

	else if (m_eCurPlayerSize == SIZE_LARGE)
	{

	}

}
void CCody::Roll(const _double TimeDelta)
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

		m_fAcceleration -= TimeDelta * 10.0;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), TimeDelta);
	}

	if (m_IsAirDash && m_pTransformCom)
	{
		if (m_fAcceleration <= 0.f)
		{
			m_fAcceleration = 5.0;
			m_IsAirDash = false;
		}

		m_fAcceleration -= TimeDelta * 10.0;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);
		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * m_fAcceleration);
		m_pActorCom->Move(vDirection * (m_fAcceleration / 10.f), TimeDelta);
	}
	
	
}
void CCody::Sprint(const _double TimeDelta)
{
	if (m_bSprint == true && m_bMove == true)
	{
		m_bAction = false;

		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta);


		PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

		if (m_fSprintAcceleration > 5.f)
			m_fSprintAcceleration -= (_float)TimeDelta * 50.f;
		else
			m_fSprintAcceleration = 5.f;

		m_pActorCom->Move(vDirection / m_fSprintAcceleration, TimeDelta);

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
			if (m_pModelCom->Get_CurAnimIndex() == ANI_C_Sprint) // jog 였다면
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
		m_fIdleTime += (_float)TimeDelta;

		if (m_fIdleTime > 3.f && m_pModelCom->Get_CurAnimIndex() == ANI_C_Exhausted_MH) // IDLE 상태이고 IDLE 상태가 된지 시간이 5초정도 지났다면
		{
			m_pModelCom->Set_Animation(ANI_C_Exhausted_MH_To_Idle);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
			m_bSprint = false;
			m_fIdleTime = 0.f;
		}

	}
}
void CCody::Jump(const _double TimeDelta)
{
	if (m_bShortJump == true)
	{
		if (m_iJumpCount == 1)
		{
			m_IsJumping = true;
			m_pActorCom->Jump_Start(2.6f);
			m_pModelCom->Set_Animation(ANI_C_Jump_Land_Still_Jump);
			m_bShortJump = false;
		}
		if (m_iJumpCount == 2)
		{
			m_IsJumping = true;
			//m_pActorCom->Jump_Higher(1.4f);
			//m_pActorCom->Set_Gravity(0.f);
			m_pActorCom->Jump_Start(2.6f);
			m_pModelCom->Set_Animation(ANI_C_DoubleJump);
			m_bShortJump = false;
		}
	}
	if (m_IsJumping == true && m_pActorCom->Get_IsJump() == false)
	{
		m_bSprint = false;
		if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pModelCom->Set_Animation(ANI_C_Jump_Land_Jog);
			m_pModelCom->Set_NextAnimIndex(ANI_C_Jog);
		}
		else
		{
			m_pModelCom->Set_Animation(ANI_C_Jump_Land);
			m_pModelCom->Set_NextAnimIndex(ANI_C_MH);
		}
		m_IsJumping = false;
		m_iJumpCount = 0;
	}
}
void CCody::Change_Size(const _double TimeDelta)
{
	if (m_IsSizeChanging == true)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_LARGE)
		{
			if (m_vScale.x < 5.f)
			{
				m_vScale.x += (_float)TimeDelta * 20.f;
				m_vScale.y += (_float)TimeDelta * 20.f;
				m_vScale.z += (_float)TimeDelta * 20.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 5.f, 5.f, 5.f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_LARGE;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
		}
		else if (m_eCurPlayerSize == SIZE_LARGE && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			if (m_vScale.x > 1.f)
			{
				m_vScale.x -= (_float)TimeDelta * 20.f;
				m_vScale.y -= (_float)TimeDelta * 20.f;
				m_vScale.z -= (_float)TimeDelta * 20.f;
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
			if (m_vScale.x > 0.5f)
			{
				m_vScale.x -= (_float)TimeDelta * 10.f;
				m_vScale.y -= (_float)TimeDelta * 10.f;
				m_vScale.z -= (_float)TimeDelta * 10.f;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
			else
			{
				m_vScale = { 0.2f, 0.2f, 0.2f };
				m_IsSizeChanging = false;
				m_eCurPlayerSize = SIZE_SMALL;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
			}
		}
		else if (m_eCurPlayerSize == SIZE_SMALL && m_eNextPlayerSize == SIZE_MEDIUM)
		{
			if (m_vScale.x < 1.f)
			{
				m_vScale.x += (_float)TimeDelta * 10.f;
				m_vScale.y += (_float)TimeDelta * 10.f;
				m_vScale.z += (_float)TimeDelta * 10.f;
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
void CCody::Ground_Pound(const _double TimeDelta)
{
	if (m_bGroundPound == true)
	{
		if (m_fGroundPoundAirDelay > 0.4f)
		{
			m_pModelCom->Set_Animation(ANI_C_Bhv_GroundPound_Falling);
			m_pActorCom->Set_Gravity(-9.8f);
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
			m_fGroundPoundAirDelay += TimeDelta;
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
		m_bCanMove = true;
	}

}
void CCody::TriggerCheck(_double TimeDelta)
{
	//m_bMove = false;
	//m_bRoll = false;
}


#pragma region Shader_Variables
HRESULT CCody::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CCody::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	return S_OK;
}
#pragma endregion

