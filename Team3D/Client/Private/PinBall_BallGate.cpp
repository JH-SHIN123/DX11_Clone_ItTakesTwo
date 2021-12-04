#include "stdafx.h"
#include "..\Public\PinBall_BallGate.h"
#include "PinBall.h"

CPinBall_BallGate::CPinBall_BallGate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall_BallGate::CPinBall_BallGate(const CPinBall_BallGate & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPinBall_BallGate::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall_BallGate::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CPinBall_BallGate::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);
	return NO_EVENT;
}

_int CPinBall_BallGate::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall_BallGate::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPinBall_BallGate::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall_BallGate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* PinBall */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::ePINBALL && false == m_bGoal)
	{
		((CPinBall*)pGameObject)->Goal(XMVectorSet(-672.6f, 755.9f, -163.f, 1.f));
		m_bGoal = true;
	}
}

HRESULT CPinBall_BallGate::Ready_Component(void * pArg)
{
	/* Trigger */
	PxGeometry* Geom = new PxSphereGeometry(1.f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = Geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(Geom);

	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CPinBall_BallGate * CPinBall_BallGate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall_BallGate* pInstance = new CPinBall_BallGate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall_BallGate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall_BallGate::Clone_GameObject(void * pArg)
{
	CPinBall_BallGate* pInstance = new CPinBall_BallGate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall_BallGate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall_BallGate::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
