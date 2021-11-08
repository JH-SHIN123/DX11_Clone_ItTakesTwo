#include "stdafx.h"
#include "..\Public\UI_Loader.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Loader)

CUI_Loader::CUI_Loader()
{
}

HRESULT CUI_Loader::LoadData_UI(const _tchar* pFilePath)
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

		if (!lstrcmp(UIDesc.szUITag, L"클래스"))
		{
			if (FAILED(pGameInstance->Add_GameObject_Prototype(UIDesc.iLevelIndex, PSData.szUITag,
				클래스::Create))
				return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Loader::Create_UI(UI_TRIGGER eTrigger)
{


	return S_OK;
}

void CUI_Loader::Free()
{
}
