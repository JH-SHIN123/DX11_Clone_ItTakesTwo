#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel_Smoke.h"
#include "GameInstance.h"

CEffect_RespawnTunnel_Smoke::CEffect_RespawnTunnel_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel_Smoke::CEffect_RespawnTunnel_Smoke(const CEffect_RespawnTunnel_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_RespawnTunnel_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel_Smoke::NativeConstruct(void * pArg) 
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_RespawnTunnel_Smoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	return _int();
}

_int CEffect_RespawnTunnel_Smoke::Late_Tick(_double TimeDelta)
{

	return __super::Late_Tick(TimeDelta);
}

HRESULT CEffect_RespawnTunnel_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	__super::Render(eGroup);



	return S_OK;
}

void CEffect_RespawnTunnel_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_RespawnTunnel_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_RespawnTunnel_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_RespawnTunnel_Smoke * CEffect_RespawnTunnel_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_RespawnTunnel_Smoke*	pInstance = new CEffect_RespawnTunnel_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel_Smoke* pInstance = new CEffect_RespawnTunnel_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel_Smoke::Free()
{
	__super::Free();
}
