#include "stdafx.h"
#include "..\Public\Test_EnvObject.h"
#include "GameInstance.h"
#include "FireDoor.h"

CTest_EnvObject::CTest_EnvObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTest_EnvObject::CTest_EnvObject(const CTest_EnvObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_EnvObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTest_EnvObject::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Effect", Level::LEVEL_STAGE, L"GameObject_2D_FireDoor", nullptr, (CGameObject**)&m_pFireDoor);


	CModel_Instance::ARG_DESC Arg;
	Arg.iInstanceCount = 1;
	Arg.fCullingRadius = 10.f;
	Arg.pActorName = "TileBox";
	Arg.pWorldMatrices = new _float4x4[Arg.iInstanceCount];
	m_pFireDoor->SetUp_InstanceCount(1);

	for (_uint i = 0; i < Arg.iInstanceCount; ++i)
	{
		Arg.pWorldMatrices[i] = MH_XMFloat4x4Identity();
		Arg.pWorldMatrices[i]._41 = 10.f;//((i % 100) * 10.f);
		Arg.pWorldMatrices[i]._43 = 10.f;//((i / 100) * 10.f);
		m_pFireDoor->SetUp_Instance_WorldMatrix(i, Arg.pWorldMatrices[i]);
	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_TileBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &Arg), E_FAIL);

	return S_OK;
}

_int CTest_EnvObject::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CTest_EnvObject::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTest_EnvObject::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

CTest_EnvObject * CTest_EnvObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTest_EnvObject* pInstance = new CTest_EnvObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTest_EnvObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTest_EnvObject::Clone_GameObject(void * pArg)
{
	CTest_EnvObject* pInstance = new CTest_EnvObject(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTest_EnvObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_EnvObject::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFireDoor);
	CGameObject::Free();
}