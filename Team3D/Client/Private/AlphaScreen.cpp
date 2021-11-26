#include "stdafx.h"
#include "..\Public\AlphaScreen.h"

#include "UI_Generator.h"

CAlphaScreen::CAlphaScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CAlphaScreen::CAlphaScreen(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CAlphaScreen::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAlphaScreen::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	lstrcpy(m_UIDesc.szTextureTag, TEXT("AlphaScreen"));


	return S_OK;
}

HRESULT CAlphaScreen::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	if (2 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 50.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(400.f, 300.f, 0.f, 0.f));
		m_fSortOrder = -3.f;
	}
	else if (1 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 0.f));
		m_fSortOrder = -2.f;
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(640.f, 720.f, 0.f, 0.f));
		m_fSortOrder = -1.f;
	}

	return S_OK;
}

_int CAlphaScreen::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CAlphaScreen::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CAlphaScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (1 == m_iOption || 2 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_pVIBuffer_RectCom->Set_Variable("g_iAlphaOption", &m_iOption, sizeof(_int));

		m_pVIBuffer_RectCom->Render(12);
	}
	else
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_pVIBuffer_RectCom->Render(7);
	}

	return S_OK;
}


HRESULT CAlphaScreen::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CAlphaScreen * CAlphaScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CAlphaScreen* pInstance = new CAlphaScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create AlphaScreen Prototype, Error to CAlphaScreen::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlphaScreen::Clone_GameObject(void * pArg)
{
	CAlphaScreen* pClone = new CAlphaScreen(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CAlphaScreen, Error to CAlphaScreen::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAlphaScreen::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
