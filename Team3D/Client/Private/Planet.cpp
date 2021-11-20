#include "stdafx.h"
#include "..\Public\Planet.h"
#include "GameInstance.h"


CPlanet::CPlanet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlanet::CPlanet(const CPlanet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlanet::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlanet::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int CPlanet::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CPlanet::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPlanet::Render()
{
	CGameObject::Render();

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CPlanet::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PlanetRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

HRESULT CPlanet::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);

	return S_OK;
}

CPlanet * CPlanet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlanet* pInstance = new CPlanet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create - CPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlanet::Clone_GameObject(void * pArg)
{
	CPlanet* pInstance = new CPlanet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone - CPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlanet::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
