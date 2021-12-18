#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Camera.h"
#include "UI_Generator.h"
#include "DataStorage.h"
#include "MenuScreen.h"
#include "Level_Loading.h"
#include "Return_Button.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::NativeConstruct()
{
	CLevel::NativeConstruct();

	Ready_Layer_SplashScreen();

	return S_OK;
}

_int CLevel_Logo::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* ¾À ÀüÈ¯ */
	if (nullptr != DATABASE->Get_ReturnButton())
	{
		if (true == ((CReturn_Button*)(DATABASE->Get_ReturnButton()))->Get_ChangeScene() && pGameInstance->Key_Down(DIK_RETURN))
		{
			if (FAILED(pGameInstance->Change_CurrentLevel(CLevel_Loading::Create(m_pDevice, m_pDeviceContext, Level::LEVEL_LOGO, Level::LEVEL_STAGE))))
			{
				MSG_BOX("Failed to Change_CurrentLevel, Error to CMenuScreen::Late_Tick");
				return EVENT_ERROR;
			}

			pGameInstance->Clear_LevelResources(Level::LEVEL_LOGO);
			UI_Delete(Default, AlphaScreen);
		}
	}

	return NO_EVENT;
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_SplashScreen()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	_uint iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_LOGO, TEXT("Layer_UI"), Level::LEVEL_LOGO, TEXT("SplashScreen"), &iOption), E_FAIL);
	iOption = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_LOGO, TEXT("Layer_UI"), Level::LEVEL_LOGO, TEXT("SplashScreen"), &iOption), E_FAIL);

	UI_Generator->Create_Logo();

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Create Instance - CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	CLevel::Free();
}
