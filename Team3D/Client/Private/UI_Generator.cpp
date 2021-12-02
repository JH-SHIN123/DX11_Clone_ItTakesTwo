#include "stdafx.h"
#include "..\Public\UI_Generator.h"

#include "GameInstance.h"
#include "InputButton_Frame.h"
#include "InputButton.h"
#include "PC_MouseButton.h"
#include "PlayerMarker.h"
#include "Arrowkeys_Outline.h"
#include "Arrowkeys_Fill.h"
#include "UISprite.h"

IMPLEMENT_SINGLETON(CUI_Generator)

CUI_Generator::CUI_Generator()
{
}

HRESULT CUI_Generator::NativeConstruct(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDevice_Context, E_FAIL);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDevice_Context;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	if (FAILED(Add_Prototype_Texture()))
		return E_FAIL;

	m_pTexturesCom = (CTextures*)pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("Font"));
	m_pEngTexturesCom = (CTextures*)pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("EngFont"));
	m_pVIBuffer_FontCom = (CVIBuffer_FontInstance*)pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_FontInstance"));
	
	m_VTXFONT = new VTXFONT[50];

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

HRESULT CUI_Generator::Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger, _bool bActive)
{
	if (false == m_IsTrigger || ePlayer >= Player::PLAYER_END || eTrigger >= UI::TRIGGER_END)
		return S_OK;

	switch (eTrigger)
	{
	case UI::InputButton_Dot:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Dot"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), bActive);
	    break;  
	case UI::InputButton_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_F"), bActive);
		break;
	case UI::InputButton_PS_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Triangle"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Triangle"), bActive);
		break;
	case UI::PC_Mouse_Reduction:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Reduction"), bActive);
		break;
	case UI::PC_Mouse_Enlargement:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Enlargement"), bActive);
		break;
	case UI::InputButton_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Cancle"), bActive);
		break;
	case UI::InputButton_PS_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Cancle"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Cancle"), bActive);
		break;
	case UI::InputButton_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Up"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Up"), bActive);
		break;
	case UI::InputButton_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Down"), bActive);
		break;
	case UI::InputButton_PS_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Up"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Up"), bActive);
		break;
	case UI::InputButton_PS_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Down"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Down"), bActive);
		break;
	case UI::PlayerMarker:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PlayerMarker"), bActive);
		break;
	case UI::InputButton_PS_R1:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R1"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R1"), bActive);
		break;
	case UI::Arrowkeys_Side:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Left"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Right"), bActive);
		break;
	case UI::Arrowkeys_UpDown:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Up"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Down"), bActive);
		break;
	case UI::InputButton_PS_L2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_L2"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_L2"), bActive);
		break;
	case UI::InputButton_PS_R2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R2"), bActive);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R2"), bActive);
		break;
	case UI::StickIcon:
		SetUp_Clone(ePlayer, eTrigger, TEXT("StickIcon"), bActive);
		break;
	case UI::LoadingBook:
		SetUp_Clone(ePlayer, eTrigger, TEXT("LoadingBook"), bActive);
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

