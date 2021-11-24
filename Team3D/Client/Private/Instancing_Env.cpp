#include "stdafx.h"
#include "..\Public\Instancing_Env.h"

CInstancing_Env::CInstancing_Env(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInstancing_Env::CInstancing_Env(const CInstancing_Env & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInstancing_Env::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CInstancing_Env::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_Ins_Env_Desc, pArg, sizeof(INS_ENV_DESC));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Ins_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_Ins_Env_Desc.Instancing_Arg), E_FAIL);

	Set_MeshRenderGroup();

	return S_OK;
}

_int CInstancing_Env::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CInstancing_Env::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CInstancing_Env::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	m_pModelCom->Set_DefaultVariables_Shadow();

	// 모델 태그로 구분 : szModelTag
	if (!lstrcmp(TEXT("Component_Model_GlassWall_Beveled"), m_Ins_Env_Desc.szModelTag) ||
		!lstrcmp(TEXT("Component_Model_GlassWall01"), m_Ins_Env_Desc.szModelTag) ||
		!lstrcmp(TEXT("Component_Model_GlassWall01_Half"), m_Ins_Env_Desc.szModelTag))
	{
		_uint iRenderCount = m_pModelCom->Frustum_Culling();
		m_pModelCom->Bind_GBuffers(iRenderCount);

		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", 0, aiTextureType_DIFFUSE, m_Ins_Env_Desc.iMaterialIndex);
		//m_pModelCom->Set_ShaderResourceView("g_NormalTexture", 0, aiTextureType_NORMALS, m_Ins_Env_Desc.iMaterialIndex);
		//m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", 0, aiTextureType_SPECULAR, m_Ins_Env_Desc.iMaterialIndex);
		//m_pModelCom->Set_ShaderResourceView("g_ReflectTexture", 0, aiTextureType_REFLECTION, m_Ins_Env_Desc.iMaterialIndex);
		m_pModelCom->Render_ModelByPass(iRenderCount, 0, 2, false, tagRenderGroup::RENDER_ALPHA);

		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", 1, aiTextureType_DIFFUSE, m_Ins_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", 1, aiTextureType_NORMALS, m_Ins_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", 1, aiTextureType_SPECULAR, m_Ins_Env_Desc.iMaterialIndex);
		m_pModelCom->Render_ModelByPass(iRenderCount, 1, 0, false, tagRenderGroup::RENDER_NONALPHA);
	}
	else
	{
		m_pModelCom->Render_Model(0, m_Ins_Env_Desc.iMaterialIndex);
	}

	return S_OK;
}

HRESULT CInstancing_Env::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth();
	m_pModelCom->Render_Model(1, 0, true);

	return S_OK;
}

HRESULT CInstancing_Env::Set_MeshRenderGroup()
{
	if (!lstrcmp(TEXT("Component_Model_GlassWall_Beveled"), m_Ins_Env_Desc.szModelTag) ||
		!lstrcmp(TEXT("Component_Model_GlassWall01"), m_Ins_Env_Desc.szModelTag) || 
		!lstrcmp(TEXT("Component_Model_GlassWall01_Half"), m_Ins_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	}

	return S_OK;
}

HRESULT CInstancing_Env::Add_GameObject_ToRenderGroup()
{
	if (!lstrcmp(TEXT("Component_Model_GlassWall_Beveled"), m_Ins_Env_Desc.szModelTag) ||
		!lstrcmp(TEXT("Component_Model_GlassWall01"), m_Ins_Env_Desc.szModelTag) ||
		!lstrcmp(TEXT("Component_Model_GlassWall01_Half"), m_Ins_Env_Desc.szModelTag))
	{
		m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
		m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
	}
	else 
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return S_OK;
}

CInstancing_Env * CInstancing_Env::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CInstancing_Env* pInstance = new CInstancing_Env(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CInstancing_Env");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInstancing_Env::Clone_GameObject(void * pArg)
{
	CInstancing_Env* pInstance = new CInstancing_Env(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CInstancing_Env");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancing_Env::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
