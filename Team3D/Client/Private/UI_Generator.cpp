#include "stdafx.h"
#include "..\Public\UI_Generator.h"

#include "PC_Enter.h"
#include "Pipeline.h"
#include "UISprite.h"
#include "Portrait.h"
#include "HeaderBox.h"
#include "MenuScreen.h"
#include "AlphaScreen.h"
#include "InputButton.h"
#include "SplashScreen.h"
#include "PlayerMarker.h"
#include "RespawnCircle.h"
#include "ChapterSelect.h"
#include "ControllerIcon.h"
#include "PC_MouseButton.h"
#include "Arrowkeys_Fill.h"
#include "ButtonIndicator.h"
#include "InputButton_Frame.h"
#include "Arrowkeys_Outline.h"

#include "Cody.h"
#include "May.h"

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
	
	//CUIObject::UI_DESC UIDesc;
	//UIDesc.iLevelIndex = 0;
	//UIDesc.iRenderGroup = 1;
	//UIDesc.iSubTextureNum = 0;
	//UIDesc.iTextureLevelIndex = 0;
	//UIDesc.iTextureRenderIndex = 0;
	//lstrcpy(UIDesc.szSubTextureTag, TEXT(""));
	//lstrcpy(UIDesc.szTextureTag, TEXT("LoadingBook"));
	//lstrcpy(UIDesc.szSubTextureTag, TEXT("Loading_Book"));
	//UIDesc.vPos = _float2(583.f, -307.f);
	//UIDesc.vScale = _float2(100.f, 100.f);
	//FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STATIC, TEXT("Loading_Book"), CUISprite::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	m_VTXFONT = new VTXFONT[50];


	return S_OK;
}

