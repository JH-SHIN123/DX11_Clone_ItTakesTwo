#include "stdafx.h"
#include "..\Public\HeaderBox.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CHeaderBox::CHeaderBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CHeaderBox::CHeaderBox(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CHeaderBox::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHeaderBox::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CHeaderBox::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_LocalPlay")))
	{
		m_fFontPos = _float2(155.f, 380.f);
		m_IsRender = true;
	}

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Creator")))
	{
		m_fFontPos = _float2(132.f, 428.f);
		m_IsRender = false;
	}

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Exit")))
	{
		m_fFontPos = _float2(125.f, 605.f);
		m_IsRender = false;
	}

	return S_OK;
}

_int CHeaderBox::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CHeaderBox::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	//Mouse_Picking();

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Continue")))
		m_IsRender = false;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_ChapterSelect")))
		m_IsRender = false;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Minigame")))
		m_IsRender = false;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CHeaderBox::Render(RENDER_GROUP::Enum eGroup)
{
	if (false == m_IsLogoDisappear && 1 != m_iOption)
		return S_OK;

	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	Set_ShaderOption();

	if(true == m_IsRender)
		m_pVIBuffer_RectCom->Render(13);

	Render_Font();

	return S_OK;
}

_bool CHeaderBox::Get_LocalPlayRender()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_LocalPlay")))
		return m_IsRender;

	return false;
}

void CHeaderBox::Set_ScaleEffect()
{
	//if (2 != m_iOption && 10.f >= m_UIDesc.vScale.x)
	//	return;

	//m_UIDesc.vScale.x -= m_fPower;
	//m_UIDesc.vScale.y -= m_fPower;
	//m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
}

void CHeaderBox::Set_NextSelect()
{
	m_IsRender = true;
	m_iPassNum = 4;
}

void CHeaderBox::Set_PreviousSelect()
{
	m_IsRender = false;
	m_iPassNum = 1;
}

void CHeaderBox::Set_LogoDisappear()
{
	m_IsLogoDisappear = true;
}

void CHeaderBox::Set_Ready(_bool IsCheck)
{
	m_IsReady = IsCheck;
}

void CHeaderBox::Set_ColorChange()
{
	m_IsReady = true;
	m_iShaderOption = 3;
}

void CHeaderBox::Render_Font()
{
	CUI_Generator::FONTDESC		tFontDesc;
	_float fSizeX = 13.f;
	_float fSizeY = 13.f;

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_LocalPlay")))
	{
		if(false == m_IsRender)
			m_pFont->Render_Font(TEXT("플레이"), _float2(632, 505.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.34f);
		else
			m_pFont->Render_Font(TEXT("플레이"), _float2(632, 505.f), XMVectorSet(0.25f, 0.f, 0.f, 1.f), 0.37f);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Creator")))
	{
		if (false == m_IsRender)
			m_pFont->Render_Font(TEXT("제작진"), _float2(632, 558.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.34f);
		else
			m_pFont->Render_Font(TEXT("제작진"), _float2(632, 558.f), XMVectorSet(0.25f, 0.f, 0.f, 1.f), 0.37f);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Exit")))
	{
		if (false == m_IsRender)
			m_pFont->Render_Font(TEXT("종료"), _float2(633, 609.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.34f);
		else
			m_pFont->Render_Font(TEXT("종료"), _float2(633, 609.f), XMVectorSet(0.25f, 0.f, 0.f, 1.f), 0.37f);
	}
}

HRESULT CHeaderBox::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_Font"), (CComponent**)&m_pFont), E_FAIL);

	return S_OK;
}

void CHeaderBox::Mouse_Picking()
{
	//POINT	pt = {};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);

	//D3D11_VIEWPORT Viewport;
	//Viewport = m_pGameInstance->Get_ViewportInfo(0);

	//pt.x = (_long)(pt.x - ((_float)Viewport.Width / 2.f));
	//pt.y = (_long)((_float)Viewport.Height / 2.f - pt.y);

	//// 렉트 버그 있음;;
	//RECT rc = { (_long)(m_UIDesc.vPos.x - (m_UIDesc.vScale.x / 2.f)), (_long)(m_UIDesc.vPos.y - (m_UIDesc.vScale.y / 2.f) + (m_UIDesc.vScale.y / 2.f)),
	//	(_long)(m_UIDesc.vPos.x + (m_UIDesc.vScale.x / 2.f)), (_long)(m_UIDesc.vPos.y + (m_UIDesc.vScale.y / 2.f) + (m_UIDesc.vScale.y / 2.f)) };

	//if (PtInRect(&rc, pt))
	//{
	//	m_vFontPos.x = m_UIDesc.vPos.x - 100.f;
	//	m_vFontPos.y = m_UIDesc.vPos.y;
	//	m_IsRender = true;

	//	m_IsMousePicking = true;
	//}
	//else
	//{
	//	m_vFontPos.x = m_UIDesc.vPos.x - 150.f;
	//	m_vFontPos.y = m_UIDesc.vPos.y;
	//	m_IsRender = false;

	//	m_IsMousePicking = false;
	//}
}

void CHeaderBox::Set_Option()
{
}

void CHeaderBox::Set_ShaderOption()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("ChapterLocalPlay")))
		m_iShaderOption = 0;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_NewGame")))
		m_iShaderOption = 1;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Continue")))
		m_iShaderOption = 1;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_ChapterSelect")))
		m_iShaderOption = 1;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Minigame")))
		m_iShaderOption = 1;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Cancle")))
		m_iShaderOption = 1;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_1p_Ready")) && false == m_IsReady)
		m_iShaderOption = 0;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_2p_Ready")) && false == m_IsReady)
		m_iShaderOption = 0;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Banner")))
		m_iShaderOption = 2;
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Banner_Back")))
		m_iShaderOption = 2;

	m_pVIBuffer_RectCom->Set_Variable("g_iHeaderBoxOption", &m_iShaderOption, sizeof(_int));
}

CHeaderBox * CHeaderBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHeaderBox* pInstance = new CHeaderBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create HeaderBox Prototype, Error to CHeaderBox::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHeaderBox::Clone_GameObject(void * pArg)
{
	CHeaderBox* pClone = new CHeaderBox(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CHeaderBox, Error to CHeaderBox::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CHeaderBox::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
