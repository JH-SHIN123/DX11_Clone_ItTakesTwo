#include "stdafx.h"
#include "..\Public\RotationCylinder.h"
#include "Cody.h"

CRotationCylinder::CRotationCylinder(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CRotationCylinder::CRotationCylinder(const CRotationCylinder & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CRotationCylinder::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotationCylinder::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eELECTRICBOX;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(1.f, 70.f);

	return S_OK;
}

_int CRotationCylinder::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if(0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->RotatePitch_Speed(dTimeDelta);
	else
		m_pTransformCom->RotatePitch_Speed(-dTimeDelta);

	return NO_EVENT;
}

_int CRotationCylinder::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotationCylinder::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CRotationCylinder::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CRotationCylinder::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CRotationCylinder * CRotationCylinder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotationCylinder* pInstance = new CRotationCylinder(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CRotationCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRotationCylinder::Clone_GameObject(void * pArg)
{
	CRotationCylinder* pInstance = new CRotationCylinder(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CRotationCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotationCylinder::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