HRESULT CUI_Generator::Load_Data(const _tchar * pFilePath, Level::ID eLevel, _uint iOption)
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
			if (0 == iOption)
			{
				if (FAILED(Add_Prototype_Menu(PSData)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(Add_Prototype_Chapter(PSData)))
					return E_FAIL;
			}
		}
		else if (eLevel == Level::LEVEL_STAGE)
		{
			if (FAILED(Add_Prototype_Interactive_UI(PSData)))
				return E_FAIL;

			if (FAILED(Add_Prototype_Fixed_UI(PSData)))
				return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Generator::Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger,void* pArg)
{
	if (false == m_IsTrigger || ePlayer >= Player::PLAYER_END || eTrigger >= UI::TRIGGER_END)
		return S_OK;

	_uint iOption = 1;

	switch (eTrigger)
	{
	case UI::InputButton_Dot:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Dot"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_F"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_InterActive_Render:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_F"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_InterActive:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Triangle"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Triangle"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::PC_Mouse_Reduction:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Reduction"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::PC_Mouse_Enlargement:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PC_Mouse_Enlargement"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Cancle"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_Cancle:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Cancle"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Cancle"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Up"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Up"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_Down"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_Up:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Up"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Up"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_Down:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_Down"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_Down"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::PlayerMarker:
		SetUp_Clone(ePlayer, eTrigger, TEXT("PlayerMarker"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_R1:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R1"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R1"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::Arrowkeys_Side:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Left"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Right"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::Arrowkeys_UpDown:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Outline"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Up"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("Arrowkeys_Fill_Down"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_L2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_L2"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_L2"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::InputButton_PS_R2:
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_PS_R2"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_PS_R2"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::StickIcon:
		SetUp_Clone(ePlayer, eTrigger, TEXT("StickIcon"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::LoadingBook:
		SetUp_Clone(ePlayer, eTrigger, TEXT("LoadingBook"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::Portrait_Cody:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Portrait_Cody"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::Portrait_May:
		SetUp_Clone(ePlayer, eTrigger, TEXT("Portrait_May"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::RespawnCircle:
		/* 리스폰 서클 게이지 바 */
		SetUp_Clone(ePlayer, eTrigger, TEXT("RespawnCircle"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("InputButton_Frame_E"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone(ePlayer, eTrigger, TEXT("ButtonIndicator"), Level::LEVEL_STATIC, pArg);
		/* 리스폰 서클 하트 */
		SetUp_Clone(ePlayer, eTrigger, TEXT("RespawnCircle"), Level::LEVEL_STATIC, &iOption);
		iOption = 0;
		SetUp_Clone(ePlayer, eTrigger, TEXT("AlphaScreen"), Level::LEVEL_STATIC, &iOption);
		break;
	case UI::ControllerIcon_KeyBoard:
		SetUp_Clone(ePlayer, eTrigger, TEXT("ControllerIcon_KeyBoard"), Level::LEVEL_LOGO, pArg);
		break;
	case UI::ControllerIcon_Pad:
		SetUp_Clone(ePlayer, eTrigger, TEXT("ControllerIcon_Pad"), Level::LEVEL_LOGO, pArg);
		break;
	default:
		MSG_BOX("UI Trigger does not exist, Error to CUI_Generator::Generator_UI");
		break;
	}

	m_IsTrigger = false;

	return S_OK;
}


HRESULT CUI_Generator::Generator_InterActive_UI(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, void * pArg)
{
	if (false == m_IsTrigger || ePlayer >= Player::PLAYER_END || eTrigger >= UI::INTERACTIVE_ID_END)
		return S_OK;

	switch (eTrigger)
	{
	case UI::ControlRoom_Battery:
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Dot"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::PinBall_Door:
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Dot"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::StarBuddy:
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Dot"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), Level::LEVEL_STATIC, pArg);
		break;
	case UI::PinBall_Handle:
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Dot"), Level::LEVEL_STATIC, pArg);
		SetUp_Clone_InterActive(ePlayer, eTrigger, TEXT("InputButton_Frame_Dot"), Level::LEVEL_STATIC, pArg);
		break;
	default:
		MSG_BOX("UI Trigger does not exist, Error to CUI_Generator::Generator_UI");
		break;
	}



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

HRESULT CUI_Generator::Delete_InterActive_UI(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger)
{
	if (true == m_vecInterActiveUI[ePlayer][eTrigger].empty())
		return S_OK;

	for (auto UIObject : m_vecInterActiveUI[ePlayer][eTrigger])
	{
		UIObject->Set_Dead();
		Safe_Release(UIObject);
	}

	m_vecInterActiveUI[ePlayer][eTrigger].clear();

	return S_OK;
}

HRESULT CUI_Generator::Render_Font(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer)
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
			//iNumChar -= 44032;
			//iX = iNumChar % 132;
			//iY = iNumChar / 132;
			//iTextureWidth = 4096;
			//iTextureHeigth = 4096;
			//iFontWidth = 31;
			//iFontHeigth = 46;
			//iOption = 0;
			iNumChar -= 44032;
			iX = iNumChar % 132;
			iY = iNumChar / 132;
			iTextureWidth = 8192;
			iTextureHeigth = 8192;
			iFontWidth = 62;
			iFontHeigth = 96;
			iOption = 0;

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

		_float fInterval = ((_float)TextLen * iFontWidth) / (tFontDesc.vScale.x * 2.f * (_float)TextLen);
		_float fValue = 1.f;

		if (0.f <= tFontDesc.vPosition.x)
			fValue *= -1.f;

		_float2 vLeftTop = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		_float2 vRightBottom = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vRightTop = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vLeftBottom = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };

		//_float fPositionX = (tFontDesc.vPosition.x + (_float)i * iFontWidth) / fInterval + (tFontDesc.vPosition.x * fValue);
		_float fPositionX = tFontDesc.vPosition.x + ((_float)i * iFontWidth / fInterval);

		m_VTXFONT[i].vPosition = _float3(fPositionX, tFontDesc.vPosition.y, 0.f);
		m_VTXFONT[i].vScale = _float2(tFontDesc.vScale.x, tFontDesc.vScale.y);
		m_VTXFONT[i].vTexUV = _float4(vLeftTop.x, vLeftTop.y, vRightBottom.x, vRightBottom.y);

		_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubProjMatrix;

		WorldMatrix = XMMatrixIdentity();
		ViewMatrix = XMMatrixIdentity();

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
		}
	}
	m_pVIBuffer_FontCom->Set_Variable("g_vColor", &tFontDesc.vColor, sizeof(_float3));

	if (0 == iOption)
		m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	else
		m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pEngTexturesCom->Get_ShaderResourceView(0));

	m_pVIBuffer_FontCom->Render(tFontDesc.iShaderPassNum, m_VTXFONT, TextLen);

	return S_OK;
}


HRESULT CUI_Generator::Render_AlphaFont(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer)
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
			//iNumChar -= 44032;
			//iX = iNumChar % 132;
			//iY = iNumChar / 132;
			//iTextureWidth = 4096;
			//iTextureHeigth = 4096;
			//iFontWidth = 31;
			//iFontHeigth = 46;
			//iOption = 0;
			iNumChar -= 44032;
			iX = iNumChar % 132;
			iY = iNumChar / 132;
			iTextureWidth = 8192;
			iTextureHeigth = 8192;
			iFontWidth = 62;
			iFontHeigth = 96;
			iOption = 0;

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

		_float fTime = 0.0016f;

		if (0.f >= m_fFontAlpha)
			m_fChange *= -1.f;
		else if (1.f <= m_fFontAlpha)
			m_fChange = 1.f;

		m_fFontAlpha -= fTime * m_fChange * 0.2f;

		m_pVIBuffer_FontCom->Set_Variable("g_fFontAlpha", &m_fFontAlpha, sizeof(_float));

		_float fInterval = ((_float)TextLen * iFontWidth) / (tFontDesc.vScale.x * 2.f * (_float)TextLen);
		_float fValue = 1.f;

		if (0.f <= tFontDesc.vPosition.x)
			fValue *= -1.f;

		_float2 vLeftTop = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		_float2 vRightBottom = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vRightTop = { (_float)(iX + 1) * iFontWidth / (_float)iTextureWidth, (_float)iY * iFontHeigth / (_float)iTextureHeigth };
		//_float2 vLeftBottom = { (_float)iX * iFontWidth / (_float)iTextureWidth, (_float)(iY + 1) * iFontHeigth / (_float)iTextureHeigth };

		_float fPositionX = tFontDesc.vPosition.x + ((_float)i * iFontWidth / fInterval);

		m_VTXFONT[i].vPosition = _float3(fPositionX, tFontDesc.vPosition.y, 0.f);
		m_VTXFONT[i].vScale = _float2(tFontDesc.vScale.x, tFontDesc.vScale.y);
		m_VTXFONT[i].vTexUV = _float4(vLeftTop.x, vLeftTop.y, vRightBottom.x, vRightBottom.y);

		_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubProjMatrix;

		WorldMatrix = XMMatrixIdentity();
		ViewMatrix = XMMatrixIdentity();

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

		}
	}

	m_pVIBuffer_FontCom->Set_Variable("g_vColor", &tFontDesc.vColor, sizeof(_float3));

	if (0 == iOption)
		m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	else
		m_pVIBuffer_FontCom->Set_ShaderResourceView("g_DiffuseTexture", m_pEngTexturesCom->Get_ShaderResourceView(0));

	m_pVIBuffer_FontCom->Render(tFontDesc.iShaderPassNum, m_VTXFONT, TextLen);

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
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_LocalPlay"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_OnlinePlay"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Option"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Option2"))
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

HRESULT CUI_Generator::Add_Prototype_Chapter(CUIObject::UI_DESC * UIDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	if (!lstrcmp(UIDesc->szUITag, L"ChapterLocalPlay"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_NewGame"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Continue"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_ChapterSelect"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Minigame"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"ChapterSelectAlpha"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CChapterSelect::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"ChapterSelect_1"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CChapterSelect::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Cancle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_1p_Ready"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_2p_Ready"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"ControllerIcon_KeyBoard"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CControllerIcon::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"ControllerIcon_Pad"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CControllerIcon::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Right"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"PC_Enter"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPC_Enter::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Right_TriAngle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Left_TriAngle"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"InputButton_Frame_Left"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CInputButton_Frame::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"PC_Enter_Right"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CPC_Enter::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Banner"))
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype((Level::ID)UIDesc->iLevelIndex, UIDesc->szUITag, CHeaderBox::Create(m_pDevice, m_pDeviceContext, UIDesc)), E_FAIL);
	}
	else if (!lstrcmp(UIDesc->szUITag, L"HeaderBox_Banner_Back"))
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
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Font"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_DDS, TEXT("../Bin/Resources/Texture/UI/Font/Font4_0.dds"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("EngFont"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_DDS, TEXT("../Bin/Resources/Texture/UI/Font/EngFont.dds"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("RespawnCircle"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/RespawnCircle.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Portrait_Cody"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/Portrait_Cody2021.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Portrait_May"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/Portrait_May2021.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("ButtonIndicator"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/ButtonIndicator.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("CoolDown"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/CoolDown.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("AlphaScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/SplashScreen/AlphaScreen.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Noise"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/PlayerHealth/RespawnCircle_Noise.png"))), E_FAIL);

	return S_OK;
}



