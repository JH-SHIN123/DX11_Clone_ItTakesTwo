#include "stdafx.h"
#include "..\Public\BossSlideDoor.h"

_bool	CBossSlideDoor::m_bCloseDoor = false;

CBossSlideDoor::CBossSlideDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CBossSlideDoor::CBossSlideDoor(const CBossSlideDoor & rhs)
	: CDynamic_Env(rhs)
{
}

void CBossSlideDoor::Set_DoorClose()
{
	m_bCloseDoor = true;
}

HRESULT CBossSlideDoor::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBossSlideDoor::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(5.f, 0.f);
	DATABASE->Set_BossSlideDoor(this);

	return S_OK;
}

_int CBossSlideDoor::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Close_Door(dTimeDelta);

	return NO_EVENT;
}

_int CBossSlideDoor::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBossSlideDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CBossSlideDoor::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CBossSlideDoor::Close_Door(_double dTimeDelta)
{
	if (false == m_bCloseDoor)
		return;

	_float fDist = (_float)dTimeDelta * 5.f;
	m_fDistance += fDist;

	if (12.f <= m_fDistance)
	{
		m_fDistance = 0.f;
		m_bCloseDoor = false;
		return;
	}

	if(0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->Go_Right(dTimeDelta);
	else
		m_pTransformCom->Go_Right(-dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CBossSlideDoor::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CBossSlideDoor * CBossSlideDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossSlideDoor* pInstance = new CBossSlideDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBossSlideDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossSlideDoor::Clone_GameObject(void * pArg)
{
	CBossSlideDoor* pInstance = new CBossSlideDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBossSlideDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossSlideDoor::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
