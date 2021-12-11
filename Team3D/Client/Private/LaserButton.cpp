#include "stdafx.h"
#include "..\Public\LaserButton.h"
#include "LaserTennis_Manager.h"

CLaserButton::CLaserButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CLaserButton::CLaserButton(const CLaserButton & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CLaserButton::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserButton::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(1.f, 0.f);
	m_fResetPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	LASERTENNIS->Add_LaserButton(this);

	return S_OK;
}

_int CLaserButton::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CLaserButton::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pTriggerActorCom->Update_TriggerActor(XMVectorSet(0.f, 0.5f, 0.f, 1.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserButton::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	/* Active, Option에 따라 렌더 다르게 해야함 */
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CLaserButton::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserButton::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		if (true == m_bActive)
		{
			LASERTENNIS->Create_LaserTrigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION), CLaserTennis_Manager::TARGET_MAY);
			m_bActive = false;
		}
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		m_bCollision = false;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		if (true == m_bActive)
		{
			LASERTENNIS->Create_LaserTrigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION), CLaserTennis_Manager::TARGET_CODY);
			m_bActive = false;
		}
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		m_bCollision = false;
}

void CLaserButton::Movement(_double dTimeDelta)
{
	if (true == m_bCollision)
	{
		m_pTransformCom->Go_Down(dTimeDelta);
		if ((m_fResetPosY - m_fDistance) >= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosY - m_fDistance));
	}
	else
	{
		m_pTransformCom->Go_Up(dTimeDelta);
		if (m_fResetPosY <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosY));
	}
}

HRESULT CLaserButton::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(0.7f, 0.05f, 0.7f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

CLaserButton * CLaserButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserButton* pInstance = new CLaserButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserButton::Clone_GameObject(void * pArg)
{
	CLaserButton* pInstance = new CLaserButton(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserButton::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
