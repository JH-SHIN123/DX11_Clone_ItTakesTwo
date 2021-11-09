#include "stdafx.h"
#include "..\public\Ortho_UIObject.h"
#include "GameInstance.h"

COrtho_UIObject::COrtho_UIObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

COrtho_UIObject::COrtho_UIObject(const COrtho_UIObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT COrtho_UIObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT COrtho_UIObject::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int COrtho_UIObject::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int COrtho_UIObject::Late_Tick(_double TimeDelta)
{
	NULL_CHECK_RETURN(m_pRendererCom, EVENT_ERROR);

	CGameObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT COrtho_UIObject::Render()
{
	CGameObject::Render();

	return S_OK;
}


HRESULT COrtho_UIObject::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);

	if (1 <= m_UIDesc.iSubTextureNum)
		FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szSubTextureTag, TEXT("Com_SubTexture"), (CComponent**)&m_pSubTexturCom), E_FAIL);

	return S_OK;
}

HRESULT CModel::Set_DefaultVariables_Perspective(_fmatrix WorldMatrix)
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	NULL_CHECK_RETURN(pPipeline, E_FAIL);

	Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));


	return S_OK;
}


COrtho_UIObject * COrtho_UIObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	COrtho_UIObject* pInstance = new COrtho_UIObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - COrtho_UIObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COrtho_UIObject::Clone_GameObject(void * pArg)
{
	COrtho_UIObject* pInstance = new COrtho_UIObject(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - COrtho_UIObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COrtho_UIObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	if (1 <= m_UIDesc.iSubTextureNum)
		Safe_Release(m_pSubTexturCom);

	CGameObject::Free();
}