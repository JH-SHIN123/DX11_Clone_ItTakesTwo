#include "stdafx.h"
#include "..\Public\AlienScreen.h"

CAlienScreen::CAlienScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CAlienScreen::CAlienScreen(const CAlienScreen & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CAlienScreen::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CAlienScreen::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eBlocked;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);
	return S_OK;
}

_int CAlienScreen::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CAlienScreen::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CAlienScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CAlienScreen::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CAlienScreen::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

CAlienScreen * CAlienScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAlienScreen* pInstance = new CAlienScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CAlienScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAlienScreen::Clone_GameObject(void * pArg)
{
	CAlienScreen* pInstance = new CAlienScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CAlienScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAlienScreen::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
