#include "stdafx.h"
#include "..\Public\LaserPowerCoord.h"
#include "LaserTennis_Manager.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"

CLaserPowerCoord::CLaserPowerCoord(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CLaserPowerCoord::CLaserPowerCoord(const CLaserPowerCoord & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CLaserPowerCoord::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserPowerCoord::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eLASERTENNISPOWERCOORD;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(5.f, 0.f);
	m_fResetPosZ = XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	LASERTENNIS->Add_LaserPowerCoord(m_tDynamic_Env_Desc.iOption, this);

	return S_OK;
}

_int CLaserPowerCoord::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//if(0 == m_tDynamic_Env_Desc.iOption)
	//	UI_Generator->CreateInterActiveUI_AccordingRange(Player::Cody, UI::StarBuddy, XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) - 3.f), 3.f, m_bCollision);
	//else
	//	UI_Generator->CreateInterActiveUI_AccordingRange(Player::May, UI::StarBuddy, XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) - 3.f), 3.f, m_bCollision);

	return NO_EVENT;
}

_int CLaserPowerCoord::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pTriggerActorCom->Update_TriggerActor(XMVectorSet(0.f, 0.f, -3.f, 1.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserPowerCoord::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);
		
	return S_OK;
}

HRESULT CLaserPowerCoord::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserPowerCoord::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eLASERTENNISPOWERCOORD, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eLASERTENNISPOWERCOORD, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_bCollision = false;
	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eLASERTENNISPOWERCOORD, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_bCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eLASERTENNISPOWERCOORD, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_bCollision = false;
	}
}

void CLaserPowerCoord::Movement(_double dTimeDelta)
{
	if (false == m_bMovement)
		return;

	if (false == m_bActive)
	{
		m_pTransformCom->Set_Speed(20.f, 0.f);
		m_pTransformCom->Go_Left(dTimeDelta);
		if ((m_fResetPosZ + m_fDistance) <= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosZ + m_fDistance));
			m_bActive = true;
			m_bMovement = false;
		}
	}
	else
	{
		m_pTransformCom->Set_Speed(3.f, 0.f);
		m_pTransformCom->Go_Right(dTimeDelta);
		if (m_fResetPosZ >= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosZ));
			m_bActive = false;
			m_bMovement = false;
		}
	}

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CLaserPowerCoord::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxSphereGeometry(1.f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

CLaserPowerCoord * CLaserPowerCoord::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserPowerCoord* pInstance = new CLaserPowerCoord(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserPowerCoord");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserPowerCoord::Clone_GameObject(void * pArg)
{
	CLaserPowerCoord* pInstance = new CLaserPowerCoord(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserPowerCoord");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserPowerCoord::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CDynamic_Env::Free();
}
