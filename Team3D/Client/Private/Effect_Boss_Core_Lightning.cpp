#include "stdafx.h"
#include "..\Public\Effect_Boss_Core_Lightning.h"

CEffect_Boss_Core_Lightning::CEffect_Boss_Core_Lightning(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Core_Lightning::CEffect_Boss_Core_Lightning(const CEffect_Boss_Core_Lightning & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Core_Lightning::NativeConstruct_Prototype(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CEffect_Boss_Core_Lightning::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

_int CEffect_Boss_Core_Lightning::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Boss_Core_Lightning::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Boss_Core_Lightning::Render(RENDER_GROUP::Enum eGroup)
{
	return E_NOTIMPL;
}

void CEffect_Boss_Core_Lightning::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core_Lightning::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core_Lightning::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Boss_Core_Lightning * CEffect_Boss_Core_Lightning::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Core_Lightning*	pInstance = new CEffect_Boss_Core_Lightning(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Core_Lightning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Core_Lightning::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Core_Lightning* pInstance = new CEffect_Boss_Core_Lightning(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Core_Lightning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Core_Lightning::Free()
{
	__super::Free();
}
