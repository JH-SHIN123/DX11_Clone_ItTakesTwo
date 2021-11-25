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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_vFontPos.x = m_UIDesc.vPos.x - 200.f;
	m_vFontPos.y = m_UIDesc.vPos.y;

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_LocalPlay")))
	{
		m_vFontPos.x = m_UIDesc.vPos.x - 100.f;
		m_IsRender = true;
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

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CHeaderBox::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	if(true == m_IsRender)
		m_pVIBuffer_RectCom->Render(11);

	//Render_Font();

	return S_OK;
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
	m_vFontPos.x = m_UIDesc.vPos.x - 100.f;
	m_IsRender = true;
}

void CHeaderBox::Set_PreviousSelect()
{
	m_vFontPos.x = m_UIDesc.vPos.x - 150.f;

	m_IsRender = false;
}


void CHeaderBox::Render_Font()
{
	CUI_Generator::FONTDESC		tFontDesc;

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_LocalPlay")))
	{
 		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("로컬 플레이"), tFontDesc, Player::Default);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_OnlinePlay")))
	{
		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("온라인 플레이"), tFontDesc, Player::Default);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Option")))
	{
		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("옵션"), tFontDesc, Player::Default);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Option2")))
	{
		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("접근성 옵션"), tFontDesc, Player::Default);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Creator")))
	{
		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("제작진"), tFontDesc, Player::Default);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_Exit")))
	{
		tFontDesc.vPosition = { m_vFontPos.x , m_vFontPos.y };
		tFontDesc.vScale = { 15.f, 20.f };
		tFontDesc.fInterval = 0.f;
		tFontDesc.iShaderPassNum = 1;

		UI_Generator->Render_Font(TEXT("종료"), tFontDesc, Player::Default);
	}

}

HRESULT CHeaderBox::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

void CHeaderBox::Mouse_Picking()
{
	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3D11_VIEWPORT Viewport;
	Viewport = m_pGameInstance->Get_ViewportInfo(0);

	pt.x = (_long)(pt.x - ((_float)Viewport.Width / 2.f));
	pt.y = (_long)((_float)Viewport.Height / 2.f - pt.y);

	// 렉트 버그 있음;;
	RECT rc = { (_long)(m_UIDesc.vPos.x - (m_UIDesc.vScale.x / 2.f)), (_long)(m_UIDesc.vPos.y - (m_UIDesc.vScale.y / 2.f) + (m_UIDesc.vScale.y / 2.f)),
		(_long)(m_UIDesc.vPos.x + (m_UIDesc.vScale.x / 2.f)), (_long)(m_UIDesc.vPos.y + (m_UIDesc.vScale.y / 2.f) + (m_UIDesc.vScale.y / 2.f)) };

	if (PtInRect(&rc, pt))
	{
		m_vFontPos.x = m_UIDesc.vPos.x - 100.f;
		m_vFontPos.y = m_UIDesc.vPos.y;
		m_IsRender = true;

		m_IsMousePicking = true;
	}
	else
	{
		m_vFontPos.x = m_UIDesc.vPos.x - 150.f;
		m_vFontPos.y = m_UIDesc.vPos.y;
		m_IsRender = false;

		m_IsMousePicking = false;
	}
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
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
