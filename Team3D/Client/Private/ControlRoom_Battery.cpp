#include "stdafx.h"
#include "..\public\ControlRoom_Battery.h"
#include "Cody.h"
#include "May.h"

CControlRoom_Battery::CControlRoom_Battery(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CControlRoom_Battery::CControlRoom_Battery(const CControlRoom_Battery & rhs)
	: CGameObject(rhs)
{
}

HRESULT CControlRoom_Battery::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControlRoom_Battery::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Battery"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.019f, 221.12184f, 224.74f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(25.f));
	m_fAngle = 25.f;
	
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);
	m_UserData = USERDATA(GameID::eCONTROLROOMBATTERY, this);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

_int CControlRoom_Battery::Tick(_double dTimeDelta)
{
 	CGameObject::Tick(dTimeDelta);
	
	m_pStaticActorCom->Update_StaticActor();
	m_pTriggerCom->Update_TriggerActor();

	//_vector vPos = DATABASE->GetCody()->Get_Position();
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, DATABASE->GetCody()->Get_Position());
	////m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(46.138f, 220.669f, 223.875, 1.f));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.469f, 221.12184f, 223.58f, 1.f));

	///* Å×½ºÆ® */
	//if (m_pGameInstance->Key_Pressing(DIK_K))
	//{
	//	m_fTestAngle += dTimeDelta * 10.f;
	//	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fTestAngle));
	//}

	//if (m_pGameInstance->Key_Pressing(DIK_L))
	//{
	//	m_fTestAngle += dTimeDelta * 10.f;
	//	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-m_fTestAngle));
	//}

	//if (m_pGameInstance->Key_Down(DIK_J))
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, DATABASE->GetCody()->Get_Position());

	return NO_EVENT;
}

_int CControlRoom_Battery::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CControlRoom_Battery::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CControlRoom_Battery::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CControlRoom_Battery::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CControlRoom_Battery * CControlRoom_Battery::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControlRoom_Battery* pInstance = new CControlRoom_Battery(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControlRoom_Battery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CControlRoom_Battery::Clone_GameObject(void * pArg)
{
	CControlRoom_Battery* pInstance = new CControlRoom_Battery(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControlRoom_Battery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlRoom_Battery::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}