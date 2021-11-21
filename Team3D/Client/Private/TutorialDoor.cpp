#include "stdafx.h"
#include "..\public\TutorialDoor.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"

CTutorialDoor::CTutorialDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTutorialDoor::CTutorialDoor(const CTutorialDoor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTutorialDoor::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTutorialDoor::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_TutorialDoor"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(35.f, 1.f, 25.f, 1.f));

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eVERTICALDOOR, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = &PxSphereGeometry(1.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CTutorialDoor::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide)
	{
		m_bPull = true;
		UI_Delete(Cody, InputButton_InterActive);
	}

	if (m_bPull == false)
	{
	}

	else if (m_bPull == true)
	{
		m_fLifeTime += (_float)dTimeDelta;
		if (m_fLifeTime <= 0.71f)
		{
			m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
			m_pTransformCom->RotatePitch(dTimeDelta * 0.2f);
		}
		else if(m_fLifeTime > 0.71f)
			Pull_TutorialDoor(dTimeDelta);

		if (m_fLifeTime > 3.5f)
			return EVENT_DEAD; // 
	}

	return NO_EVENT;
}


_int CTutorialDoor::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	InterActive_UI();

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CTutorialDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CTutorialDoor::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eVERTICALDOOR, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
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

HRESULT CTutorialDoor::InterActive_UI()
{
	CCody* pCody = (CCody*)DATABASE->GetCody();
	NULL_CHECK_RETURN(pCody, E_FAIL);
	CMay* pMay = (CMay*)DATABASE->GetMay();
	NULL_CHECK_RETURN(pMay, E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCodyComparePos = vPos - vCodyPos;
	_vector vMayComparePos = vPos - vMayPos;

	_float fRange = 20.f;

	_float vCodyComparePosX = abs(XMVectorGetX(vCodyComparePos));
	_float vCodyComparePosZ = abs(XMVectorGetZ(vCodyComparePos));

	if (fRange >= vCodyComparePosX && fRange >= vCodyComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::Cody, UI::InputButton_Dot))
			UI_Create(Cody, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(Cody, InputButton_Dot);

	_float vMayComparePosX = abs(XMVectorGetX(vMayComparePos));
	_float vMayComparePosZ = abs(XMVectorGetZ(vMayComparePos));

	if (fRange >= vMayComparePosX && fRange >= vMayComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::May, UI::InputButton_Dot))
			UI_Create(May, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(May, InputButton_Dot);

	return S_OK;
}

HRESULT CTutorialDoor::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CTutorialDoor::Pull_TutorialDoor(_double dTimeDelta)
{
	
}


CTutorialDoor * CTutorialDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTutorialDoor* pInstance = new CTutorialDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTutorialDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTutorialDoor::Clone_GameObject(void * pArg)
{
	CTutorialDoor* pInstance = new CTutorialDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTutorialDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorialDoor::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}