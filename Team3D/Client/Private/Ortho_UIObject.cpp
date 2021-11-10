#include "stdafx.h"
#include "..\public\Ortho_UIObject.h"
#include "GameInstance.h"

COrtho_UIObject::COrtho_UIObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

COrtho_UIObject::COrtho_UIObject(const COrtho_UIObject & rhs)
	: CGameObject(rhs),
	m_UIDesc(rhs.m_UIDesc)
{
}

HRESULT COrtho_UIObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT COrtho_UIObject::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

_int COrtho_UIObject::Tick(_double TimeDelta)
{
	
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int COrtho_UIObject::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return NO_EVENT;
}

HRESULT COrtho_UIObject::Render()
{
	CGameObject::Render();

	return S_OK;
}

void COrtho_UIObject::Set_Dead()
{
	m_IsDead = true;
}

HRESULT COrtho_UIObject::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (1 <= m_UIDesc.iSubTextureNum)
		FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szSubTextureTag, TEXT("Com_SubTexture"), (CComponent**)&m_pSubTexturCom), E_FAIL);

	return S_OK;
}


void COrtho_UIObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	if (1 <= m_UIDesc.iSubTextureNum)
		Safe_Release(m_pSubTexturCom);

	CGameObject::Free();
}