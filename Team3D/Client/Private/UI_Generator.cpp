#include "stdafx.h"
#include "..\Public\UI_Generator.h"

#include "GameInstance.h"
#include "InputButton_Frame.h"
#include "InputButton.h"
#include "PC_MouseButton.h"
#include "PlayerMarker.h"
#include "Arrowkeys_Outline.h"
#include "Arrowkeys_Fill.h"

IMPLEMENT_SINGLETON(CUI_Generator)

CUI_Generator::CUI_Generator()
{
}

HRESULT CUI_Generator::NativeConstruct(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDevice_Context, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDevice_Context;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	if (FAILED(Add_Prototype_Texture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Generator::Load_Data(const _tchar * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to LoadData UI, Error to CUI_Generator::Load_Data");
		CloseHandle(hFile);
		return E_FAIL;
	}

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

		if (FAILED(Add_Prototype_Fixed_UI(PSData)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Generator::Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (false == m_IsTrigger || ePlayer >= Player::PLAYER_END || eTrigger >= UI::TRIGGER_END)
		return S_OK;

	switch (eTrigger)
	{
	case UI::InputButton_Dot:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Dot"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"));
	    break;  
	case UI::InputButton_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_F"));
		break;
	case UI::InputButton_PS_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Triangle"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Triangle"));
		break;
	case UI::PC_Mouse_Reduction:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Reduction"));
		break;
	case UI::PC_Mouse_Enlargement:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Enlargement"));
		break;
	case UI::InputButton_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Cancle"));
		break;
	case UI::InputButton_PS_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Cancle"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Cancle"));
		break;
	case UI::InputButton_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Up"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Up"));
		break;
	case UI::InputButton_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Down"));
		break;
	case UI::InputButton_PS_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Up"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Up"));
		break;
	case UI::InputButton_PS_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Down"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Down"));
		break;
	case UI::PlayerMarker:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PlayerMarker"));
		break;
	case UI::InputButton_PS_R1:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R1"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R1"));
		break;
	case UI::Arrowkeys_Side:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Left"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Right"));
		break;
	case UI::Arrowkeys_UpDown:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Up"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Down"));
		break;
	case UI::InputButton_PS_L2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_L2"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_L2"));
		break;
	case UI::InputButton_PS_R2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R2"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R2"));
		break;
	default:
		MSG_BOX("UI Trigger does not exist, Error to CUI_Generator::Generator_UI");
		break;
	}

	m_IsTrigger = false;

	return S_OK;
}

HRESULT CUI_Generator::Delete_UI(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
 		return S_OK;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
	{
		UIObject->Set_Dead();
		Safe_Release(UIObject);
	}

	m_vecUIOBjects[ePlayer][eTrigger].clear();

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

	if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Dot"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_F"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Dot"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_Triangle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_Triangle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"PlayerMarker"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPlayerMarker::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_R1"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_R1"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}


	return S_OK;
}


HRESULT CUI_Generator::Add_Prototype_Fixed_UI(CUIObject::UI_DESC* UIDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	if (!lstrcmp(UIDesc->szUITag, L"PC_Mouse_Reduction"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPC_MouseButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"PC_Mouse_Enlargement"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPC_MouseButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Cancle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_Cancle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_Cancle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_Up"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_Up"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_Down"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_Down"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Up"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Up"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Down"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Arrowkeys_Outline"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CArrowkeys_Outline::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Arrowkeys_Fill_Up"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CArrowkeys_Fill::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Arrowkeys_Fill_Right"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CArrowkeys_Fill::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Arrowkeys_Fill_Down"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CArrowkeys_Fill::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Arrowkeys_Fill_Left"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CArrowkeys_Fill::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_L2"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_PS_R2"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_L2"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_PS_R2"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}

	return S_OK;
}

HRESULT CUI_Generator::Add_Prototype_Texture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("InputButton_Frame"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/InputButton_Frame%d.png"), 6)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("InputButton"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/InputButton%d.png"), 10)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("PlayerMarker"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/ActorMarker/PlayerMarker.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("PC_Mouse"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/PC_Mouse%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("PC_Mouse_SubTexture"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/PC_Mouse_SubTexture.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Arrowkeys_Outline"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/Arrowkeys_Outline.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Arrowkeys_Fill"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/Arrowkeys_Fill.png"))), E_FAIL);

	return S_OK;
}

HRESULT CUI_Generator::SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, PrototypeTag, nullptr, &pGameObject), E_FAIL);
	pUIObject = static_cast<CUIObject*>(pGameObject);
	pUIObject->Set_PlayerID(ePlayer);
	m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);

	return S_OK;
}


void CUI_Generator::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	for (auto PSData : m_vecPSData)
		Safe_Delete(PSData);

	m_vecPSData.clear();

	for (_uint i = 0; i < Player::PLAYER_END; ++i)
	{
		for (_uint j = 0; j < UI::TRIGGER_END; ++j)
		{
			if (0 != m_vecUIOBjects[i][j].size())
			{
				for (auto UIObject : m_vecUIOBjects[i][j])
					Safe_Release(UIObject);

				m_vecUIOBjects[i][j].clear();
			}
		}
	}
}
