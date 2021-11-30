#include "stdafx.h"
#include "..\public\RotatedRobotParts.h"
#include "RotatedRobotBody.h"
#include "RotatedRobotHead.h"
#include "RotatedRobotLever.h"
#include "RotatedRobotBattery.h"
#include "RotatedNoBatterySign.h"

CRotatedRobotParts::CRotatedRobotParts(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRotatedRobotParts::CRotatedRobotParts(const CRotatedRobotParts& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRotatedRobotParts::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	
	return S_OK;
}

HRESULT CRotatedRobotParts::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_tRtRobotDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));
	
	switch (m_tRtRobotDesc.iStageNum)
	{
	case ST_PINBALL:
		DATABASE->Set_STPinBallRobotPtr(this);
		break;
	}

	CGameObject* pRobotBody = nullptr;
	CGameObject* pRobotHead = nullptr;
	CGameObject* pRobotLever = nullptr;
	CGameObject* pRobotBattery = nullptr;
	CGameObject* pNoBatterySign = nullptr;
	
	m_tRtRobotDesc.RotX = 0.f;
	m_tRtRobotDesc.RotY = 90.f;
	m_tRtRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RotatedRobotBody"), Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotBody"), &m_tRtRobotDesc, &pRobotBody), E_FAIL);

	m_tRtRobotDesc.RotX = 0.f;
	m_tRtRobotDesc.RotY = 0.f;
	m_tRtRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RotatedRobotHead"), Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotHead"), &m_tRtRobotDesc, &pRobotHead), E_FAIL);


	m_tRtRobotDesc.RotX = 0.f;
	m_tRtRobotDesc.RotY = 0.f;
	m_tRtRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RotatedRobotBattery"), Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotBattery"), &m_tRtRobotDesc, &pRobotBattery), E_FAIL);

	m_tRtRobotDesc.RotX = 0.f;
	m_tRtRobotDesc.RotY = 0.f;
	m_tRtRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RotatedRobotLever"), Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotLever"), &m_tRtRobotDesc, &pRobotLever), E_FAIL);


	m_tRtRobotDesc.RotX = 0.f;
	m_tRtRobotDesc.RotY = 0.f;
	m_tRtRobotDesc.RotZ = 0.f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RotatedNoBatterySign"), Level::LEVEL_STAGE, TEXT("GameObject_RotatedNoBatterySign"), &m_tRtRobotDesc, &pNoBatterySign), E_FAIL);

	m_pRTRobotBody = static_cast<CRotatedRobotBody*>(pRobotBody);
	m_pRTRobotHead = static_cast<CRotatedRobotHead*>(pRobotHead);
	m_pRTRobotLever = static_cast<CRotatedRobotLever*>(pRobotLever);
	m_pRTRobotBattery = static_cast<CRotatedRobotBattery*>(pRobotBattery);
	m_pRTNoBatterySign = static_cast<CRotatedNoBatterySign*>(pNoBatterySign);
	

	return S_OK;
}

_int CRotatedRobotParts::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CRotatedRobotParts::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CRotatedRobotParts::Render(RENDER_GROUP::Enum eGroup)
{
	return CGameObject::Render(eGroup);
}

HRESULT CRotatedRobotParts::Render_ShadowDepth()
{
	return CGameObject::Render_ShadowDepth();
}

void CRotatedRobotParts::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

CRotatedRobotParts * CRotatedRobotParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedRobotParts* pInstance = new CRotatedRobotParts(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - RotatedRobotParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedRobotParts::Clone_GameObject(void * pArg)
{
	CRotatedRobotParts* pInstance = new CRotatedRobotParts(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedRobotParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedRobotParts::Free()
{
	Safe_Release(m_pRTRobotBody);
	Safe_Release(m_pRTRobotBattery);
	Safe_Release(m_pRTRobotLever);
	Safe_Release(m_pRTNoBatterySign);
	Safe_Release(m_pRTRobotHead);

	CGameObject::Free();
}