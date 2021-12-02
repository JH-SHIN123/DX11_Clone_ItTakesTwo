#include "stdafx.h"
#include "..\Public\HookahTube.h"
#include "Cody.h"
#include "May.h"

CHookahTube::CHookahTube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CHookahTube::CHookahTube(const CHookahTube & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CHookahTube::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CHookahTube::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eHOOKAHTUBE;
	m_UserData.pGameObject = this;

	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* geom = new PxSphereGeometry(2.5f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(geom);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) - 0.7f);

	m_pTriggerActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(vPosition)));

	return S_OK;
}

_int CHookahTube::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CHookahTube::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CHookahTube::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CHookahTube::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CHookahTube::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eHOOKAHTUBE, true, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = true;
	}
	/* May */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eHOOKAHTUBE, true, ((CMay*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = true;
	}
}

void CHookahTube::Movement(_double dTimeDelta)
{
	if (false == m_bTrigger)
		return;

	if (false == m_bChange)
	{
		_float	fScale = (_float)dTimeDelta;
		m_fScale += fScale;

		if (m_fScale >= 1.1f)
		{
			m_fScale = 1.1f;
			fScale = 0.f;
			m_bChange = true;
		}
	}
	else
	{
		_float	fScale = (_float)dTimeDelta;
		m_fScale -= fScale;

		if (m_fScale <= 1.f)
		{
			m_fScale = 1.f;
			fScale = 0.f;
			m_bChange = false;
			m_bTrigger = false;
		}
	}
	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
}

CHookahTube * CHookahTube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHookahTube* pInstance = new CHookahTube(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CHookahTube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHookahTube::Clone_GameObject(void * pArg)
{
	CHookahTube* pInstance = new CHookahTube(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CHookahTube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHookahTube::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
