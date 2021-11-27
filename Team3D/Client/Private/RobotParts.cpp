#include "stdafx.h"
#include "..\public\RobotParts.h"
#include "Robot.h"
#include "RobotHead.h"
#include "RobotLever.h"
#include "RobotBattery.h"
#include "NoBatterySign.h"

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

	
	return S_OK;
}

HRESULT CRobotParts::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_tRobotDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	
	switch (m_tRobotDesc.iStageNum)
	{
	case ST_GRAVITYPATH:
		DATABASE->Set_STGravityRobotPtr(this);
		break;
	case ST_PINBALL:
		DATABASE->Set_STPinBallRobotPtr(this);
		break;
	case ST_RAIL:
		DATABASE->Set_STPlanetRobotPtr(this);
		break;
	}

	CGameObject* pRobotBody = nullptr;
	CGameObject* pRobotHead = nullptr;
	CGameObject* pRobotLever = nullptr;
	CGameObject* pRobotBattery = nullptr;
	CGameObject* pNoBatterySign = nullptr;
	
	m_tRobotDesc.RotX = 0.f;
	m_tRobotDesc.RotY = 90.f;
	m_tRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotBody"), Level::LEVEL_STAGE, TEXT("GameObject_Robot"), &m_tRobotDesc, &pRobotBody), E_FAIL);

	m_tRobotDesc.RotX = 0.f;
	m_tRobotDesc.RotY = 0.f;
	m_tRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotHead"), Level::LEVEL_STAGE, TEXT("GameObject_RobotHead"), &m_tRobotDesc, &pRobotHead), E_FAIL);

	m_tRobotDesc.RotX = 0.f;
	m_tRobotDesc.RotY = 0.f;
	m_tRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotLever"), Level::LEVEL_STAGE, TEXT("GameObject_RobotLever"), &m_tRobotDesc, &pRobotLever), E_FAIL);

	m_tRobotDesc.RotX = 0.f;
	m_tRobotDesc.RotY = 0.f;
	m_tRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RobotBattery"), Level::LEVEL_STAGE, TEXT("GameObject_RobotBattery"), &m_tRobotDesc, &pRobotBattery), E_FAIL);

	m_tRobotDesc.RotX = 0.f;
	m_tRobotDesc.RotY = 0.f;
	m_tRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_NoBatterySign"), Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign"), &m_tRobotDesc, &pNoBatterySign), E_FAIL);

	m_pRobotBody = static_cast<CRobot*>(pRobotBody);
	m_pRobotHead = static_cast<CRobotHead*>(pRobotHead);
	m_pRobotLever = static_cast<CRobotLever*>(pRobotLever);
	m_pRobotBattery = static_cast<CRobotBattery*>(pRobotBattery);
	m_pNoBatterySign = static_cast<CNoBatterySign*>(pNoBatterySign);
	

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

void CRobotParts::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

CRobotParts * CRobotParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobotParts* pInstance = new CRobotParts(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - RobotParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobotParts::Clone_GameObject(void * pArg)
{
	CRobotParts* pInstance = new CRobotParts(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobotParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobotParts::Free()
{
	Safe_Release(m_pRobotBattery);
	Safe_Release(m_pRobotLever);
	Safe_Release(m_pRobotBody);
	Safe_Release(m_pNoBatterySign);
	Safe_Release(m_pRobotHead);

	CGameObject::Free();
}