#include "stdafx.h"
#include "..\Public\BossDoor.h"
#include "PlayerActor.h"
#include "Cody.h"
#include "May.h"


CBossDoor::CBossDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CBossDoor::CBossDoor(const CBossDoor & rhs)
	: CDynamic_Env(rhs)
{
}

void CBossDoor::Set_DoorClose()
{
	m_bCloseDoor = true;
	m_fSpeed = 25.f;
	m_pTransformCom->Set_Speed(m_fSpeed, 0.f);
}

void CBossDoor::Set_DoorUp(_float fMaxDistance, _float fSpeed)
{
	XMStoreFloat3(&m_vMaxPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vMaxPos.y += fMaxDistance;

	m_fMaxY = fMaxDistance;
	m_bGoUp = true;
	m_fSpeed = fSpeed;
	m_pTransformCom->Set_Speed(m_fSpeed, 0.f);
	((CCody*)DATABASE->GetCody())->Get_Actor()->Set_Gravity(-6.0f);
	((CMay*)DATABASE->GetMay())->Get_Actor()->Set_Gravity(-6.0f);
}

HRESULT CBossDoor::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBossDoor::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	if(1 == m_tDynamic_Env_Desc.iOption)
		DATABASE->Set_BossDoor01(this);
	else
		DATABASE->Set_BossDoor02(this);

	return S_OK;
}

_int CBossDoor::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CBossDoor::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 500.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBossDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CBossDoor::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CBossDoor::Movement(_double dTimeDelta)
{
	Close_Door(dTimeDelta);
	GoUp(dTimeDelta);
}

void CBossDoor::Close_Door(_double dTimeDelta)
{
	if (false == m_bCloseDoor || true == m_bClose || 0 == m_tDynamic_Env_Desc.iOption)
		return;

	_float fDist = (_float)dTimeDelta * m_fSpeed;
	m_fDistance += fDist;

	if (25.2f <= m_fDistance)
	{
		m_fDistance = 0.f;
		m_bCloseDoor = false;
		m_bClose = true;
		return;
	}

	m_pTransformCom->Go_Right(dTimeDelta);
}

void CBossDoor::GoUp(_double dTimeDelta)
{
	if (false == m_bGoUp)
		return;

	m_pTransformCom->Set_Speed(m_fSpeed, 0.f);

	_float fDist = (_float)dTimeDelta * m_fSpeed;
	m_fDistance += fDist;

	if (m_fMaxY <= m_fDistance)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vMaxPos), 1.f));
		m_fMaxY = 0.f;
		m_bGoUp = false;
		m_fDistance = 0.f;
		((CCody*)DATABASE->GetCody())->Get_Actor()->Set_Gravity(-9.8f);
		((CMay*)DATABASE->GetMay())->Get_Actor()->Set_Gravity(-9.8f);
		return;
	}

	m_pTransformCom->Go_Up(dTimeDelta);
}

HRESULT CBossDoor::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CBossDoor * CBossDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossDoor* pInstance = new CBossDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBossDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossDoor::Clone_GameObject(void * pArg)
{
	CBossDoor* pInstance = new CBossDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBossDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossDoor::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
