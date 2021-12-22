#include "stdafx.h"
#include "..\Public\EndingCredit_Manager.h"
#include "C3DText.h"
#include "Cody.h"

IMPLEMENT_SINGLETON(CEndingCredit_Manager)

CEndingCredit_Manager::CEndingCredit_Manager()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEndingCredit_Manager::NativeConstruct_EndingCredit()
{
	return S_OK;
}

HRESULT CEndingCredit_Manager::Create_3DText(_uint iIndex, _float fPosY)
{
	if (23 < iIndex)
		return S_OK;

	if (0 == iIndex || 10 == iIndex || 16 == iIndex || 23 == iIndex)
		FAILED_CHECK_RETURN(Create_HugeRock(), E_FAIL);

	C3DText::ARG_DESC tArg;
	tArg.iIndex = iIndex;
	tArg.fPosY = fPosY;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_3DText"), &tArg), E_FAIL);

	return S_OK;
}

HRESULT CEndingCredit_Manager::Create_Environment()
{
	/* 돌 생성 */
	FAILED_CHECK_RETURN(Create_Rocks(50), E_FAIL);

	/* 2DMesh 생성 */
	FAILED_CHECK_RETURN(Create_2DMesh(20), E_FAIL);

	return S_OK;
}

HRESULT CEndingCredit_Manager::Create_Rocks(_uint iNumRock)
{
	for (_uint i = 0; i < iNumRock; ++i)
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_Rock")), E_FAIL);

	return S_OK;
}

HRESULT CEndingCredit_Manager::Create_HugeRock()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_HugeRock")), E_FAIL);

	return S_OK;
}

HRESULT CEndingCredit_Manager::Create_2DMesh(_uint iNum2DMesh)
{
	for (_uint i = 0; i < iNum2DMesh; ++i)
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_2DMesh")), E_FAIL);

	return S_OK;
}

void CEndingCredit_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

//////////HRESULT CEndingCredit_Manager::Create_3DText(_bool bBoost)
//////////{
//////////	if (23 < m_iTextIndex)
//////////		return S_OK;
//////////
//////////	if (0 == m_iTextIndex || 10 == m_iTextIndex || 16 == m_iTextIndex || 23 == m_iTextIndex)
//////////		FAILED_CHECK_RETURN(Create_HugeRock(), E_FAIL);
//////////
//////////	if (true == m_bDeadEnvironment)
//////////	{
//////////		Create_Environment();
//////////		m_bDeadEnvironment = false;
//////////	}
//////////
//////////	++m_iRandomModel;
//////////	if (4 < m_iRandomModel)
//////////		m_iRandomModel = 0;
//////////
//////////	C3DText::ARG_DESC tArg;
//////////	tArg.IsBoost = bBoost;
//////////	tArg.iIndex = m_iTextIndex;
//////////	wstring strModelTag = TEXT("Component_Model_3DText");
//////////	strModelTag += to_wstring(m_iTextIndex);
//////////	tArg.szModelTag = strModelTag.c_str();
//////////
//////////	Add_Argument_Info(m_iTextIndex, tArg);
//////////
//////////	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_3DText"), &tArg), E_FAIL);
//////////
//////////	++m_iTextIndex;
//////////
//////////	return S_OK;
//////////}

//////////void CEndingCredit_Manager::Add_Argument_Info(_uint iIndex, C3DText::ARG_DESC & tArg)
//////////{
//////////	/* 글자 생성 시간 */
//////////	if (0 == iIndex)
//////////		tArg.fTime = 5.f;
//////////	else if (9 >= iIndex)
//////////		tArg.fTime = 5.f;
//////////	else if (15 >= iIndex)
//////////		tArg.fTime = 5.f;
//////////	else if (16 >= iIndex)
//////////		tArg.fTime = 5.f;
//////////	else if (22 >= iIndex)
//////////		tArg.fTime = 5.f;
//////////	else
//////////		tArg.fTime = 5.f;
//////////
//////////	/* 글자 트리거 사이즈, 최대 사이즈 */
//////////	switch (iIndex)
//////////	{
//////////	case 0:
//////////		tArg.vTriggerSize = _float3(4.72f, 1.f, 0.65f);
//////////		tArg.fMaxScale = 10.f;
//////////		break;
//////////	case 1:
//////////		tArg.vTriggerSize = _float3(6.02f, 1.f, 2.74f);
//////////		break;
//////////	case 2:
//////////		tArg.vTriggerSize = _float3(3.43f, 1.f, 1.76f);
//////////		break;
//////////	case 3:
//////////		tArg.vTriggerSize = _float3(6.11f, 1.f, 1.77f);
//////////		break;
//////////	case 4:
//////////		tArg.vTriggerSize = _float3(2.31f, 1.f, 1.76f);
//////////		break;
//////////	case 5:
//////////		tArg.vTriggerSize = _float3(2.32f, 1.f, 1.74f);
//////////		break;
//////////	case 6:
//////////		tArg.vTriggerSize = _float3(9.98f, 1.f, 1.74f);
//////////		break;
//////////	case 7:
//////////		tArg.vTriggerSize = _float3(3.11f, 1.f, 1.74f);
//////////		break;
//////////	case 8:
//////////		tArg.vTriggerSize = _float3(6.55f, 1.f, 4.17f);
//////////		break;
//////////	case 9:
//////////		tArg.vTriggerSize = _float3(2.79f, 1.f, 2.75f);
//////////		break;
//////////	case 10:
//////////		tArg.vTriggerSize = _float3(7.11f, 1.f, 1.58f);
//////////		tArg.fMaxScale = 10.f;
//////////		break;
//////////	case 11:
//////////		tArg.vTriggerSize = _float3(10.07f, 1.f, 1.77f);
//////////		break;
//////////	case 12:
//////////		tArg.vTriggerSize = _float3(14.54f, 1.f, 4.75f);
//////////		break;
//////////	case 13:
//////////		tArg.vTriggerSize = _float3(5.66f, 1.f, 1.74f);
//////////		break;
//////////	case 14:
//////////		tArg.vTriggerSize = _float3(8.02f, 1.f, 1.77f);
//////////		break;
//////////	case 15:
//////////		tArg.vTriggerSize = _float3(7.9f, 1.f, 1.74f);
//////////		break;
//////////	case 16:
//////////		tArg.vTriggerSize = _float3(7.1f, 1.f, 0.66f);
//////////		tArg.fMaxScale = 10.f;
//////////		break;
//////////	case 17:
//////////		tArg.vTriggerSize = _float3(7.88f, 1.f, 1.84f);
//////////		break;
//////////	case 18:
//////////		tArg.vTriggerSize = _float3(10.4f, 1.f, 1.83f);
//////////		break;
//////////	case 19:
//////////		tArg.vTriggerSize = _float3(13.55f, 1.f, 2.85f);
//////////		break;
//////////	case 20:
//////////		tArg.vTriggerSize = _float3(13.17f, 1.f, 2.83f);
//////////		break;
//////////	case 21:
//////////		tArg.vTriggerSize = _float3(10.43f, 1.f, 2.85f);
//////////		break;
//////////	case 22:
//////////		tArg.vTriggerSize = _float3(13.66f, 1.f, 1.84f);
//////////		break;
//////////	case 23:
//////////		tArg.vTriggerSize = _float3(4.25f, 1.f, 0.83f);
//////////		tArg.fMaxScale = 10.f;
//////////		break;
//////////	}
//////////	tArg.fMaxScale = 8.f;
//////////}