#include "stdafx.h"
#include "..\Public\UI_Generator.h"

#include "GameInstance.h"
#include "Ortho_UIObject.h"

IMPLEMENT_SINGLETON(CUI_Generator)

CUI_Generator::CUI_Generator()
{
}

HRESULT CUI_Generator::Load_Data(const _tchar * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to LoadData UI");
		CloseHandle(hFile);
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

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
		UIDesc.iLevelIndex = PSData.iLevelIndex;
		UIDesc.iRenderGroup = PSData.iRenderGroup;
		UIDesc.iTextureLevelIndex = PSData.iTextureLevelIndex;
		UIDesc.iTextureRenderIndex = PSData.iTextureRenderIndex;
		UIDesc.iSubTextureNum = PSData.iSubTextureNum;
		lstrcpyW(UIDesc.szSubTextureTag, PSData.szSubTextureTag);
		lstrcpyW(UIDesc.szTextureTag, PSData.szTextureTag);
		lstrcpyW(UIDesc.szUITag, PSData.szUITag);
		UIDesc.vPos = { PSData.vPos.x ,  PSData.vPos.y };
		UIDesc.vScale = { PSData.vScale.x , PSData.vScale.y };

		if (!lstrcmp(UIDesc.szUITag, L"InputButton_Frame"))
		{

		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Generator::Generator_UI(UI::TRIGGER eTrigger)
{
	return S_OK;
}


void CUI_Generator::Free()
{
}
