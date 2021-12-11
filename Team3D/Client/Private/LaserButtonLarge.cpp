#include "stdafx.h"
#include "..\Public\LaserButtonLarge.h"
#include "LaserTennis_Manager.h"
#include "Cody.h"
#include "May.h"

CLaserButtonLarge::CLaserButtonLarge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CLaserButtonLarge::CLaserButtonLarge(const CLaserButtonLarge & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CLaserButtonLarge::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserButtonLarge::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(3.f, 0.f);
	m_fMaxPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + m_fDistance;

	LASERTENNIS->Add_LaserButtonLarge(m_tDynamic_Env_Desc.iOption, this);

	return S_OK;
}

_int CLaserButtonLarge::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Activaion_Movement(dTimeDelta);
	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CLaserButtonLarge::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pTriggerActorCom->Update_TriggerActor(XMVectorSet(0.f, 0.5f, 0.f, 1.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserButtonLarge::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CLaserButtonLarge::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserButtonLarge::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		if (((((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Falling || (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land_Exit))
		{
			LASERTENNIS->Create_LaserTrigger_LargeButton(m_pTransformCom->Get_State(CTransform::STATE_POSITION), CLaserTennis_Manager::TARGET_MAY);
			m_bActiveMove = true;
			m_bActive = false;
		}
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		m_bCollision = false;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		if (((((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Falling || (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Land_Exit))
		{
			LASERTENNIS->Create_LaserTrigger_LargeButton(m_pTransformCom->Get_State(CTransform::STATE_POSITION), CLaserTennis_Manager::TARGET_CODY);
			m_bActiveMove = true;
			m_bActive = false;
		}
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		m_bCollision = false;
}

void CLaserButtonLarge::Movement(_double dTimeDelta)
{
	if (false == m_bCheckMovement)
		return;

	m_fDistance = 0.08f;
	m_pTransformCom->Set_Speed(2.f, 0.f);

	if (true == m_bCollision)
	{
		m_pTransformCom->Go_Down(dTimeDelta);
		if ((m_fMaxPosY - m_fDistance) >= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMaxPosY - m_fDistance));
	}
	else
	{
		m_pTransformCom->Go_Up(dTimeDelta);
		if (m_fMaxPosY <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMaxPosY));
	}
}

void CLaserButtonLarge::Activaion_Movement(_double dTimeDelta)
{
	if (false == m_bActiveMove)
		return;

	m_fDistance = 0.6f;

	/* 활성화 모션 */
	if (true == m_bActive)
	{
		m_pTransformCom->Set_Speed(4.f, 0.f);
		m_pTransformCom->Go_Up(dTimeDelta);
		if (m_fMaxPosY <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMaxPosY));
			m_bActiveMove = false;
			m_bCheckMovement = true;
		}
	}
	/* 버튼 눌렀을때 */
	else
	{
		m_pTransformCom->Set_Speed(4.f, 0.f);
		m_pTransformCom->Go_Down(dTimeDelta);
		if ((m_fMaxPosY - m_fDistance) <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMaxPosY - m_fDistance));
			m_bActiveMove = false;
			m_bCheckMovement = false;
		}
	}
}

HRESULT CLaserButtonLarge::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(0.8f, 0.05f, 0.8f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

CLaserButtonLarge * CLaserButtonLarge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserButtonLarge* pInstance = new CLaserButtonLarge(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserButtonLarge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserButtonLarge::Clone_GameObject(void * pArg)
{
	CLaserButtonLarge* pInstance = new CLaserButtonLarge(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserButtonLarge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserButtonLarge::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
