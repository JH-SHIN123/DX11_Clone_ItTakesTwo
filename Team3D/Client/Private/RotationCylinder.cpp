#include "stdafx.h"
#include "..\Public\RotationCylinder.h"
#include "Cody.h"

CRotaionCylinder::CRotaionCylinder(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CRotaionCylinder::CRotaionCylinder(const CRotaionCylinder & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CRotaionCylinder::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotaionCylinder::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eELECTRICBOX;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(1.f, 70.f);

	return S_OK;
}

_int CRotaionCylinder::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if(0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->RotateRoll_Speed(dTimeDelta);
	else
		m_pTransformCom->RotateRoll_Speed(-dTimeDelta);

	return NO_EVENT;
}

_int CRotaionCylinder::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotaionCylinder::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CRotaionCylinder::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CRotaionCylinder::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CRotaionCylinder * CRotaionCylinder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotaionCylinder* pInstance = new CRotaionCylinder(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CRotaionCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRotaionCylinder::Clone_GameObject(void * pArg)
{
	CRotaionCylinder* pInstance = new CRotaionCylinder(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CRotaionCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotaionCylinder::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
