#include "stdafx.h"
#include "SpaceRail.h"
#include "Character.h"
#include "SpaceRail_Node.h"
#include "SpaceRailData.h"

CSpaceRail::CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CSpaceRail::CSpaceRail(const CSpaceRail& rhs)
	: CComponent(rhs),
	m_pRailData(rhs.m_pRailData)
{
}

void CSpaceRail::RideOnRail(const _tchar* pRailTag, _uint iIndex, STATE eState)
{
	if (m_bRiding) return;
	if (nullptr == pRailTag) return;
	if (nullptr == m_pRailData) return;

	m_pSpaceRailNodes = m_pRailData->Get_RailNodes(pRailTag);

	m_iIndex_CurNode = iIndex;
	m_eState = eState;
	m_bRiding = true;
}

void CSpaceRail::Riding(CCharacter* pSubject)
{
	if (nullptr == pSubject) return;
	if (false == m_bRiding) return;
	if (nullptr == m_pSpaceRailNodes) return;

	switch (m_eState)
	{
	case STATE_FORWARD:
		if (m_pSpaceRailNodes->size()-1 > m_iIndex_CurNode) {
			// 다음 노드에 도달했을때
			++m_iIndex_CurNode;
		}
		else
		{
			m_pSpaceRailNodes = nullptr;
			m_eState = STATE_END;
			m_bRiding = false;
			return;
		}
		break;
	case STATE_BACKWARD:
		if (0 < m_iIndex_CurNode) {
			// 다음 노드에 도달했을때
			--m_iIndex_CurNode;
		}
		else
		{
			m_pSpaceRailNodes = nullptr;
			m_eState = STATE_END;
			m_bRiding = false;
			return;
		}
		break;
	}

	pSubject->Set_WorldMatrix((*m_pSpaceRailNodes)[m_iIndex_CurNode]->Get_WorldMatrix());
}

HRESULT CSpaceRail::NativeConstruct_Prototype()
{
	m_pRailData = CSpaceRailData::GetInstance();

	return S_OK;
}

HRESULT CSpaceRail::NativeConstruct(void* pArg)
{
	return S_OK;
}

CSpaceRail* CSpaceRail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSpaceRail* pInstance = new CSpaceRail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSpaceRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSpaceRail::Clone_Component(void* pArg)
{
	CSpaceRail* pInstance = new CSpaceRail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSpaceRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpaceRail::Free()
{
	m_pRailData = nullptr;
	m_pSpaceRailNodes = nullptr;
	CComponent::Free();
}
