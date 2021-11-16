#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel.h"
#include "GameInstance.h"

CEffect_RespawnTunnel::CEffect_RespawnTunnel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel::CEffect_RespawnTunnel(const CEffect_RespawnTunnel & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_RespawnTunnel::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_RespawnTunnel::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_RespawnTunnel::Render()
{
	NULL_CHECK_RETURN(m_pModelInstanceCom, E_FAIL);

	m_pModelInstanceCom->Set_DefaultVariables_Perspective();
	m_pModelInstanceCom->Render_Model(0);

	return S_OK;
}

void CEffect_RespawnTunnel::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

CEffect_RespawnTunnel * CEffect_RespawnTunnel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel*	pInstance = new CEffect_RespawnTunnel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel* pInstance = new CEffect_RespawnTunnel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel::Free()
{
	__super::Free();
}
