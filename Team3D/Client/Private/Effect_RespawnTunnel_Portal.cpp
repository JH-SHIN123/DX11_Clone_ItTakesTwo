#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel_Portal.h"
#include "GameInstance.h"

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(const CEffect_RespawnTunnel_Portal & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_RespawnTunnel_Portal::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_RespawnTunnel_Portal::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_RespawnTunnel_Portal::Render()
{
	NULL_CHECK_RETURN(m_pModelInstanceCom, E_FAIL);

	m_pModelInstanceCom->Set_DefaultVariables_Perspective();
	m_pModelInstanceCom->Render_Model(0);

	return S_OK;
}

void CEffect_RespawnTunnel_Portal::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_RespawnTunnel_Portal::Ready_Instance()
{



	return S_OK;
}

CEffect_RespawnTunnel_Portal * CEffect_RespawnTunnel_Portal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel_Portal*	pInstance = new CEffect_RespawnTunnel_Portal(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel_Portal::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel_Portal* pInstance = new CEffect_RespawnTunnel_Portal(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel_Portal::Free()
{
	__super::Free();
}
