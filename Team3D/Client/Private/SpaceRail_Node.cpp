#include "stdafx.h"
#include "SpaceRail_Node.h"
#include "Character.h"

CSpaceRail_Node::CSpaceRail_Node(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

_fmatrix CSpaceRail_Node::Get_WorldMatrix() const
{
	if (nullptr == m_pTransformCom) return XMMatrixIdentity();

	return m_pTransformCom->Get_WorldMatrix();
}

_fvector CSpaceRail_Node::Get_Position() const
{
	if (nullptr == m_pTransformCom) return XMVectorZero();

	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CSpaceRail_Node::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(m_tDesc));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	CGameObject::NativeConstruct(nullptr);

	m_UserData.eID = GameID::eSPACERAIL;
	m_UserData.pGameObject = this;

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pGeometry = new PxSphereGeometry(3.f);
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

void CSpaceRail_Node::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	if (eID == GameID::eCODY || eID == GameID::eMAY)
	{
		CCharacter* pChar = (CCharacter*)pGameObject;
		switch (eStatus)
		{
		case Engine::TriggerStatus::eFOUND:
			pChar->Set_SpaceRailNode(this);
			break;
		}
	}
}

CSpaceRail_Node* CSpaceRail_Node::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CSpaceRail_Node* pInstance = new CSpaceRail_Node(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CSpaceRail_Node");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceRail_Node::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
