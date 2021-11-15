#include "stdafx.h"
#include "..\Public\RespawnTunnel_Portal.h"

CRespawnTunnel_Portal::CRespawnTunnel_Portal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CRespawnTunnel_Portal::CRespawnTunnel_Portal(const CRespawnTunnel_Portal & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CRespawnTunnel_Portal::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CRespawnTunnel_Portal::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);
	Ready_Instance();

	return S_OK;
}

_int CRespawnTunnel_Portal::Tick(_double TimeDelta)
{
	return _int();
}

_int CRespawnTunnel_Portal::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CRespawnTunnel_Portal::Render()
{
	return S_OK;
}

HRESULT CRespawnTunnel_Portal::Ready_Instance()
{



	return S_OK;
}

CRespawnTunnel_Portal * CRespawnTunnel_Portal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRespawnTunnel_Portal*	pInstance = new CRespawnTunnel_Portal(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CRespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRespawnTunnel_Portal::Clone_GameObject(void * pArg)
{
	CRespawnTunnel_Portal* pInstance = new CRespawnTunnel_Portal(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRespawnTunnel_Portal::Free()
{
	__super::Free();
}
