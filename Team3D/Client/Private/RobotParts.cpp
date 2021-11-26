#include "stdafx.h"
#include "..\public\RobotParts.h"
#include "GameInstance.h"

CRobotParts::CRobotParts(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRobotParts::CRobotParts(const CRobotParts& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobotParts::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();


	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotBody") , Level::LEVEL_STAGE, TEXT("GameObject_Robot")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotHead"), Level::LEVEL_STAGE, TEXT("GameObject_RobotHead")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotLever"), Level::LEVEL_STAGE, TEXT("GameObject_RobotLever")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotBattery"), Level::LEVEL_STAGE, TEXT("GameObject_RobotBattery")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_NoBatterySign"), Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign")), E_FAIL);
	
	return S_OK;
}

HRESULT CRobotParts::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int CRobotParts::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CRobotParts::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CRobotParts::Render(RENDER_GROUP::Enum eGroup)
{
	return CGameObject::Render(eGroup);
}

HRESULT CRobotParts::Render_ShadowDepth()
{
	return CGameObject::Render_ShadowDepth();
}



void CRobotParts::Free()
{
	CGameObject::Free();
}