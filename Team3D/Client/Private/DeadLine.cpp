#include "stdafx.h"
#include "..\Public\DeadLine.h"

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

	ARG_DESC tDeadLineArg;
	if (nullptr != pArg)
		memcpy(&tDeadLineArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tDeadLineArg.vPosition));
	m_pTransformCom->Set_Rotaion(XMLoadFloat3(&tDeadLineArg.vRotation));

	CTriggerActor::ARG_DESC tArg;

	PxGeometry* pGeom = new PxBoxGeometry(tDeadLineArg.vScale.x, tDeadLineArg.vScale.y, tDeadLineArg.vScale.z);
	m_UserData.eID = GameID::eDEADLINE;
	m_UserData.pGameObject = this;

	tArg.pTransform = m_pTransformCom;
	tArg.pGeometry = pGeom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &tArg), E_FAIL);

	Safe_Delete(pGeom);
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
