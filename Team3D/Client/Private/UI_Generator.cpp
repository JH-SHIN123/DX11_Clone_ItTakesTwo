#include "stdafx.h"
#include "..\Public\UI_Generator.h"

<<<<<<< HEAD
#include "Pipeline.h"
#include "UISprite.h"
#include "Portrait.h"
#include "HeaderBox.h"
#include "MenuScreen.h"
#include "AlphaScreen.h"
=======
#include "GameInstance.h"
#include "InputButton_Frame.h"
>>>>>>> main
#include "InputButton.h"
#include "SplashScreen.h"
#include "PlayerMarker.h"
#include "RespawnCircle.h"
#include "PC_MouseButton.h"
#include "Arrowkeys_Fill.h"
<<<<<<< HEAD
#include "ButtonIndicator.h"
#include "InputButton_Frame.h"
#include "Arrowkeys_Outline.h"
=======
#include "UISprite.h"
>>>>>>> main

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
	m_pVIBuffer_Rect = (CVIBuffer_Rect*)pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect"));
	
	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STATIC, TEXT("AlphaScreen"), CAlphaScreen::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	m_VTXFONT = new VTXFONT[50];

	return S_OK;
}

HRESULT CUI_Generator::Load_Data(const _tchar * pFilePath, Level::ID eLevel)
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
		if (eLevel == Level::LEVEL_LOGO)
		{
			if (FAILED(Add_Prototype_Menu(PSData)))
				return E_FAIL;
		}
		else if (eLevel == Level::LEVEL_STAGE)
		{
			if (FAILED(Add_Prototype_Interactive_UI(PSData)))
				return E_FAIL;

			if (FAILED(Add_Prototype_Fixed_UI(PSData)))
				return E_FAIL;
		}
	}

	if (eLevel == Level::LEVEL_LOGO)
		Create_Logo();

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Generator::Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (false == m_IsTrigger || ePlayer >= Player::PLAYER_END || eTrigger >= UI::TRIGGER_END)
		return S_OK;

	_uint iOption = 1;

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
	case UI::StickIcon:
		SetUp_Clone(ePlayer, eTrigger, TEXT("StickIcon"));
		break;
	case UI::LoadingBook:
		SetUp_Clone(ePlayer, eTrigger, TEXT("LoadingBook"));
		break;
	case UI::Portrait_Cody:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Portrait_Cody"));
		break;
	case UI::Portrait_May:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Portrait_May"));
		break;
	case UI::RespawnCircle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("RespawnCircle"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_E"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("ButtonIndicator"));
		SetUp_Clone(ePlayer, eTrigger, TEXT("RespawnCircle"), &iOption);
		SetUp_Clone(ePlayer, eTrigger, TEXT("AlphaScreen"));
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

HRESULT CUI_Generator::Render_Font(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer, _bool IsAlpha)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	_ulong iX, iY, iTextureWidth, iTextureHeigth, iFontWidth, iFontHeigth;
  	_int TextLen = lstrlen(pText);
	_int iGsOption;
	_int iOption;

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
			iOption = 0;

			//iNumChar -= 44032;
			//iX = iNumChar % 195;
			//iY = iNumChar / 195;
			//iTextureWidth = 4096;
			//iTextureHeigth = 4096;
			//iFontWidth = 21;
			//iFontHeigth = 33;
			//iOption = 0;
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
			iOption = 1;
		}
		/* 띄어쓰기 */
		else if (32 == iNumChar)
			continue;

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

		//if (true == IsAlpha)
		//{
		//	m_fFontAlpha = tFontDesc.fAlpha;
		//	m_pVIBuffer_FontCom->Set_Variable("g_fFontAlpha", &m_fFontAlpha, sizeof(_float));
		//}

		_float2 vMainViewPort, vSubViewPort, vDefaultViewPort;
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

		if (ePlayer == Player::Default)
		{
			ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
			vDefaultViewPort = { (_float)g_iWinCX / 2.f, (_float)g_iWinCY / 2.f };

			m_pVIBuffer_FontCom->Set_Variable("g_DefaultViewPort", &vDefaultViewPort, sizeof(_float2));

			m_pVIBuffer_FontCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));

			if (0 == iOption)
				m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
			else
				m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pEngTexturesCom->Get_ShaderResourceView(0));

			m_pVIBuffer_FontCom->Render(tFontDesc.iShaderPassNum, m_VTXFONT, TextLen);
		}
		else
		{
			m_pVIBuffer_FontCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

			m_pVIBuffer_FontCom->Set_Variable("g_MainViewPort", &vMainViewPort, sizeof(_float2));
			m_pVIBuffer_FontCom->Set_Variable("g_SubViewPort", &vSubViewPort, sizeof(_float2));

			m_pVIBuffer_FontCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
			m_pVIBuffer_FontCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

			if (0 == iOption)
				m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
			else
				m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pEngTexturesCom->Get_ShaderResourceView(0));

			m_pVIBuffer_FontCom->Render(tFontDesc.iShaderPassNum, m_VTXFONT, TextLen);
		}
	}

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
	else if (!lstrcmp(UIDesc->szUITag, L"Portrait_Cody"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPortrait::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"Portrait_May"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPortrait::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"RespawnCircle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CRespawnCircle::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_E"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"ButtonIndicator"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CButtonIndicator::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}


	return S_OK;
}

HRESULT CUI_Generator::Add_Prototype_Menu(CUIObject::UI_DESC* UIDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	if (!lstrcmp(UIDesc->szUITag, L"MenuBackScreen"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CMenuScreen::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Start"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Option"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Creator"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Exit"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
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
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("RespawnCircle"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/RespawnCircle.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Portrait_Cody"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/Portrait_Cody2021.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Portrait_May"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/Portrait_May2021.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("ButtonIndicator"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/ButtonIndicator.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("CoolDown"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/CoolDown.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("AlphaScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/SplashScreen/AlphaScreen.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Noise"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_TGA, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/RespawnCircle_Noise.tga"))), E_FAIL);

	return S_OK;
}


HRESULT CUI_Generator::Add_Prototype_LogoTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance(); 
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("Border_Banner_Texture"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/Border_Banner_Texture.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ButtonArrow"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ButtonArrow.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ChapterselectImageAlpha"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ChapterselectImageAlpha.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ControllerIcon"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ControllerIcon%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("HeaderBox"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/HeaderBox%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("MenuBackScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/MenuBackScreen.png"))), E_FAIL);
	//FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ChapterselectImageAlpha"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ChapterselectImageAlpha.png"))), E_FAIL);
	//FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ChapterSelect"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ChapterSelect.png"))), E_FAIL);

	return S_OK;
}


HRESULT CUI_Generator::SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, PrototypeTag, pArg, &pGameObject), E_FAIL);
	pUIObject = static_cast<CUIObject*>(pGameObject);
	pUIObject->Set_PlayerID(ePlayer);
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


void CUI_Generator::Set_ScaleEffect(Player::ID ePlayer, UI::TRIGGER eTrigger)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
		UIObject->Set_ScaleEffect();
}

HRESULT CUI_Generator::Create_Logo()
{
	SetUp_Clone(Player::Default, UI::MenuScreen, TEXT("MenuBackScreen"));
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Start"));
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Option"));
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Creator"));
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Exit"));

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

	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pEngTexturesCom);
	Safe_Release(m_pVIBuffer_FontCom);
	Safe_Release(m_pVIBuffer_Rect);

	Safe_Delete_Array(m_VTXFONT);
}

