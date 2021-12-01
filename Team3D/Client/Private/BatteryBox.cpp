#include "stdafx.h"
#include "..\public\BatteryBox.h"
#include "Cody.h"
#include "May.h"
#include "ControlRoom_Battery.h"

CBatteryBox::CBatteryBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBatteryBox::CBatteryBox(const CBatteryBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBatteryBox::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBatteryBox::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_BatteryBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.469f, 220.95084f, 224.66f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	FAILED_CHECK_RETURN(Ready_Layer_ControlRoom_Battery(TEXT("Layer_ControlRoom_Battery")), E_FAIL);
	
	m_UserData = USERDATA(GameID::eBATTERYBOX, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CBatteryBox::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	return NO_EVENT;
}

_int CBatteryBox::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBatteryBox::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CBatteryBox::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CBatteryBox::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

_bool CBatteryBox::Get_BatteryHolding()
{
	NULL_CHECK_RETURN(m_pBattery, false);

	return m_pBattery->Get_BatteryHolding();
}

HRESULT CBatteryBox::Ready_Layer_ControlRoom_Battery(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Battery"), nullptr, &pGameObject), E_FAIL);
	m_pBattery = static_cast<CControlRoom_Battery*>(pGameObject);

	return S_OK;
}

CBatteryBox * CBatteryBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBatteryBox* pInstance = new CBatteryBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBatteryBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBatteryBox::Clone_GameObject(void * pArg)
{
	CBatteryBox* pInstance = new CBatteryBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBatteryBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBatteryBox::Free()
{
	Safe_Release(m_pBattery);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}