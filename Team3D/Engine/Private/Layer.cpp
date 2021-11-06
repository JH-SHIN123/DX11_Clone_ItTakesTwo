#include "..\public\Layer.h"
#include "GameObject.h"

void CLayer::Bring_GameObjectList(list<CGameObject*>& GameObjectList)
{
	GameObjectList = m_Objects;
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_Objects.emplace_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_double TimeDelta)
{
	_int	iProgressNum = 0;

	for (auto& pGameObject : m_Objects)
	{
		iProgressNum = pGameObject->Tick(TimeDelta);

		if (EVENT_ERROR == iProgressNum)
			return iProgressNum;
		else if (EVENT_DEAD == iProgressNum)
			m_DeadObjects.emplace_back(pGameObject);
	}

	return iProgressNum;
}

_int CLayer::Late_Tick(_double TimeDelta)
{
	_int	iProgressNum = 0;

	for (auto& pGameObject : m_Objects)
	{
		iProgressNum = pGameObject->Late_Tick(TimeDelta);

		if (EVENT_ERROR == iProgressNum)
			return iProgressNum;
	}

	return iProgressNum;
}

void CLayer::Remove_DeadObject()
{
	if (m_DeadObjects.empty())
		return;

	for (auto& pGameObject : m_DeadObjects)
	{
		auto iter = find(m_Objects.begin(), m_Objects.end(), pGameObject);
		Safe_Release(*iter);
		m_Objects.erase(iter);
	}
	m_DeadObjects.clear();
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer;

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Create Instance - CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	m_DeadObjects.clear();

	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);
	m_Objects.clear();
}