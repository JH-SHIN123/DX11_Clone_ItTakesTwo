#include "stdafx.h"
#include "..\Public\Static_Env.h"

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

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	Set_MeshRenderGroup();
	Set_Simulation();

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

	Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CStatic_Env::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	_uint iMaterialIndex = 0;
	if (!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_01"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		/* 렌더순서 주의 - 논알파 -> 알파 */
		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		// 4: Alpha / 5: GlassWall Custom Alpha
		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 5, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_02"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 2;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 5, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_GlassWall_End"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 2;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_Space_Pinball_Frame"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_Space_Pinball_BaseStar"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_Saturn"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

		iMaterialIndex = 2;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 8, false, eGroup);

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);
	}
	else if (!lstrcmp(TEXT("Component_Model_PlanetFloor"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Sepd_Bind_Buffer();

		iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 8, false, eGroup);

		iMaterialIndex = 2;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, m_Static_Env_Desc.iMaterialIndex);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);
	}
	else if(!lstrcmp(TEXT("Component_Model_Planet"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_LavaLamp"), m_Static_Env_Desc.szModelTag))
		m_pModelCom->Render_Model(8, m_Static_Env_Desc.iMaterialIndex);
	else
		m_pModelCom->Render_Model(1, m_Static_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CStatic_Env::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, m_Static_Env_Desc.iMaterialIndex, true);

	return S_OK;
}

HRESULT CStatic_Env::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_Static_Env_Desc, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Static_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_Static_Env_Desc.WorldMatrix));
	m_UserData.eID = m_Static_Env_Desc.eGameID;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

HRESULT CStatic_Env::Set_Simulation()
{
	/* 피직스 시뮬레이션에서 제외해야하는 경우 */
	if (!lstrcmp(TEXT("Component_Model_Space_Pinball_Attachment"), m_Static_Env_Desc.szModelTag))
		m_pStaticActorCom->Set_Simulation(true);

	return S_OK;
}

HRESULT CStatic_Env::Set_MeshRenderGroup()
{
	if (!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_01"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_02"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
		m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_GlassWall_End"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_Space_Pinball_Frame"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_Space_Pinball_BaseStar"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_NONALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_ALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_Saturn"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_NONALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	}
	else if (!lstrcmp(TEXT("Component_Model_PlanetFloor"), m_Static_Env_Desc.szModelTag))
	{
		m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
		m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
		m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	}

	return S_OK;
}

HRESULT CStatic_Env::Add_GameObject_ToRenderGroup()
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_Static_Env_Desc.fCullRadius))
	{
		if (!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_01"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_MoonBaboon_GlassWall_02"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_GlassWall_End"), m_Static_Env_Desc.szModelTag) || 
			!lstrcmp(TEXT("Component_Model_Space_Pinball_Frame"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_Space_Pinball_BaseStar"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_Saturn"), m_Static_Env_Desc.szModelTag) ||
			!lstrcmp(TEXT("Component_Model_PlanetFloor"), m_Static_Env_Desc.szModelTag))
		{
			m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
			m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
		}
		else m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

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