HRESULT CUI_Generator::Render_Font(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	_ulong iX, iY, iTextureWidth, iTextureHeigth, iFontWidth, iFontHeigth;
  	_int TextLen = lstrlen(pText);
	_int iGsOption;

	for (_int i = 0; i < TextLen; ++i)
	{
		_ulong iNumChar = pText[i];

		/* 한글 */
		if (44032 <= iNumChar) 		
		{
			iNumChar -= 44032;
			iX = iNumChar % 132;
			iY = iNumChar / 132;
			iTextureWidth = 4096;
			iTextureHeigth = 4096;
			iFontWidth = 31;
			iFontHeigth = 46;
		}
		/* 영어 */
		else if (65 <= iNumChar) 		
		{
			iNumChar -= 65 - 1;

			if (14 <= iNumChar)
				iNumChar += 1;

			iX = iNumChar % 16;
			iY = iNumChar / 16;
			iTextureWidth = 512;
			iTextureHeigth = 512;
			iFontWidth = 34;
			iFontHeigth = 45;
		}

		_float2 vLeftTop = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		_float2 vRightBottom = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vRightTop = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vLeftBottom = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };

		_float fPositionX = (tFontDesc.vPosition.x + (_float)i * iFontWidth) + ((_float)i * tFontDesc.fInterval);

		m_VTXFONT[i].vPosition = _float3(fPositionX, tFontDesc.vPosition.y, 0.f);
		m_VTXFONT[i].vScale = _float2(tFontDesc.vScale.x, tFontDesc.vScale.y);
		m_VTXFONT[i].vTexUV = _float4(vLeftTop.x, vLeftTop.y, vRightBottom.x, vRightBottom.y);

		_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubProjMatrix;

		WorldMatrix = XMMatrixIdentity();
		ViewMatrix = XMMatrixIdentity();
	
		_float2 vMainViewPort, vSubViewPort;
		D3D11_VIEWPORT Viewport;

		if (ePlayer == Player::Cody)
		{
			Viewport = pGameInstance->Get_ViewportInfo(1);
			vMainViewPort = { Viewport.Width, Viewport.Height };

			iGsOption = 0;

			if (0.f < Viewport.Width)
				ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		}
		else if (ePlayer == Player::May)
		{
			Viewport = pGameInstance->Get_ViewportInfo(2);
			vSubViewPort = { Viewport.Width, Viewport.Height };

			iGsOption = 1;

			if (0.f < Viewport.Width)
				SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
		}

		m_pVIBuffer_FontCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

		m_pVIBuffer_FontCom->Set_Variable("g_MainViewPort", &vMainViewPort, sizeof(_float2));
		m_pVIBuffer_FontCom->Set_Variable("g_SubViewPort", &vSubViewPort, sizeof(_float2));

		m_pVIBuffer_FontCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_FontCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_FontCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
		m_pVIBuffer_FontCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_FontCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

		if(0 == tFontDesc.iOption)
			m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
		else
			m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pEngTexturesCom->Get_ShaderResourceView(0));
	}
	
	m_pVIBuffer_FontCom->Render(0, m_VTXFONT, TextLen);

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
	else if (!lstrcmp(UIDesc->szUITag, L"LoadingBook"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CUISprite::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"StickIcon"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CUISprite::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}

	return S_OK;
}


void CUI_Generator::UI_RETutorial(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	if (eTrigger == UI::PC_Mouse_Enlargement)
	{
		for (auto UIObject : m_vecUIOBjects[ePlayer][UI::PC_Mouse_Reduction])
			UIObject->Set_PosX(0.f);

		Delete_UI(Player::Cody, UI::PC_Mouse_Enlargement);
	}
	else if (eTrigger == UI::PC_Mouse_Reduction)
	{
		for (auto UIObject : m_vecUIOBjects[ePlayer][UI::PC_Mouse_Enlargement])
			UIObject->Set_PosX(0.f);

		Delete_UI(Player::Cody, UI::PC_Mouse_Reduction);
	}

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
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("StickIcon"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/InputIcon/StickIcon.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("LoadingBook"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Loading/HakimSpinner.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Font"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_DDS, TEXT("../Bin/Resources/Texture/UI/Font/Font.dds"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("EngFont"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_DDS, TEXT("../Bin/Resources/Texture/UI/Font/EngFont.dds"))), E_FAIL);

	return S_OK;
}

HRESULT CUI_Generator::SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag, _bool bActive)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, PrototypeTag, nullptr, &pGameObject), E_FAIL);
	pUIObject = static_cast<CUIObject*>(pGameObject);
	pUIObject->Set_PlayerID(ePlayer);
	pUIObject->Set_Active(bActive);
	m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);

	return S_OK;
}

void CUI_Generator::Set_TargetPos(Player::ID ePlayer, UI::TRIGGER eTrigger, _vector vTargetPos)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
		UIObject->Set_TargetPos(vTargetPos);
}

void CUI_Generator::Set_Active(Player::ID ePlayer, UI::TRIGGER eTrigger, _bool bActive)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
		UIObject->Set_Active(bActive);
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

	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pEngTexturesCom);
	Safe_Release(m_pVIBuffer_FontCom);

	Safe_Delete_Array(m_VTXFONT);
}