HRESULT CUI_Generator::Add_Prototype_LogoTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance(); 
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("SplashScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Logo/SplashScreen.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("SplashScreen_Mask"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Logo/SplashScreen_Masks.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("SplashBackScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Logo/SplashBackScreen.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Prototype(Level::LEVEL_LOGO, TEXT("SplashScreen"), CSplashScreen::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ButtonArrow"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ButtonArrow.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ChapterSelect"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ChapterSelect%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ControllerIcon"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ControllerIcon%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("HeaderBox"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/HeaderBox%d.png"), 4)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("MenuBackScreen"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/MenuBackScreen.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("PC_Enter"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/PC_Enter.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Component_Prototype(Level::LEVEL_LOGO, TEXT("ChapterSelectAlpha"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/UI/Menu/ChapterSelect0.png"))), E_FAIL);

	return S_OK;
}

HRESULT CUI_Generator::SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag, Level::ID eLevel, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(eLevel, TEXT("Layer_UI"), eLevel, PrototypeTag, pArg, &pGameObject), E_FAIL);
	pUIObject = static_cast<CUIObject*>(pGameObject);
	pUIObject->Set_PlayerID(ePlayer);
	m_vecUIOBjects[ePlayer][eTrigger].push_back(pUIObject);

	return S_OK;
}

HRESULT CUI_Generator::SetUp_Clone_InterActive(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, const _tchar * PrototypeTag, Level::ID eLevel, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	CGameObject* pGameObject = nullptr;
	CUIObject* pUIObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(eLevel, TEXT("Layer_UI"), eLevel, PrototypeTag, pArg, &pGameObject), E_FAIL);
	pUIObject = static_cast<CUIObject*>(pGameObject);
	pUIObject->Set_PlayerID(ePlayer);
	m_vecInterActiveUI[ePlayer][eTrigger].push_back(pUIObject);

	return S_OK;
}


