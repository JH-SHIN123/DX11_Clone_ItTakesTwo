#include "stdafx.h"
#include "..\Public\PlanetRing.h"
#include "GameInstance.h"

CPlanetRing::CPlanetRing(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlanetRing::CPlanetRing(const CPlanetRing & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlanetRing::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlanetRing::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CPlanetRing::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CPlanetRing::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPlanetRing::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CPlanetRing::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PlanetRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

HRESULT CPlanetRing::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);

	return S_OK;
}

CPlanetRing * CPlanetRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlanetRing* pInstance = new CPlanetRing(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create - CPlanetRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlanetRing::Clone_GameObject(void * pArg)
{
	CPlanetRing* pInstance = new CPlanetRing(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone - CPlanetRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlanetRing::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
