#include "stdafx.h"
#include "..\Public\RespawnTunnel_Smoke.h"
#include "GameInstance.h"

CRespawnTunnel_Smoke::CRespawnTunnel_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CRespawnTunnel_Smoke::CRespawnTunnel_Smoke(const CRespawnTunnel_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CRespawnTunnel_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CRespawnTunnel_Smoke::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CRespawnTunnel_Smoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	return _int();
}

_int CRespawnTunnel_Smoke::Late_Tick(_double TimeDelta)
{

	return __super::Late_Tick(TimeDelta);
}

HRESULT CRespawnTunnel_Smoke::Render()
{
	__super::Render();



	return S_OK;
}

CRespawnTunnel_Smoke * CRespawnTunnel_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CRespawnTunnel_Smoke*	pInstance = new CRespawnTunnel_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CRespawnTunnel_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRespawnTunnel_Smoke::Clone_GameObject(void * pArg)
{
	CRespawnTunnel_Smoke* pInstance = new CRespawnTunnel_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRespawnTunnel_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRespawnTunnel_Smoke::Free()
{
	__super::Free();
}
