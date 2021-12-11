#include "stdafx.h"
#include "..\Public\LaserActivation.h"
#include "LaserTennis_Manager.h"

CLaserActivation::CLaserActivation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CLaserActivation::CLaserActivation(const CLaserActivation & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CLaserActivation::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserActivation::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(5.f, 0.f);
	m_fMinPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fMaxPosY = m_fMinPosY + m_fDistance;

	LASERTENNIS->Add_LaserActivation(this);

	return S_OK;
}

_int CLaserActivation::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CLaserActivation::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserActivation::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CLaserActivation::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserActivation::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);
}

void CLaserActivation::Movement(_double dTimeDelta)
{
	if (false == m_bMovement)
		return;

	if (false == m_bActivate)/* 활성화 */
	{
		m_fDistanceCheck += (_float)(dTimeDelta * m_pTransformCom->Get_SpeedPerSec());
		if (m_fDistance <= m_fDistanceCheck)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMaxPosY));
			m_fDistanceCheck = 0.f;
			m_bActivate = true;
			m_bMovement = false;
			return;
		}
		m_pTransformCom->Go_Up(dTimeDelta);
	}
	else/* 비활성화 */
	{
		m_fDistanceCheck += (_float)(dTimeDelta * m_pTransformCom->Get_SpeedPerSec());
		if (m_fDistance <= m_fDistanceCheck)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMinPosY));
			m_fDistanceCheck = 0.f;
			m_bActivate = false;
			m_bMovement = false;
			return;
		}
		m_pTransformCom->Go_Down(dTimeDelta);
	}
}

void CLaserActivation::CreateLaser(_double dTimeDelta)
{
}

HRESULT CLaserActivation::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CLaserActivation * CLaserActivation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserActivation* pInstance = new CLaserActivation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserActivation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserActivation::Clone_GameObject(void * pArg)
{
	CLaserActivation* pInstance = new CLaserActivation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserActivation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserActivation::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
