#include "stdafx.h"
#include "..\Public\Effect_Boss_Core_Explosion.h"

CEffect_Boss_Core_Explosion::CEffect_Boss_Core_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Core_Explosion::CEffect_Boss_Core_Explosion(const CEffect_Boss_Core_Explosion & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Core_Explosion::NativeConstruct_Prototype(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CEffect_Boss_Core_Explosion::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

_int CEffect_Boss_Core_Explosion::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Boss_Core_Explosion::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Boss_Core_Explosion::Render(RENDER_GROUP::Enum eGroup)
{
	return E_NOTIMPL;
}

void CEffect_Boss_Core_Explosion::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core_Explosion::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core_Explosion::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Boss_Core_Explosion * CEffect_Boss_Core_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Core_Explosion*	pInstance = new CEffect_Boss_Core_Explosion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Core_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Core_Explosion::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Core_Explosion* pInstance = new CEffect_Boss_Core_Explosion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Core_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Core_Explosion::Free()
{
	__super::Free();
}
