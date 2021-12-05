#include "stdafx.h"
#include "..\Public\SavePoint.h"
#include "Cody.h"
#include "May.h"

CSavePoint::CSavePoint(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSavePoint::CSavePoint(const CSavePoint & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSavePoint::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSavePoint::NativeConstruct(void * pArg)
{
	ARG_DESC tSavePointDesc;
	if (nullptr != pArg)
		memcpy(&tSavePointDesc, pArg, sizeof(ARG_DESC));

	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eSAVEPOINT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_vSavePosition = tSavePointDesc.vSavePosition;

	return S_OK;
}

_int CSavePoint::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CSavePoint::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

void CSavePoint::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);

	// Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eSAVEPOINT, true, XMLoadFloat3(&m_vSavePosition));

	// May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eSAVEPOINT, true, XMLoadFloat3(&m_vSavePosition));
}

HRESULT CSavePoint::Ready_Component(void * pArg)
{
	ARG_DESC tSavePointDesc;
	if (nullptr != pArg)
		memcpy(&tSavePointDesc, pArg, sizeof(ARG_DESC));

	/* Transform */
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tSavePointDesc.vPosition));
	m_pTransformCom->Set_Rotaion(XMLoadFloat3(&tSavePointDesc.vRotation));

	CTriggerActor::ARG_DESC tArg;
	PxGeometry* pGeom = nullptr;

	if (tSavePointDesc.eShape == BOX)
		pGeom = new PxBoxGeometry(tSavePointDesc.vScale.x, tSavePointDesc.vScale.y, tSavePointDesc.vScale.z);
	else if (tSavePointDesc.eShape == SPHERE)
		pGeom = new PxSphereGeometry(tSavePointDesc.vScale.x);

	tArg.pTransform = m_pTransformCom;
	tArg.pGeometry = pGeom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &tArg), E_FAIL);
	Safe_Delete(pGeom);

	return S_OK;
}

CSavePoint * CSavePoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSavePoint* pInstance = new CSavePoint(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSavePoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSavePoint::Clone_GameObject(void * pArg)
{
	CSavePoint* pInstance = new CSavePoint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSavePoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSavePoint::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);
	CGameObject:: Free();
}
