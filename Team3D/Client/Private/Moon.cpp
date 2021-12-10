#include "stdafx.h"
#include "..\Public\Moon.h"

CMoon::CMoon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CMoon::CMoon(const CMoon & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CMoon::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoon::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	DATABASE->Set_Moon(this);

	return S_OK;
}

_int CMoon::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CMoon::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 500.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CMoon::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CMoon::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CMoon::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CMoon * CMoon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoon* pInstance = new CMoon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoon::Clone_GameObject(void * pArg)
{
	CMoon* pInstance = new CMoon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoon::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
