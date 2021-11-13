#include "stdafx.h"
#include "..\public\TileBox.h"
#include "GameInstance.h"

CTileBox::CTileBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTileBox::CTileBox(const CTileBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTileBox::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTileBox::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	CModel_Instance::ARG_DESC Arg;
	Arg.iInstanceCount = 500;
	Arg.fCullingRadius = 10.f;
	Arg.pActorName = "TileBox";
	Arg.pWorldMatrices = new _float4x4[Arg.iInstanceCount];

	for (_uint i = 0; i < Arg.iInstanceCount; ++i)
	{
		Arg.pWorldMatrices[i] = MH_XMFloat4x4Identity();
		Arg.pWorldMatrices[i]._41 = ((i % 100) * 10.f);
		Arg.pWorldMatrices[i]._43 = ((i / 100) * 10.f);
	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_TileBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &Arg), E_FAIL);

	return S_OK;
}

_int CTileBox::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CTileBox::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTileBox::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

CTileBox * CTileBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTileBox* pInstance = new CTileBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTileBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTileBox::Clone_GameObject(void * pArg)
{
	CTileBox* pInstance = new CTileBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTileBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTileBox::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}