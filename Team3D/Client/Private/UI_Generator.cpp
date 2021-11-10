#include "stdafx.h"
#include "..\Public\UI_Generator.h"

#include "GameInstance.h"
#include "InputButton_Frame.h"
#include "InputButton.h"

IMPLEMENT_SINGLETON(CUI_Generator)

CUI_Generator::CUI_Generator()
{
}

HRESULT CUI_Generator::Set_Device(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDevice_Context, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDevice_Context;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
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

	if (FAILED(Add_Prototype_Texture()))
		return E_FAIL;

	DWORD dwByte = 0;
	
	while (true)
	{
		CUIObject::UI_DESC* psDataElement = new CUIObject::UI_DESC;

		ReadFile(hFile, psDataElement, sizeof(CUIObject::UI_DESC), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(psDataElement);
			break;
		}

		m_vecPSData.emplace_back(psDataElement);
	}

	for (auto PSData : m_vecPSData)
	{
		if (FAILED(Add_Prototype_Interactive_UI(PSData)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Generator::Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (false == m_IsTrigger)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	switch (eTrigger)
	{
	case UI::InputButton_Dot:
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_UI"), Level::LEVEL_STAGE, TEXT("InputButton_Dot"), nullptr, &pGameObject), E_FAIL);
		pUIObject = static_cast<CUIObject*>(pGameObject);
		m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_UI"), Level::LEVEL_STAGE, TEXT("InputButton_Frame_Circle"), nullptr, &pGameObject), E_FAIL);
		pUIObject = static_cast<CUIObject*>(pGameObject);
		m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);
		break;
	case UI::InputButton_F:
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_UI"), Level::LEVEL_STAGE, TEXT("InputButton_Dot"), nullptr, &pGameObject), E_FAIL);
		pUIObject = static_cast<CUIObject*>(pGameObject);
		m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);
		break;
	case UI::TRIGGER_END:
		break;
	default:
		break;
	}

	m_IsTrigger = false;

	return S_OK;
}

void CUI_Generator::Set_TriggerOn()
{
	m_IsTrigger = true;
}

HRESULT CUI_Generator::Add_Prototype_Interactive_UI(CUIObject::UI_DESC* UIDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Circle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Rect"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Dot"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}

	return S_OK;
}


HRESULT CUI_Generator::Add_Prototype_Fixed_UI(CUIObject::UI_DESC UIDesc)
{
	return S_OK;
}

HRESULT CUI_Generator::Add_Prototype_Texture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("InputButton_Frame"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/InputButton_Frame%d.png"), 5)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("InputButton"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/InputButton%d.png"), 4)), E_FAIL);

	return S_OK;
}


void CUI_Generator::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	for (auto PSData : m_vecPSData)
		Safe_Delete(PSData);

	

	m_vecPSData.clear();
}
