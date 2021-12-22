#include "stdafx.h"
#include "..\Public\ControlRoom_Monitor.h"

CControlRoom_Monitor::CControlRoom_Monitor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CControlRoom_Monitor::CControlRoom_Monitor(const CControlRoom_Monitor & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CControlRoom_Monitor::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControlRoom_Monitor::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	DATABASE->Set_ControlRoomMonitor(this);

	return S_OK;
}

_int CControlRoom_Monitor::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CControlRoom_Monitor::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CControlRoom_Monitor::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Sepd_Bind_Buffer();

	_uint iMaterialIndex = 0;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false);

	iMaterialIndex = 1;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false);

	iMaterialIndex = 2;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_iFrame));
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, m_tDynamic_Env_Desc.iMaterialIndex);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 34, false);

	return S_OK;
}

HRESULT CControlRoom_Monitor::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CControlRoom_Monitor::Ready_Component(void * pArg)
{
	/* Texture */
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ControlRoom01"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom), E_FAIL);

	return S_OK;
}

CControlRoom_Monitor * CControlRoom_Monitor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControlRoom_Monitor* pInstance = new CControlRoom_Monitor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControlRoom_Monitor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CControlRoom_Monitor::Clone_GameObject(void * pArg)
{
	CControlRoom_Monitor* pInstance = new CControlRoom_Monitor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControlRoom_Monitor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CControlRoom_Monitor::Free()
{
	Safe_Release(m_pTextureCom);

	CDynamic_Env::Free();
}
