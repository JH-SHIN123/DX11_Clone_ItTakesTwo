#include "stdafx.h"
#include "..\public\UFO.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "May.h"
#include "Cody.h"

CUFO::CUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUFO::CUFO(const CUFO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUFO::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUFO::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	m_pCodyTransform = ((CCody*)CDataBase::GetInstance()->GetCody())->Get_Transform();
	if (nullptr == m_pCodyTransform)
		return E_FAIL;
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataBase::GetInstance()->GetMay())->Get_Transform();
	if (nullptr == m_pMayTransform)
		return E_FAIL;
	Safe_AddRef(m_pMayTransform);

	m_pActorCom->Set_Gravity(0.f);
	m_eNextState = UFO_LASER_MH;
	m_pModelCom->Set_Animation(ANI_UFO_LASER_MH, m_pTransformCom);
	m_pModelCom->Set_NextAnimIndex(ANI_UFO_LASER_MH);

	// DataBase에 포인터 등록
	CDataBase::GetInstance()->Set_UFOPtr(this);

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	
	if (m_pGameInstance->Key_Pressing(DIK_K))
		m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_M))
		m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_N))
		m_pActorCom->Move(XMVectorSet(0.f, 1.f, 0.f, 0.f) / 10.f, dTimeDelta);
	if (m_pGameInstance->Key_Down(DIK_0))
		m_IsCoreExplode = true;
	if (m_pGameInstance->Key_Down(DIK_9))
		m_IsCodyHold = true;

	Check_State(dTimeDelta);
	Change_State(dTimeDelta);
	During_Animation_Behavior(dTimeDelta);


	PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}


CUFO::UFO_STATE CUFO::Check_State(_double dTimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		switch (m_eNextState)
		{
		case Client::CUFO::UFO_ENTIRE_ANIMATION:
			m_pModelCom->Set_Animation(0, m_pTransformCom);
			break;
		case Client::CUFO::UFO_REF:
			m_pModelCom->Set_Animation(ANI_UFO_REF, m_pTransformCom);
			break;
		case Client::CUFO::UFO_BACK:
			m_pModelCom->Set_Animation(ANI_UFO_BACK, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CODYHOLDING:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CODYHOLDING_ENTER:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING_ENTER, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CODYHOLDING_LOW:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING_LOW, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_ADDITIVE:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_ADDITIVE_BOOST:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE_BOOST, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_ADDITIVE_FLYING:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE_FLYING, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_POSE_BCK:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_BCK, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_POSE_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_FWD, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_POSE_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_LEFT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_CONTROLLABLE_POSE_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_RIGHT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_FIREROCKET_ADDITIVE_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_FIREROCKET_ADDITIVE_LEFT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_FIREROCKET_ADDITIVE_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_FIREROCKET_ADDITIVE_RIGHT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_FWD, m_pTransformCom);
			break;
		case Client::CUFO::UFO_GROUNDPOUND:
			m_pModelCom->Set_Animation(ANI_UFO_GROUNDPOUND, m_pTransformCom);
			break;
		case Client::CUFO::UFO_HITREACTION_BCK:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_BCK, m_pTransformCom);
			break;
		case Client::CUFO::UFO_HITREACTION_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_FWD, m_pTransformCom);
			break;
		case Client::CUFO::UFO_HITREACTION_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_LEFT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_HITREACTION_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_RIGHT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_KNOCKDOWNMH:
			m_pModelCom->Set_Animation(ANI_UFO_KNOCKDOWNMH, m_pTransformCom);
			break;
		case Client::CUFO::UFO_LASER_HITPOD:
			m_pModelCom->Set_Animation(ANI_UFO_LASER_HITPOD, m_pTransformCom);
			break;
		case Client::CUFO::UFO_LASER_MH:
			m_pModelCom->Set_Animation(ANI_UFO_LASER_MH, m_pTransformCom);
			break;
		case Client::CUFO::UFO_LASERRIPPEDOFF:
			m_pModelCom->Set_Animation(ANI_UFO_LASERRIPPEDOFF, m_pTransformCom);
			break;
		case Client::CUFO::UFO_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_LEFT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_MH:
			m_pModelCom->Set_Animation(ANI_UFO_MH, m_pTransformCom);
			break;
		case Client::CUFO::UFO_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_RIGHT, m_pTransformCom);
			break;
		case Client::CUFO::UFO_ROCKETKNOCKDOWN_MH:
			m_pModelCom->Set_Animation(ANI_UFO_ROCKETKNOCKDOWN_MH, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_EJECT_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_EJECT_FLYINGSAUCER, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_ENTERUFO_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_ENTERUFO_FLYINGSAUCER, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_UFO_BOSS_INTRO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_BOSS_INTRO, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_UFO_OUTRO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_OUTRO, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_POWERCORESDESTROYED_UFO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_POWERCORESDESTROYED_UFO, m_pTransformCom);
			break;
		case Client::CUFO::CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER, m_pTransformCom);
			break;
		default:
			break;
		}
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CUFO::Change_State(_double dTimeDelta)
{
		if (m_eCurState == UFO_LASER_MH && m_IsCoreExplode == true)
		{
			m_eNextState = UFO_LASER_HITPOD;
		}
		else if (m_eCurState == UFO_LASER_HITPOD && m_pModelCom->Is_AnimFinished(ANI_UFO_LASER_HITPOD) == true)
		{
			m_eNextState = CUTSCENE_POWERCORESDESTROYED_UFO;
			m_IsCoreExplode = false;
		}
		else if (m_eCurState == CUTSCENE_POWERCORESDESTROYED_UFO && m_pModelCom->Is_AnimFinished(ANI_CUTSCENE_POWERCORESDESTROYED_UFO) == true)
		{
			m_eNextState = UFO_KNOCKDOWNMH;
		}
		else if (m_eCurState == UFO_KNOCKDOWNMH)
		{
			if (m_IsCodyHold == false)
				m_eNextState = UFO_KNOCKDOWNMH;
			else if (m_IsCodyHold == true)
				m_eNextState = UFO_CODYHOLDING_ENTER;
		}
		else if (m_eCurState == UFO_CODYHOLDING_ENTER && m_pModelCom->Is_AnimFinished(ANI_UFO_CODYHOLDING_ENTER))
		{
			m_eNextState = UFO_CODYHOLDING;
		}
		else if (m_eCurState == UFO_CODYHOLDING)
		{
			if (m_IsMayLaserRippedOff == false)
				m_eNextState = UFO_CODYHOLDING;
			else
				m_eNextState = CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER; 
		}

}

