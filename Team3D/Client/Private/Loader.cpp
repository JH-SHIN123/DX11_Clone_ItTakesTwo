#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Ortho_UIObject.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLoader)

CLoader::CLoader()
{
}

HRESULT CLoader::LoadData_UI(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load_PassData");
		CloseHandle(hFile);
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	vector<COrtho_UIObject::UI_DESC> vecPSData;

	DWORD dwByte = 0;
	COrtho_UIObject::UI_DESC psDataElement;

	while (true)
	{
		ReadFile(hFile, &psDataElement, sizeof(COrtho_UIObject::UI_DESC), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		vecPSData.emplace_back(psDataElement);
	}

	for (auto PSData : vecPSData)
	{
		COrtho_UIObject::UI_DESC		UIDesc;
		UIDesc.iRenderGroup = PSData.iRenderGroup;
		UIDesc.iLevelIndex = PSData.iLevelIndex;
		UIDesc.iTextureLevelIndex = PSData.iTextureLevelIndex;
		lstrcpyW(UIDesc.szTextureTag, PSData.szTextureTag);
		lstrcpyW(UIDesc.szUITag, PSData.szUITag);
		UIDesc.vPos = { PSData.vPos.x ,  PSData.vPos.y };
		UIDesc.vScale = { PSData.vScale.x , PSData.vScale.y };

		if (FAILED(pGameInstance->Add_GameObject_Clone(UIDesc.iLevelIndex, PSData.szUITag,
			UIDesc.iLevelIndex, TEXT("Layer_UI"), &UIDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

void CLoader::Free()
{
}
