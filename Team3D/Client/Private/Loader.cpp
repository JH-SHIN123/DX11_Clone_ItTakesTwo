#include "stdafx.h"
#include "..\Public\Loader.h"

IMPLEMENT_SINGLETON(CLoader)

CLoader::CLoader()
{
}

HRESULT CLoader::LoadData_UI()
{
	//HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (INVALID_HANDLE_VALUE == hFile)
	//{
	//	MSG_BOX("Failed to Load_PassData");
	//	CloseHandle(hFile);
	//	return E_FAIL;
	//}

	//vector<PSDATA_UI> vecPSData;

	//DWORD dwByte = 0;
	//PSDATA_UI psDataElement;

	//while (true)
	//{
	//	ReadFile(hFile, &psDataElement, sizeof(PSDATA_UI), &dwByte, nullptr);

	//	if (0 == dwByte)
	//		break;

	//	vecPSData.emplace_back(psDataElement);
	//}

	//for (auto PSData : vecPSData)
	//{
	//	CUIObject::UI_DESC		UIDesc;
	//	UIDesc.psData.iRenderGroup = PSData.iRenderGroup;
	//	UIDesc.psData.iLevelIndex = PSData.iLevelIndex;
	//	UIDesc.psData.iTextureLevelIndex = PSData.iTextureLevelIndex;
	//	lstrcpyW(UIDesc.psData.szTextureTag, PSData.szTextureTag);
	//	lstrcpyW(UIDesc.psData.szUITag, PSData.szUITag);
	//	UIDesc.psData.vPos = { PSData.vPos.x ,  PSData.vPos.y };
	//	UIDesc.psData.vScale = { PSData.vScale.x , PSData.vScale.y };

	//	if (FAILED(pGameInstance->Add_GameObject(UIDesc.psData.iLevelIndex, PSData.szUITag,
	//		UIDesc.psData.iLevelIndex, TEXT("Layer_UI"), &UIDesc)))
	//		return E_FAIL;
	//}

	//CloseHandle(hFile);

	return S_OK;
}

void CLoader::Free()
{
}
