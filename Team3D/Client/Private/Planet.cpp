#include "stdafx.h"
#include "..\Public\Planet.h"

CPlanet::CPlanet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPlanet::CPlanet(const CPlanet & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPlanet::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlanet::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(5.f, 0.5f);

	return S_OK;
}

_int CPlanet::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), dTimeDelta);
	m_pStaticActorCom->Update_StaticActor();

	return NO_EVENT;
}

_int CPlanet::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPlanet::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(8, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPlanet::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

HRESULT CPlanet::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CPlanet * CPlanet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlanet* pInstance = new CPlanet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlanet::Clone_GameObject(void * pArg)
{
	CPlanet* pInstance = new CPlanet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlanet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlanet::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
