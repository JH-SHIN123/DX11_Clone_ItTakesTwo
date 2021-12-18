#include "stdafx.h"
#include "LightUtility.h"
#include "VolumeLight.h"

HRESULT CLightUtility::Load_StaticLightData(LOAD_LIGHTTYPE eLoadType, const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	vector<CLight*> tempLights;
	CLight* pLight = nullptr;

	LIGHT_DESC lightDesc;
	CVolumeLight::VOLUMELIGHT_DESC volumeLightDesc;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	while (true)
	{
		switch (eLoadType)
		{
		case LOAD_BASICLIGHT:
			ReadFile(hFile, &lightDesc, sizeof(lightDesc), &dwByte, nullptr);
			break;
		case LOAD_VOLUMELIGHT:
			ReadFile(hFile, &volumeLightDesc, sizeof(volumeLightDesc), &dwByte, nullptr);
			break;
		case LOAD_EFFECTLIGHT:
			break;
		}

		if (0 == dwByte)
			break;

		switch (eLoadType)
		{
		case LOAD_BASICLIGHT:
			pLight = CLight::Create(TEXT("Static_Basic"), &lightDesc);
			break;
		case LOAD_VOLUMELIGHT:
			pLight = (CLight*)CVolumeLight::Create(TEXT("Static_Volume"), &volumeLightDesc);
			break;
		case LOAD_EFFECTLIGHT:
			break;
		}

		// Add_Light
		FAILED_CHECK_RETURN(pGameInstance->Add_Light(LightStatus::eSTATIC, pLight), E_FAIL);
	}

	CloseHandle(hFile);

    return S_OK;
}
