#include "stdafx.h"
#include "SpaceRail.h"
#include "Character.h"

CSpaceRail::CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

_fmatrix CSpaceRail::Get_WorldMatrix() const
{
	if (nullptr == m_pTransformCom) return XMMatrixIdentity();

	return m_pTransformCom->Get_WorldMatrix();
}

_fvector CSpaceRail::Get_Position() const
{
	if (nullptr == m_pTransformCom) return XMVectorZero();

	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CSpaceRail::NativeConstruct(void* pArg)
{
	//if (nullptr != pArg)
	//	m_pParent = (CToyBoxButton*)pArg;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);

	CGameObject::NativeConstruct(nullptr);

	m_UserData.eID = GameID::eSPACERAIL;
	m_UserData.pGameObject = this;

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pGeometry = new PxSphereGeometry(4.f);
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

void CSpaceRail::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	if (eID == GameID::eCODY || eID == GameID::eMAY)
	{
		CCharacter* pChar = (CCharacter*)pGameObject;
		switch (eStatus)
		{
		case Engine::TriggerStatus::eFOUND:
			pChar->Set_SpaceRail(this);
			break;
		}
	}
}

CSpaceRail* CSpaceRail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CSpaceRail* pInstance = new CSpaceRail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CSpaceRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceRail::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
