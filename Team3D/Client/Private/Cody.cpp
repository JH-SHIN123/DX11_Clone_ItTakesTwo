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

	m_pModelCom->Set_Animation(2, m_pTransformCom);
	CDataBase::GetInstance()->Set_CodyPtr(this);
	 

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
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	return S_OK;
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
	TriggerCheck(dTimeDelta);
	StateCheck(dTimeDelta);
	Move(dTimeDelta);
	Roll(dTimeDelta);
	Change_Size(dTimeDelta);


	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);
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
	Safe_Release(m_pCamera);
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
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, vCameraLook);
		}
		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			bMove[0] = !bMove[0];
			XMStoreFloat3(&m_vMoveDirection, -vCameraLook);
		}

		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			bMove[1] = !bMove[1];
			XMStoreFloat3(&m_vMoveDirection, -vCameraRight);
		}
		if (m_pGameInstance->Key_Pressing(DIK_D))
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

#pragma region Keyboard_Shift_Button
	if (m_pGameInstance->Key_Down(DIK_LSHIFT) && m_bRoll == false)
	{
		XMStoreFloat3(&m_vMoveDirection, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_bRoll = true;
	}
#pragma endregion

#pragma region Keyboard_Space_Button
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pActorCom->Jump_Start(2.2f);
		m_bShortJump = true;
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

}

void CCody::Move(const _double TimeDelta)
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

		// TEST!! 8번 jog start , 4번 jog , 7번 jog to stop. TEST!!
		if (m_pModelCom->Is_AnimFinished(8) == true) // JogStart -> Jog
			m_pModelCom->Set_Animation(4, m_pTransformCom);
		else if (m_pModelCom->Is_AnimFinished(4) == true) // Jog -> Jog // 보간속도 Up
			m_pModelCom->Set_Animation(4, m_pTransformCom);
		else											// Idle To Jog Start. -> Jog 예약
		{
			m_pModelCom->Set_Animation(8, m_pTransformCom);
			m_pModelCom->Set_NextAnimIndex(4);
		}
	}
	else
	{
		if (m_pModelCom->Get_CurAnimIndex() == 4) // jog 였다면
		{
			m_pModelCom->Set_Animation(12, m_pTransformCom); // jog to stop 으로 바꿔
			m_pModelCom->Set_NextAnimIndex(2); // jog to stop 끝나면 idle 예약.
		}
		else if (m_pModelCom->Get_CurAnimIndex() == 8) // JogStart 였다면
		{
			m_pModelCom->Set_Animation(12, m_pTransformCom); // jog to stop 으로 바꿔
			m_pModelCom->Set_NextAnimIndex(2);
		}
	}
}
void CCody::Roll(const _double TimeDelta)
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
void CCody::Sprint(const _double TimeDelta)
{
}
void CCody::Jump(const _double TimeDelta)
{

}
void CCody::Change_Size(const _double TimeDelta)
{
	if (m_IsSizeChanging == true)
	{
		if (m_eCurPlayerSize == SIZE_MEDIUM && m_eNextPlayerSize == SIZE_LARGE)
		{
			if (m_vScale.x < 5.f)
			{
				m_vScale.x += TimeDelta * 20.f;
				m_vScale.y += TimeDelta * 20.f;
				m_vScale.z += TimeDelta * 20.f;
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
				m_vScale.x -= TimeDelta * 20.f;
				m_vScale.y -= TimeDelta * 20.f;
				m_vScale.z -= TimeDelta * 20.f;
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
				m_vScale.x -= TimeDelta * 10.f;
				m_vScale.y -= TimeDelta * 10.f;
				m_vScale.z -= TimeDelta * 10.f;
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
				m_vScale.x += TimeDelta * 10.f;
				m_vScale.y += TimeDelta * 10.f;
				m_vScale.z += TimeDelta * 10.f;
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
void CCody::StateCheck(_double TimeDelta)
{
	// 변경해준 m_iNextState 에 따라 애니메이션을 세팅해주고 NextState를 CurState 로 바꿔준다.
	// 애니메이션 인덱스 == 상태 인덱스 같음 ㅇㅇ.
	//if (m_iCurState != m_iNextState)
	//{
	//m_iCurState = m_iNextState;
	//}
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

