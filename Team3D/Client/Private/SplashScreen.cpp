#include "stdafx.h"
#include "..\public\SplashScreen.h"
#include "GameInstance.h"
#include "UI_Generator.h"

CSplashScreen::CSplashScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSplashScreen::CSplashScreen(const CSplashScreen & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSplashScreen::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSplashScreen::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	if (0 == m_iOption)
	{
		m_fSortOrder = 0.f * -1.f;
		m_iPassNum = 8;
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 0.f));
		m_fSortOrder = -2.f;
	}
	else
	{
		m_fSortOrder = 1.f * -1.f;
		m_iPassNum = 9;
		m_pTransformCom->Set_Scale(XMVectorSet(900.f, 400.f, 0.f, 0.f));
		m_fSortOrder = -3.f;
	}
	
	m_vMaskUV = { 0.f ,0.f };

	if (0 == m_iOption)
		m_fScreenAlpha = 1.f;

	return S_OK;
}

_int CSplashScreen::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CSplashScreen::Late_Tick(_double dTimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	Logo_DisAppearing(dTimeDelta);

	m_vMaskUV.x -= (_float)dTimeDelta * 0.02f;
	m_vMaskUV.y -= (_float)dTimeDelta * 0.02f;

	m_fFontAlpha -= (_float)dTimeDelta;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CSplashScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(m_iPassNum);
	
	if (1 == m_iOption)
	{
		m_pVIBuffer_RectCom->Set_Variable("g_vScreenMaskUV", &m_vMaskUV, sizeof(_float2));
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseMaskTexture", m_pSubTextureCom->Get_ShaderResourceView(0));

		_int iCioror = 0;
		m_pVIBuffer_RectCom->Set_Variable("g_iColorOption", &iCioror, sizeof(_int));
		m_pVIBuffer_RectCom->Render(10);
		++iCioror;
		
		m_pVIBuffer_RectCom->Set_Variable("g_iColorOption", &iCioror, sizeof(_int));
		m_pVIBuffer_RectCom->Render(10);
		++iCioror;
		
		m_pVIBuffer_RectCom->Set_Variable("g_iColorOption", &iCioror, sizeof(_int));
		m_pVIBuffer_RectCom->Render(10);
	}

	Render_Font();

	return S_OK;
}

_float CSplashScreen::Get_DistanceFromCamera()
{
	return m_fSortOrder;
}

void CSplashScreen::Render_Font()
{
	CUI_Generator::FONTDESC tFontDesc;
	tFontDesc.vPosition = { -100.f, -270.f };
	tFontDesc.vScale = { 15.f, 20.f };
	tFontDesc.fInterval = -12.f;
	tFontDesc.iShaderPassNum = 1;
	tFontDesc.fAlpha = m_fFontAlpha;

	UI_Generator->Render_Font(TEXT("버튼을 눌러 계속하세요"), tFontDesc, Player::Default, true);
}

HRESULT CSplashScreen::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	if (0 == m_iOption)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("SplashBackScreen"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("SplashScreen"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("SplashScreen_Mask"), TEXT("Com_SubTexture"), (CComponent**)&m_pSubTextureCom), E_FAIL);

	}

	return S_OK;
}

HRESULT CSplashScreen::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();
	ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	m_pVIBuffer_RectCom->Set_Variable("g_fScreenAlpha", &m_fScreenAlpha, sizeof(_float));
	
	m_pVIBuffer_RectCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	if (1 == m_iOption)
	{
		m_pVIBuffer_RectCom->Set_Variable("g_vScreenMaskUV", &m_vMaskUV, sizeof(_float2));
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseMaskTexture", m_pSubTextureCom->Get_ShaderResourceView(0));
	}

	return S_OK;
}

void CSplashScreen::Logo_DisAppearing(_double TimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_RETURN))
		m_IsDisappear = true;

	if (true == m_IsDisappear)
	{
		m_fScreenAlpha -= (_float)TimeDelta * 0.4f;

		if (0.f >= m_fScreenAlpha && 1 == m_iOption)
			m_IsDead = true;
		else if (0.f >= m_fScreenAlpha && 0 == m_iOption)
		{
			m_IsBackScreenAlpha = true;
			m_IsDisappear = false;
			m_fScreenAlpha = 1.f;
			m_iPassNum = 9;
		}
	}
	else if (true == m_IsBackScreenAlpha && 0 == m_iOption)
	{
		m_fScreenAlpha -= (_float)TimeDelta * 0.4f;

		if (0.f >= m_fScreenAlpha)
			m_IsDead = true;
	}
	else
	{
		m_fScreenAlpha += (_float)TimeDelta * 0.6f;

		if (1.f <= m_fScreenAlpha)
			m_fScreenAlpha = 1.f;
	}
}

CSplashScreen * CSplashScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSplashScreen* pInstance = new CSplashScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSplashScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSplashScreen::Clone_GameObject(void * pArg)
{
	CSplashScreen* pInstance = new CSplashScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSplashScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplashScreen::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pSubTextureCom);
	Safe_Release(m_pVIBuffer_RectCom);

	CGameObject::Free();
}