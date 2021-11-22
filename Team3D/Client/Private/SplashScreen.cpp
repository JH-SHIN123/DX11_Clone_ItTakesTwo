#include "stdafx.h"
#include "..\public\SplashScreen.h"
#include "GameInstance.h"
#include "DataStorage.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("SplashScreen"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("SplashScreen_Mask"), TEXT("Com_SubTexture"), (CComponent**)&m_pSubTextureCom), E_FAIL);

	return S_OK;
}

_int CSplashScreen::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CSplashScreen::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CSplashScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CSplashScreen::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();
	ProjMatrix = XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f);

	m_pVIBuffer_RectCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	return S_OK;
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