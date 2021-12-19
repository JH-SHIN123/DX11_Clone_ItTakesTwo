#include "stdafx.h"
#include "..\Public\EndingCredit_Manager.h"
#include <string>
#include "C3DText.h"

IMPLEMENT_SINGLETON(CEndingCredit_Manager)

CEndingCredit_Manager::CEndingCredit_Manager()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEndingCredit_Manager::Create_3DText(_vector vPosition, _bool bBoost)
{
	C3DText::ARG_DESC tArg;
	XMStoreFloat3(&tArg.vPosition, vPosition);
	tArg.IsBoost = bBoost;
	tArg.szModelTag = TEXT("Component_Model_Test");
	//wstring strModelTag = TEXT("Component_Model_3DText");
	//strModelTag += to_wstring(m_iTextIndex);
	//tArg.szModelTag = strModelTag.c_str();

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_3DText"), &tArg), E_FAIL);

	return S_OK;
}

HRESULT CEndingCredit_Manager::NativeConstruct_EndingCredit()
{
	return S_OK;
}

void CEndingCredit_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
