#include "stdafx.h"
#include "..\public\Level_Loading.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Stage.h"
#include "UI_Generator.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Loading::NativeConstruct(Level::ID ePreLevelID, Level::ID eNextLevelID)
{
	CLevel::NativeConstruct();

	m_ePreLevelID	= ePreLevelID;
	m_eNextLevelID	= eNextLevelID;

	m_pLoading = CLoading::Create(m_pDevice, m_pDeviceContext, ePreLevelID, eNextLevelID);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

_int CLevel_Loading::Tick(_double dTimedelta)
{
	NULL_CHECK_RETURN(m_pGameInstance, EVENT_ERROR);

	CLevel::Tick(dTimedelta);

	if (true == m_pLoading->Is_FinishedToLoading())
	{
		CLevel* pLevel = nullptr;

		switch (m_eNextLevelID)
		{
		case Level::LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
			break;
		case Level::LEVEL_STAGE:
			pLevel = CLevel_Stage::Create(m_pDevice, m_pDeviceContext);
			break;
		}

		NULL_CHECK_RETURN(pLevel, EVENT_ERROR);

		m_pGameInstance->Change_CurrentLevel(pLevel);
		//m_pGameInstance->Clear_LevelResources(m_ePreLevelID);
		//m_pGameInstance->Clear_Level(Level::LEVEL_LOADING);

	}

	return NO_EVENT;
}

HRESULT CLevel_Loading::Render()
{
	CLevel::Render();

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(ePreLevelID, eNextLevelID)))
	{
		MSG_BOX("Failed to Create Instance - CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	Safe_Release(m_pLoading);

	CLevel::Free();
}
