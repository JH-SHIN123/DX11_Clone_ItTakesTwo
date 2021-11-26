#include "stdafx.h"
#include "..\public\SpaceValve.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"

#include "InGameEffect.h"
#include "Effect_Generator.h"

CSpaceValve::CSpaceValve(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpaceValve::CSpaceValve(const CSpaceValve & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpaceValve::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSpaceValve::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	EFFECT_DESC_CLONE a;
	if (nullptr != pArg)
		memcpy(&a, pArg, sizeof(EFFECT_DESC_CLONE));

	if (a.iPlayerValue == 1)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 1.5f, 20.f, 1.f));
		m_iTargetPlayer = GameID::eCODY;
	}
	else if (a.iPlayerValue == 2)
	{
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(35.f, 1.5f, 20.f, 1.f));
		m_iTargetPlayer = GameID::eMAY;
	}

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eSPACEVALVE, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxSphereGeometry(1.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	return S_OK;
}

_int CSpaceValve::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_iTargetPlayer == GameID::eCODY)
	{
		if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide == true)
		{
			UI_Delete(May, InputButton_InterActive);
			UI_Delete(Cody, InputButton_InterActive);

			m_bEnterValve = true;
			// 키보드 화살표 UI 생성.
		}
	}
	else if (m_iTargetPlayer == GameID::eMAY)
	{
		if (m_pGameInstance->Key_Down(DIK_END) && m_IsCollide == true)
		{
			UI_Delete(May, InputButton_InterActive);
			UI_Delete(Cody, InputButton_InterActive);

			m_bEnterValve = true;
			// 키보드 화살표 UI 생성.
		}
	}


	if (m_bEnterValve == true)
	{
		Rotate_SpaceValve(dTimeDelta);
	}

	return NO_EVENT;
}

_int CSpaceValve::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CSpaceValve::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

void CSpaceValve::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (m_iTargetPlayer == Player::Cody)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eSPACEVALVE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION), Player::Cody);
			UI_Create(Cody, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		{
			m_IsCollide = false;
			UI_Delete(Cody, InputButton_InterActive);
		}
	}
	//May
	if (m_iTargetPlayer == Player::May)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		{
			((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eSPACEVALVE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION), Player::May);
			UI_Create(May, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		{
			m_IsCollide = false;
			UI_Delete(May, InputButton_InterActive);
		}
	}
}

HRESULT CSpaceValve::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

void CSpaceValve::Rotate_SpaceValve(_double dTimeDelta)
{
	// 축은 맵찍고나서 수정해야함.
	if (m_iTargetPlayer == GameID::eCODY)
	{
		if (m_pGameInstance->Key_Down(DIK_D) && m_bRotate == false && m_iRotateCount < 3)
		{
			m_iRotateCount += 1;
			m_bRotate = true;
		}

		if (m_bRotate == true)
		{
			m_fRotateDelay += (_float)dTimeDelta;
			if (m_fRotateDelay < 2.1f)
			{
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);
			}
			else if (m_fRotateDelay >= 2.1f)
			{
				m_bRotate = false;
				m_fRotateDelay = 0.f;
			}

		}
	}

	else if (m_iTargetPlayer == GameID::eMAY)
	{
		if (m_pGameInstance->Key_Down(DIK_RIGHT) && m_bRotate == false && m_iRotateCount < 3)
		{
			m_iRotateCount += 1;
			m_bRotate = true;
		}

		if (m_bRotate == true)
		{
			m_fRotateDelay += (_float)dTimeDelta;
			if (m_fRotateDelay < 2.1f)
			{
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);
			}
			else if (m_fRotateDelay >= 2.1f)
			{
				m_bRotate = false;
				m_fRotateDelay = 0.f;
			}

		}
	}
}


CSpaceValve * CSpaceValve::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSpaceValve* pInstance = new CSpaceValve(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSpaceValve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpaceValve::Clone_GameObject(void * pArg)
{
	CSpaceValve* pInstance = new CSpaceValve(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSpaceValve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceValve::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}