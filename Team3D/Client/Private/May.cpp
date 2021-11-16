#include "stdafx.h"
#include "..\public\May.h"
#include "GameInstance.h"
#include "SubCamera.h"
#include "Transform.h"
#include "DataBase.h"
#include "Effect_Generator.h"

CMay::CMay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCharacter(pDevice, pDeviceContext)
{
}

CMay::CMay(const CMay& rhs)
	: CCharacter(rhs)
{
}

HRESULT CMay::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CMay::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

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
	CDataBase::GetInstance()->Set_MayPtr(this);
	 

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
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	return S_OK;
}

_int CMay::Tick(_double dTimeDelta)
{
	//s
	CCharacter::Tick(dTimeDelta);

	m_pCamera = (CSubCamera*)CDataBase::GetInstance()->Get_SubCam();
	if (nullptr == m_pCamera)
		return NO_EVENT;

	KeyInput(dTimeDelta);
	TriggerCheck(dTimeDelta);
	StateCheck(dTimeDelta);
	Move(dTimeDelta);
	Roll(dTimeDelta);


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
	CCharacter::Free();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////	키체크, 상태체크, 애니메이션에 대한 Transform 변경	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMay::KeyInput(_double TimeDelta)
{
	// KeyProcess 에서 m_iCurState 와 m_iNextState를 비교하고, m_iNextState 를 변경해준다.

	_vector vCameraLook = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_vector vCameraRight = m_pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_bool bMove[2] = { false, false };
	_bool bRoll = false;

#pragma region 8Way_Move

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
		//Test
		if (m_pGameInstance->Key_Pressing(DIK_UP))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, vCameraLook);
		}
		if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
		}

		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, vCameraRight);
		}
	}

	if (bMove[0] || bMove[1])
	{
		m_bMove = true;
	}

#pragma endregion

	if (m_pGameInstance->Key_Down(DIK_RSHIFT) && m_bRoll == false)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_bRoll = true;
	}

	//if (m_pGameInstance->Key_Down(DIK_SPACE))
	//{
	//	m_pActorCom->Jump_Start(30.f);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_SPACE))
	//{
	//	m_pActorCom->Jump_Higher(1.f);
	//}

#pragma region Effet Test
	if (m_pGameInstance->Key_Down(DIK_P))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::May_Dead, m_pTransformCom->Get_WorldMatrix(), m_pModelCom);


#pragma  endregion

}

void CMay::Move(const _double TimeDelta)
{
	if (m_bMove && m_pTransformCom)
	{
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta);

		m_bMove = false;

		PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

		m_pActorCom->Move(vDirection / 10.f, TimeDelta);

		if (m_bShortJump == false)
		{
			// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
			if (m_pModelCom->Is_AnimFinished(ANI_M_Jog_Start) == true) // JogStart -> Jog
				m_pModelCom->Set_Animation(ANI_M_Jog);
			else if (m_pModelCom->Is_AnimFinished(ANI_M_Jog) == true) // Jog -> Jog // 보간속도 Up
				m_pModelCom->Set_Animation(ANI_M_Jog);
			else											// Idle To Jog Start. -> Jog 예약
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Start);
				m_pModelCom->Set_NextAnimIndex(ANI_M_Jog);
			}
		}
	}
	else
	{
		if (m_bShortJump == false)
		{
			if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog) // jog 였다면
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH_Gesture_Small_Stretch); // jog to stop 끝나면 idle 예약.
			}
			else if (m_pModelCom->Get_CurAnimIndex() == ANI_M_Jog_Start) // JogStart 였다면
			{
				m_pModelCom->Set_Animation(ANI_M_Jog_Stop); // jog to stop 으로 바꿔
				m_pModelCom->Set_NextAnimIndex(ANI_M_MH_Gesture_Small_Stretch);
			}
		}
	}
}
void CMay::Roll(const _double TimeDelta)
{
	if (m_fAcceleration <= 0.0)
	{
		m_fAcceleration = 5.0;
		m_bRoll = false;
	}
	if (m_bRoll && m_pTransformCom)
	{
		m_fAcceleration -= TimeDelta * 10.0;
		_vector vDirection = XMLoadFloat3(&m_vMoveDirection);
		vDirection = XMVectorSetY(vDirection, 0.f);
		vDirection = XMVector3Normalize(vDirection);

		m_pTransformCom->MoveDirectionOnLand(vDirection, TimeDelta * m_fAcceleration);
	}
	
}
void CMay::Sprint(const _double TimeDelta)
{
}
void CMay::Jump(const _double TimeDelta)
{

}
void CMay::StateCheck(_double TimeDelta)
{
	// 변경해준 m_iNextState 에 따라 애니메이션을 세팅해주고 NextState를 CurState 로 바꿔준다.
	// 애니메이션 인덱스 == 상태 인덱스 같음 ㅇㅇ.
	if (m_iCurState != m_iNextState)
	{
		//m_pModelCom->Set_Animation(m_iNextState, m_pTransformCom);
		m_iCurState = m_iNextState;
	}
}

void CMay::TriggerCheck(_double TimeDelta)
{
	//m_bMove = false;
	//m_bRoll = false;
}

