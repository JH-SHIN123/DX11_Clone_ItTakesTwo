#include "stdafx.h"
#include "..\Public\Effect_Wormhole.h"
#include "GameInstance.h"

CEffect_Wormhole::CEffect_Wormhole(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Wormhole::CEffect_Wormhole(const CEffect_Wormhole & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_Wormhole::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Wormhole::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_Wormhole::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Wormhole::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Wormhole::Render()
{
	return S_OK;
}

void CEffect_Wormhole::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_Wormhole::Ready_Instance()
{
	return S_OK;
}

CEffect_Wormhole * CEffect_Wormhole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Wormhole*	pInstance = new CEffect_Wormhole(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Wormhole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Wormhole::Clone_GameObject(void * pArg)
{
	CEffect_Wormhole* pInstance = new CEffect_Wormhole(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Wormhole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Wormhole::Free()
{
	__super::Free();
}
