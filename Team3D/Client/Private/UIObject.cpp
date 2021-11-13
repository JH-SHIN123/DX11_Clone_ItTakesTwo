#include "stdafx.h"
#include "..\public\UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUIObject::CUIObject(const CUIObject & rhs)
	: CGameObject(rhs),
	m_UIDesc(rhs.m_UIDesc)
{
}

HRESULT CUIObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();
	//d

	return S_OK;
}

HRESULT CUIObject::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fSortOrder = (_float)m_UIDesc.iRenderGroup * -1.f;

	return S_OK;
}

_int CUIObject::Tick(_double TimeDelta)
{

	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CUIObject::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return NO_EVENT;
}

HRESULT CUIObject::Render()
{
	CGameObject::Render();

	return S_OK;
}

_float CUIObject::Get_DistanceFromCamera()
{
	return m_fSortOrder;
}

void CUIObject::Set_Dead()
{
	m_IsDead = true;
}

void CUIObject::Set_PlayerID(Player::ID ePlayerID)
{
	m_ePlayerID = ePlayerID;
}

HRESULT CUIObject::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (1 <= m_UIDesc.iSubTextureNum)
		FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szSubTextureTag, TEXT("Com_SubTexture"), (CComponent**)&m_pSubTexturCom), E_FAIL);

	return S_OK;
}

void CUIObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	if (1 <= m_UIDesc.iSubTextureNum)
		Safe_Release(m_pSubTexturCom);

	CGameObject::Free();
}