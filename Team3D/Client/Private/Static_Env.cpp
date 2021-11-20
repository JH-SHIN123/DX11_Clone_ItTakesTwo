#include "stdafx.h"
#include "..\Public\Static_Env.h"
#include "GameInstance.h"


CStatic_Env::CStatic_Env(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStatic_Env::CStatic_Env(const CStatic_Env & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStatic_Env::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStatic_Env::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_Static_Env_Desc, pArg, sizeof(STATIC_ENV_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Static_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_Static_Env_Desc.WorldMatrix));


	//PxTriangleMesh* pMesh = m_pGameInstance->Create_PxMesh(m_pModelCom->Get_MeshActorDesc());
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &CStaticActor::ARG_DESC(m_pTransformCom->Get_WorldMatrix(), &PxTriangleMeshGeometry(pMesh), m_pGameInstance->Create_PxMaterial(0.5f, 0.5f, 0.f), m_Static_Env_Desc.pActorName)), E_FAIL);
	//pMesh->release();

	return S_OK;
}

_int CStatic_Env::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CStatic_Env::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
	//return m_pRendererCom->Add_GameObject_ToRenderGroup((CRenderer::RENDER_GROUP)m_Ins_Env_Desc.RenderGroup, this);

	return NO_EVENT;
}

HRESULT CStatic_Env::Render()
{
	CGameObject::Render();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(1);
	//m_pModelCom->Render_Model(m_Ins_Env_Desc.ShaderPass);

	return S_OK;
}

CStatic_Env * CStatic_Env::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStatic_Env* pInstance = new CStatic_Env(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CStatic_Env");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatic_Env::Clone_GameObject(void * pArg)
{
	CStatic_Env* pInstance = new CStatic_Env(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CStatic_Env");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatic_Env::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
