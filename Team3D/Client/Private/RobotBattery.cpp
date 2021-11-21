#include "stdafx.h"
#include "..\public\RobotBattery.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"

CRobotBattery::CRobotBattery(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRobotBattery::CRobotBattery(const CRobotBattery & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobotBattery::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();
	return S_OK;
}

HRESULT CRobotBattery::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RobotBattery"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 10.f, 1.f));

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = &PxSphereGeometry(1.5f);
	m_UserData = USERDATA(GameID::eROBOTBATTERY, this);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);

	return S_OK;
}

_int CRobotBattery::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_IsCollide && m_pGameInstance->Key_Down(DIK_E))
	{
		m_bRotate = true;
		UI_Delete(Cody, InputButton_InterActive);
	}

	if (m_bRotate == true)
	{
		Push_Battery(dTimeDelta);
	}

	return NO_EVENT;
}

_int CRobotBattery::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CRobotBattery::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CRobotBattery::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTBATTERY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollide = false;
		UI_Delete(Cody, InputButton_InterActive);
	}

	// May

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTBATTERY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
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

HRESULT CRobotBattery::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRobotBattery::Push_Battery(_double dTimeDelta)
{
	// 나중에 스테이지에 뜨ㅏㅣ웠을때 회전하는 축 바꿔야함.
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f , 0.f), dTimeDelta);
}

CRobotBattery * CRobotBattery::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobotBattery* pInstance = new CRobotBattery(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRobotBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobotBattery::Clone_GameObject(void * pArg)
{
	CRobotBattery* pInstance = new CRobotBattery(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobotBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobotBattery::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}