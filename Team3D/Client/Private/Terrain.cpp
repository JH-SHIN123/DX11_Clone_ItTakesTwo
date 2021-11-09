#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);

	return S_OK;
}

_int CTerrain::Tick(_double TimeDelta)
{
	return NO_EVENT;
}

_int CTerrain::Late_Tick(_double TimeDelta)
{
	NULL_CHECK_RETURN(m_pRendererCom, EVENT_ERROR);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTerrain::Render()
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);

	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CTerrain::Set_ShaderConstant_Default()
{
	return S_OK;
}

HRESULT CTerrain::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone_GameObject(void * pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}