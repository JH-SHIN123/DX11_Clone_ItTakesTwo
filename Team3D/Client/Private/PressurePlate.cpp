#include "stdafx.h"
#include "..\public\PressurePlate.h"
#include "Cody.h"
#include "May.h"

CPressurePlate::CPressurePlate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPressurePlate::CPressurePlate(const CPressurePlate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPressurePlate::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPressurePlate::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.7f);
	m_UserData = USERDATA(GameID::ePRESSUREPLATE, this);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);
	//DATABASE->Set_PressurePlatePtr(this);

	return S_OK;
}

_int CPressurePlate::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	//if (m_bUpdate == true)
	//{
	//	if (m_IsCollide && m_pGameInstance->Key_Down(DIK_E))
	//	{
	//		UI_Delete(Cody, InputButton_InterActive);
	//		UI_Delete(May, InputButton_InterActive);
	//		m_IsCollide = false;
	//		m_bRotate = true;
	//	}

	//	if (m_bRotate == true)
	//	{
	//		Activate_Lever(dTimeDelta);
	//	}
	//}

	return NO_EVENT;
}

_int CPressurePlate::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPressurePlate::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPressurePlate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	//// Cody
	//if (m_bUpdate == true)
	//{
	//	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	//	{
	//		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePressurePlate, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//		UI_Create(Cody, InputButton_InterActive);
	//		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//		m_IsCollide = true;
	//	}
	//	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	//	{
	//		m_IsCollide = false;
	//		UI_Delete(Cody, InputButton_InterActive);
	//	}

	//	// May

	//	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	//	{
	//		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::ePressurePlate, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//		UI_Create(May, InputButton_InterActive);
	//		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//		m_IsCollide = true;
	//	}
	//	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	//	{
	//		m_IsCollide = false;
	//		UI_Delete(May, InputButton_InterActive);
	//	}
	//}
}

HRESULT CPressurePlate::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CPressurePlate * CPressurePlate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPressurePlate* pInstance = new CPressurePlate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPressurePlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPressurePlate::Clone_GameObject(void * pArg)
{
	CPressurePlate* pInstance = new CPressurePlate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPressurePlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPressurePlate::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}