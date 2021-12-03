#include "stdafx.h"
#include "..\Public\PlanetRing.h"

CPlanetRing::CPlanetRing(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPlanetRing::CPlanetRing(const CPlanetRing & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPlanetRing::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlanetRing::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(0.f, 1.f);

	return S_OK;
}

_int CPlanetRing::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if (m_tDynamic_Env_Desc.iOption == 0)
		m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta);
	else if(m_tDynamic_Env_Desc.iOption == 1)
		m_pTransformCom->Rotate_Axis((m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -1.f), dTimeDelta);
	else
		m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);

	return NO_EVENT;
}

_int CPlanetRing::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPlanetRing::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPlanetRing::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, m_tDynamic_Env_Desc.iMatrialIndex, true);

	return S_OK;
}

HRESULT CPlanetRing::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

CPlanetRing * CPlanetRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlanetRing* pInstance = new CPlanetRing(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlanetRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlanetRing::Clone_GameObject(void * pArg)
{
	CPlanetRing* pInstance = new CPlanetRing(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlanetRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlanetRing::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