HRESULT CUI_Generator::SetUp_Clone_Ptr(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag, Level::ID eLevel, void* pArg, CGameObject** pGameObject)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(eLevel, TEXT("Layer_UI"), eLevel, PrototypeTag, pArg, pGameObject), E_FAIL);
	//d
	return S_OK;
}

void CUI_Generator::Set_TargetPos(Player::ID ePlayer, UI::TRIGGER eTrigger, _vector vTargetPos)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
		UIObject->Set_TargetPos(vTargetPos);
}

void CUI_Generator::Set_InterActive_TargetPos(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, _vector vTargetPos)
{
	if (true == m_vecInterActiveUI[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecInterActiveUI[ePlayer][eTrigger])
		UIObject->Set_TargetPos(vTargetPos);
}


void CUI_Generator::Set_Active(Player::ID ePlayer, UI::TRIGGER eTrigger, _bool bActive)
{
	if (true == m_vecUIOBjects[ePlayer][eTrigger].empty())
		return;

	for (auto UIObject : m_vecUIOBjects[ePlayer][eTrigger])
		UIObject->Set_Active(bActive);
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
	CGameObject* pGameObject = nullptr;
	m_vecHeaderBox.reserve(6);

	SetUp_Clone(Player::Default, UI::MenuScreen, TEXT("MenuBackScreen"), Level::LEVEL_LOGO);

	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_LocalPlay"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));
	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_OnlinePlay"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));
	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Option"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));
	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Option2"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));
	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Creator"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));
	SetUp_Clone_Ptr(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Exit"), Level::LEVEL_LOGO, nullptr, &pGameObject);
	m_vecHeaderBox.emplace_back(static_cast<CHeaderBox*>(pGameObject));

	return S_OK;
}

HRESULT CUI_Generator::Create_ChapterSelect()
{
	_uint iOption = 1;
	SetUp_Clone(Player::Default, UI::AlphaScreen, TEXT("AlphaScreen"), Level::LEVEL_STATIC, &iOption);

	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("ChapterLocalPlay"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Banner"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_NewGame"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Continue"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_ChapterSelect"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Minigame"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("ChapterSelect_1"), Level::LEVEL_LOGO);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Banner_Back"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox, TEXT("HeaderBox_Cancle"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox_1p_Ready, TEXT("HeaderBox_1p_Ready"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox_2p_Ready, TEXT("HeaderBox_2p_Ready"), Level::LEVEL_LOGO, &iOption);
	SetUp_Clone(Player::Default, UI::HeaderBox1P, TEXT("PC_Enter_Right"), Level::LEVEL_LOGO);
	SetUp_Clone(Player::Default, UI::HeaderBox1P, TEXT("InputButton_Frame_Left"), Level::LEVEL_STATIC);
	SetUp_Clone(Player::Default, UI::HeaderBox1P, TEXT("InputButton_Left_TriAngle"), Level::LEVEL_STATIC);
	SetUp_Clone(Player::Default, UI::HeaderBox2P, TEXT("InputButton_Right_TriAngle"), Level::LEVEL_STATIC);
	SetUp_Clone(Player::Default, UI::HeaderBox2P, TEXT("InputButton_Frame_Right"), Level::LEVEL_STATIC);
	SetUp_Clone(Player::Default, UI::HeaderBox2P, TEXT("PC_Enter"), Level::LEVEL_LOGO);

	iOption = 2;
	SetUp_Clone(Player::Default, UI::AlphaScreen, TEXT("AlphaScreen"), Level::LEVEL_STATIC, &iOption);

	return S_OK;
}

HRESULT CUI_Generator::CreateInterActiveUI_AccordingRange(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, _vector vTargetPosition, _float fRange, _bool IsCollision, _bool IsDisable)
{
	if (true == IsDisable)
		return S_OK;

	_vector vComparePos;

	if (ePlayer == Player::Cody)
	{
		CCody* pCody = (CCody*)DATABASE->GetCody();
		NULL_CHECK_RETURN(pCody, false);

		_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vComparePos = vTargetPosition - vCodyPos;
	}
	else if (ePlayer == Player::May)
	{
		CMay* pMay = (CMay*)DATABASE->GetMay();
		NULL_CHECK_RETURN(pMay, false);

		_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vComparePos = vTargetPosition - vMayPos;
	}

	_float vComparePosX = fabs(XMVectorGetX(vComparePos));
	_float vComparePosY = fabs(XMVectorGetY(vComparePos));
	_float vComparePosZ = fabs(XMVectorGetZ(vComparePos));

	/* 범위 안에 있다*/
	if (fRange >= vComparePosX && fRange >= vComparePosY && fRange >= vComparePosZ)
	{
		/* 충돌해서 트리거가 켜졌다면 InterActive UI 만들어주자 */
		if (ePlayer == Player::Cody)
		{
			if (true == IsCollision)
			{
				/* 충돌을 했다라면 InputButton Dot 삭제해주자 안에서 예외처리 하기때문에 걍 호출해도 상관없음 */
				Delete_InterActive_UI(ePlayer, eTrigger);
				UI_CreateOnlyOnce(Cody, InputButton_InterActive);
				UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, vTargetPosition);

				return S_OK;
			}
			else /* 만약에 충돌을 하고 트리거가 꺼졌을 때 InterActive UI 삭제해주자 */
				UI_Delete(Cody, InputButton_InterActive);
		}
		else if (ePlayer == Player::May)
		{
			if (true == IsCollision)
			{
				Delete_InterActive_UI(ePlayer, eTrigger);
				UI_CreateOnlyOnce(May, InputButton_PS_InterActive);
				UI_Generator->Set_TargetPos(Player::May, UI::InputButton_PS_InterActive, vTargetPosition);

				return S_OK;
			}
			else
				UI_Delete(May, InputButton_PS_InterActive);
		}

		/* 충돌을 아직 안했다면 그냥 InputButton Dot 만들어주자 */
		if (true == UI_Generator->Get_InterActive_UI_EmptyCheck(ePlayer, eTrigger))
		{
			m_IsTrigger = true;
			Generator_InterActive_UI(ePlayer, eTrigger);
		}

		UI_Generator->Set_InterActive_TargetPos(ePlayer, eTrigger, vTargetPosition);
	}
	else
		Delete_InterActive_UI(ePlayer, eTrigger);

	return S_OK;
}

_bool CUI_Generator::RangeCheck(Player::ID ePlayer, _vector vTargetPosition, _float fRange, _bool IsDisable)
{
	if (true == IsDisable)
		return false;

	_vector vComparePos;

	if (ePlayer == Player::Cody)
	{
		CCody* pCody = (CCody*)DATABASE->GetCody();
		NULL_CHECK_RETURN(pCody, false);

		_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vComparePos = vTargetPosition - vCodyPos;
	}
	else if (ePlayer == Player::May)
	{
		CMay* pMay = (CMay*)DATABASE->GetMay();
		NULL_CHECK_RETURN(pMay, false);

		_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vComparePos = vTargetPosition - vMayPos;
	}

	_float vComparePosX = fabs(XMVectorGetX(vComparePos));
	_float vComparePosY = fabs(XMVectorGetY(vComparePos));
	_float vComparePosZ = fabs(XMVectorGetZ(vComparePos));

	if (fRange >= vComparePosX && fRange >= vComparePosY && fRange >= vComparePosZ)
		return true;

	return false;
}

void CUI_Generator::Free()
{
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

	for (_uint i = 0; i < Player::PLAYER_END; ++i)
	{
		for (_uint j = 0; j < UI::INTERACTIVE_ID_END; ++j)
		{
			if (0 != m_vecInterActiveUI[i][j].size())
			{
				for (auto UIObject : m_vecInterActiveUI[i][j])
					Safe_Release(UIObject);

				m_vecInterActiveUI[i][j].clear();
			}
		}
	}

	for (auto pHeaderBox : m_vecHeaderBox)
		Safe_Release(pHeaderBox);

	m_vecHeaderBox.clear();

	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pEngTexturesCom);
	Safe_Release(m_pVIBuffer_FontCom);
	Safe_Release(m_pVIBuffer_Rect);

	Safe_Delete_Array(m_VTXFONT);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
