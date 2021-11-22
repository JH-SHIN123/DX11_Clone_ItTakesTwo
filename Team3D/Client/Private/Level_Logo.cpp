#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Camera.h"

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

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_LOGO, TEXT("Layer_UI"), Level::LEVEL_LOGO, TEXT("SplashScreen")), E_FAIL);

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
