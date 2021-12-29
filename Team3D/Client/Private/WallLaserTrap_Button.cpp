#include "stdafx.h"
#include "..\Public\WallLaserTrap_Button.h"
#include "May.h"
#include "Cody.h"
#include "DataStorage.h"
#include "WallLaserTrap.h"

CWallLaserTrap_Button::CWallLaserTrap_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWallLaserTrap_Button::CWallLaserTrap_Button(const CWallLaserTrap_Button & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWallLaserTrap_Button::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CWallLaserTrap_Button::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	Ready_Component();

	return S_OK;
}

_int CWallLaserTrap_Button::Tick(_double TimeDelta)
{

// 	m_pLaserTrap_Left->Set_Transform(XMVectorSet(-805.32f, 786.f, 191.37f, 1.f));
// 	m_pLaserTrap_Left->Set_Rotate(_float3(90.f, 0.f, 0.f));
// 
// 
// 	m_pLaserTrap_Right->Set_Transform(XMVectorSet(-805.32f, 786.f, 196.06f, 1.f));
// 	m_pLaserTrap_Right->Set_Rotate(_float3(-90.f, 0.f, 0.f));


	m_pLaserTrap_Left->Set_TriggerActivate(m_IsActivate);
	m_pLaserTrap_Right->Set_TriggerActivate(m_IsActivate);

	return _int();
}

_int CWallLaserTrap_Button::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CWallLaserTrap_Button::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);
	
	return S_OK;
}

void CWallLaserTrap_Button::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		m_IsActivate = false;
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		m_IsActivate = true;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		m_IsActivate = false;
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		m_IsActivate = true;
}

HRESULT CWallLaserTrap_Button::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

HRESULT CWallLaserTrap_Button::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Lamp_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	_matrix vPivot = XMMatrixIdentity();
	vPivot *= XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(0.f),
		XMConvertToRadians(0.f),
		XMConvertToRadians(-90.f));
	m_pTransformCom->Set_WorldMatrix(vPivot);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-803.32f, 789.125f, 189.37f, 1.f));//Gara
	m_pTransformCom->Set_Scale(XMVectorSet(12.f, 3.f, 12.f, 0.f));

	m_UserData = USERDATA(GameID::eWALLLASERTRAP_BUTTON, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxBoxGeometry(0.9f, 0.5f, 0.9f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	CStaticActor::ARG_DESC Static_ArgDesc;
	Static_ArgDesc.pModel = m_pModelCom;
	Static_ArgDesc.pTransform = m_pTransformCom;
	Static_ArgDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &Static_ArgDesc), E_FAIL);

	_float4 vPos	= { -805.32f, 786.f, 191.37f, 1.f };
	_float3 vAngle	= { 90.f,0.f,0.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTrap"), Level::LEVEL_STAGE, TEXT("GameObject_WallLaserTrap"), &CWallLaserTrap::tagWallLaserTrap_Desc(vPos, vAngle), (CGameObject**)&m_pLaserTrap_Left), E_FAIL);
	vPos = { -805.32f, 786.f, 196.06f, 1.f };
	vAngle = { -90.f,0.f,0.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTrap"), Level::LEVEL_STAGE, TEXT("GameObject_WallLaserTrap"), &CWallLaserTrap::tagWallLaserTrap_Desc(vPos, vAngle), (CGameObject**)&m_pLaserTrap_Right), E_FAIL);

	return S_OK;
}

CWallLaserTrap_Button * CWallLaserTrap_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWallLaserTrap_Button*	pInstance = new CWallLaserTrap_Button(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWallLaserTrap_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWallLaserTrap_Button::Clone_GameObject(void * pArg)
{
	CWallLaserTrap_Button* pInstance = new CWallLaserTrap_Button(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWallLaserTrap_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWallLaserTrap_Button::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerCom);

	Safe_Release(m_pLaserTrap_Left);
	Safe_Release(m_pLaserTrap_Right);

	__super::Free();
}
