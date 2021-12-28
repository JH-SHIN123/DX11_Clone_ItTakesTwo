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

	if (m_eNextLevelID == Level::LEVEL_STAGE)
	{
		UI_Create(Default, LoadingScreen);
		UI_Create(Default, LoadingBook);
	}

	m_pFont = (CFontDraw*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("Component_FontDraw"));

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
		UI_Delete(Default, LoadingScreen);
		UI_Delete(Default, LoadingBook);

	}

	return NO_EVENT;
}

HRESULT CLevel_Loading::Render()
{
	CLevel::Render();

	m_pFont->Render_Font(TEXT("불러오는 중"), _float2(1120.f, 675.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.35f);

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
	Safe_Release(m_pFont);
	Safe_Release(m_pLoading);

	CLevel::Free();
}