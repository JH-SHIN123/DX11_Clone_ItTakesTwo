#include "stdafx.h"
#include "..\public\UFORadarScreen.h"
#include "Effect_Generator.h"

CUFORadarScreen::CUFORadarScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUFORadarSet(pDevice, pDeviceContext)
{
}

CUFORadarScreen::CUFORadarScreen(const CUFORadarSet & rhs)
	: CUFORadarSet(rhs)
{
}

HRESULT CUFORadarScreen::NativeConstruct_Prototype()
{
	CUFORadarSet::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUFORadarScreen::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFORadarScreen"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	if (nullptr != pArg)
		memcpy(&m_tRadarScreenDesc, (RADARDESC*)pArg, sizeof(RADARDESC));

	_vector vPosition = m_tRadarScreenDesc.vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	Set_MeshRenderGroup();
	return S_OK;
}

_int CUFORadarScreen::Tick(_double dTimeDelta)
{
	CUFORadarSet::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CUFORadarScreen::Late_Tick(_double dTimeDelta)
{
	CUFORadarSet::Late_Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CUFORadarScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CUFORadarSet::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	_uint iMaterialIndex = 0;
	m_pModelCom->Sepd_Bind_Buffer();

	iMaterialIndex = 0;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 1;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 2;
	m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 12, false, eGroup);

	return S_OK;
}

HRESULT CUFORadarScreen::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CUFORadarScreen::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CUFORadarScreen::Set_MeshRenderGroup()
{
	m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	return S_OK;
}

HRESULT CUFORadarScreen::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
	return S_OK;
}


CUFORadarScreen * CUFORadarScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFORadarScreen* pInstance = new CUFORadarScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUFORadarScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUFORadarScreen::Clone_GameObject(void * pArg)
{
	CUFORadarScreen* pInstance = new CUFORadarScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUFORadarScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUFORadarScreen::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}