void CUFO::During_Animation_Behavior(_double dTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CUFO::UFO_REF:
		break;
	case Client::CUFO::UFO_BACK:
		break;
	case Client::CUFO::UFO_CODYHOLDING:
		break;
	case Client::CUFO::UFO_CODYHOLDING_ENTER:
		break;
	case Client::CUFO::UFO_CODYHOLDING_LOW:
		break;
	case Client::CUFO::UFO_CONTROLLABLE_POSE_BCK:
		break;
	case Client::CUFO::UFO_CONTROLLABLE_POSE_FWD:
		break;
	case Client::CUFO::UFO_CONTROLLABLE_POSE_LEFT:
		break;
	case Client::CUFO::UFO_CONTROLLABLE_POSE_RIGHT:
		break;
	case Client::CUFO::UFO_FWD:
		break;
	case Client::CUFO::UFO_GROUNDPOUND:
		break;
	case Client::CUFO::UFO_HITREACTION_BCK:
		break;
	case Client::CUFO::UFO_HITREACTION_FWD:
		break;
	case Client::CUFO::UFO_HITREACTION_LEFT:
		break;
	case Client::CUFO::UFO_HITREACTION_RIGHT:
		break;
	case Client::CUFO::UFO_KNOCKDOWNMH:
		break;
	case Client::CUFO::UFO_LASER_HITPOD:
		break;
	case Client::CUFO::UFO_LASER_MH:
		Laser_Pattern(dTimeDelta);
		break;
	case Client::CUFO::UFO_LASERRIPPEDOFF:
		break;
	case Client::CUFO::UFO_LEFT:
		break;
	case Client::CUFO::UFO_MH:
		break;
	case Client::CUFO::UFO_RIGHT:
		break;
	case Client::CUFO::UFO_ROCKETKNOCKDOWN_MH:
		break;
	default:
		break;
	}
}

void CUFO::Laser_Pattern(_double dTimeDelta)
{
	_vector vDir = m_pCodyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_vector vDirForLaser = XMVector3Normalize(vDir);

	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f); // 플레이어 쪽으로 천천히 회전.

}

HRESULT CUFO::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CUFO::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CUFO::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	return S_OK;
}

CUFO * CUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFO* pInstance = new CUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUFO::Clone_GameObject(void * pArg)
{
	CUFO* pInstance = new CUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUFO::Free()
{
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}