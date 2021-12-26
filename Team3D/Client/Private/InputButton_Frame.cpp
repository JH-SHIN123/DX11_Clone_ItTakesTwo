#include "stdafx.h"
#include "..\Public\InputButton_Frame.h"

#include "UI_Generator.h"

CInputButton_Frame::CInputButton_Frame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CInputButton_Frame::CInputButton_Frame(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	if (pArg != nullptr)
		memcpy(&m_iFontOption, pArg, sizeof(_uint));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));

	SetUp_Option();

	m_vStartScale = m_UIDesc.vScale;

	if (1 == m_iOption)
	{
		m_UIDesc.vScale = { 1.f, 1.f };
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
	}
	else
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x - 15.f, m_UIDesc.vScale.y - 15.f, 0.f, 0.f));
	
	return S_OK;
}

_int CInputButton_Frame::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	ScaleEffect(TimeDelta);
	RespawnCircle_ScaleEffect();

	return _int();
}

_int CInputButton_Frame::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CInputButton_Frame::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (0 == m_iOption || 2 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}
	else if (3 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CUIObject::Set_InterActiveVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}

	m_pVIBuffer_RectCom->Set_Variable("g_iColorOption", &m_iColorOption, sizeof(_int));

	m_pVIBuffer_RectCom->Render(m_iShaderPassNum);

	Render_Font();

	return S_OK;
}

void CInputButton_Frame::Set_ScaleEffect()
{
}

void CInputButton_Frame::SetUp_Option()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_F")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Dot")) ||
		!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_PS_Triangle")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_PS_R1")))
	{
		m_iOption = 1;
		m_iShaderPassNum = 1;
		m_IsScaleBigger = true;
		m_vStartFontScale = { 30.f, 40.f };
		m_vFontScale = { 0.f, 0.f };

		if (lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Dot")))
		{
			m_UIDesc.vScale.x -= 15.f;
			m_UIDesc.vScale.y -= 15.f;
		}
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_E")))
	{
		m_iOption = 2;
		m_iShaderPassNum = 1;
		m_fPower = 20.f;
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Left")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Right")))
	{
		m_iOption = 3;
		m_iShaderPassNum = 11;
	}
	else
	{
		m_iOption = 0;
		m_iShaderPassNum = 1; 
	}
}

void CInputButton_Frame::RespawnCircle_ScaleEffect()
{
	/* RespawnCircle */
	if (2 == m_iOption)
	{
		if (m_vStartScale.x >= m_UIDesc.vScale.x)
		{
			m_UIDesc.vScale.x += m_fPower / 4.f;
			m_UIDesc.vScale.y += m_fPower / 4.f;
			m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
		}
	}
}

void CInputButton_Frame::ScaleEffect(_double TimeDelta)
{
	if (1 != m_iOption)
		return;

	if (m_vStartScale.x >= m_UIDesc.vScale.x)
	{
		m_UIDesc.vScale.x += (_float)TimeDelta * 200.f;
		m_UIDesc.vScale.y += (_float)TimeDelta * 200.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
	}

	if (m_vStartFontScale.x >= m_vFontScale.x && m_vStartFontScale.y >= m_vFontScale.y)
	{
		m_vFontScale.x += (_float)TimeDelta * 100.f;
		m_vFontScale.y += (_float)TimeDelta * 120.f;
	}

}

void CInputButton_Frame::Render_Font()
{
	CUI_Generator::FONTDESC		tFontDesc;

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_F")) && 0 == m_iFontOption)
	{
		tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
		tFontDesc.vScale = { m_vFontScale.x, m_vFontScale.y };

		UI_Generator->Render_Font(TEXT("E"), tFontDesc, m_ePlayerID);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_F")) && 1 == m_iFontOption)
	{
		tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
		tFontDesc.vScale = { m_vFontScale.x, m_vFontScale.y };

		UI_Generator->Render_Font(TEXT("F"), tFontDesc, m_ePlayerID);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_E")))
	{
		tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
		tFontDesc.vScale = { m_vFontScale.x, m_vFontScale.y };

		UI_Generator->Render_Font(TEXT("E"), tFontDesc, m_ePlayerID);
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Cancle")))
	{
		m_pEngFont->Render_Font(TEXT("Q"), _float2(279.f, 657.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.35f);
		m_pFont->Render_Font(TEXT("Ãë¼Ò"), _float2(330.f, 660.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.33f);
	}
}

HRESULT CInputButton_Frame::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_Font"), (CComponent**)&m_pFont), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_EngFont"), (CComponent**)&m_pEngFont), E_FAIL);

	return S_OK;
}

CInputButton_Frame * CInputButton_Frame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CInputButton_Frame* pInstance = new CInputButton_Frame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create InputButton_Frame Prototype, Error to CInputButton_Frame::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInputButton_Frame::Clone_GameObject(void * pArg)
{
	CInputButton_Frame* pClone = new CInputButton_Frame(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CInputButton_Frame, Error to CInputButton_Frame::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CInputButton_Frame::Free()
{
	Safe_Release(m_pEngFont);
	Safe_Release(m_pFont);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
