#include "stdafx.h"
#include "..\Public\DeadLine.h"
#include "Cody.h"
#include "May.h"
#include "Effect_Generator.h"
#include "DataStorage.h"

CDeadLine::CDeadLine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDeadLine::CDeadLine(const CDeadLine & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDeadLine::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDeadLine::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eDEADLINE;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CDeadLine::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CDeadLine::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

void CDeadLine::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);

	// Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY && false == m_bTrigger)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eDEADLINE, true, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eDEADLINE, false, ((CCody*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = false;
	}

	// May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY && false == m_bTrigger)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eDEADLINE, true, ((CMay*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eDEADLINE, false, ((CMay*)pGameObject)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_bTrigger = false;
	}
}

HRESULT CDeadLine::Ready_Component(void * pArg)
{
	ARG_DESC tDeadLineArg;
	if (nullptr != pArg)
		memcpy(&tDeadLineArg, pArg, sizeof(ARG_DESC));

	/* Transform */
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tDeadLineArg.vPosition));
	m_pTransformCom->Set_Rotaion(XMLoadFloat3(&tDeadLineArg.vRotation));

	/* Trigger */
	CTriggerActor::ARG_DESC tArg;
	PxGeometry* pGeom = new PxBoxGeometry(tDeadLineArg.vScale.x, tDeadLineArg.vScale.y, tDeadLineArg.vScale.z);
	tArg.pTransform = m_pTransformCom;
	tArg.pGeometry = pGeom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &tArg), E_FAIL);
	Safe_Delete(pGeom);

	return S_OK;
}

CDeadLine * CDeadLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDeadLine* pInstance = new CDeadLine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDeadLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDeadLine::Clone_GameObject(void * pArg)
{
	CDeadLine* pInstance = new CDeadLine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDeadLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDeadLine::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
