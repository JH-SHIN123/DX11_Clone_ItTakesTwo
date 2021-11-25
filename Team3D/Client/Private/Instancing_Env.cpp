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
		memcpy(&m_Ins_Env_Desc, pArg, sizeof(ARG_DESC));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Ins_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_Ins_Env_Desc.Instancing_Arg), E_FAIL);

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

	if (0 < m_pModelCom->Culling())
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CInstancing_Env::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	m_pModelCom->Set_DefaultVariables_Shadow();

	m_pModelCom->Render_Model(0, m_Ins_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CInstancing_Env::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth();
	m_pModelCom->Render_Model(1, 0, true);

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
