#include "..\public\GameObject_Manager.h"
#include <mutex>

std::mutex g_mutexGameObject;

IMPLEMENT_SINGLETON(CGameObject_Manager)

void CGameObject_Manager::Bring_LayerGameObjects(_uint iLayerLevelIndex, const _tchar * pLayerTag, list<CGameObject*>& GameObjectList)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return;

	pLayer->Bring_GameObjectList(GameObjectList);
}

HRESULT CGameObject_Manager::Reserve_Container(_uint iLevelCount)
{
	NOT_NULL_CHECK_RETURN(m_pLayers, E_FAIL);
	NOT_NULL_CHECK_RETURN(m_pPrototypes, E_FAIL);

	m_iLevelCount = iLevelCount;
	m_pLayers = new LAYERS[iLevelCount];
	m_pPrototypes = new PROTOTYPES[iLevelCount];

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_Prototype(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	g_mutexGameObject.lock();

	NULL_CHECK_RETURN(pPrototype, E_FAIL);
	NULL_CHECK_RETURN(m_pPrototypes, E_FAIL);
	NULL_CHECK_RETURN(iPrototypeLevelIndex < m_iLevelCount, E_FAIL);

	_tchar* pTag = new _tchar[lstrlenW(pPrototypeTag) + 1];
	lstrcpyW(pTag, pPrototypeTag);

	NOT_NULL_CHECK_RETURN(Find_Prototype(iPrototypeLevelIndex, pTag), E_FAIL);

	m_pPrototypes[iPrototypeLevelIndex].emplace(pTag, pPrototype);

	g_mutexGameObject.unlock();

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_Clone(_uint iLayerLevelIndex, const _tchar* pLayerTag, _uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, void* pArg, CGameObject ** ppOut)
{
	NULL_CHECK_RETURN(m_pLayers, E_FAIL);
	NULL_CHECK_RETURN(iLayerLevelIndex < m_iLevelCount, E_FAIL);
	NULL_CHECK_RETURN(iPrototypeLevelIndex < m_iLevelCount, E_FAIL);

	CGameObject* pPrototype = Find_Prototype(iPrototypeLevelIndex, pPrototypeTag);	
	NULL_CHECK_RETURN(pPrototype, E_FAIL);

	CGameObject* pClone = pPrototype->Clone_GameObject(pArg);
	NULL_CHECK_RETURN(pClone, E_FAIL);

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		m_pLayers[iLayerLevelIndex].emplace(pLayerTag, pLayer);
	}

	pLayer->Add_GameObject(pClone);

	if (nullptr != ppOut)
	{
		*ppOut = pClone;
		Safe_AddRef(pClone);
	}

	return S_OK;
}

_int CGameObject_Manager::Tick(_double dTimeDelta)
{
	_int iProgressNum = 0;

	for (_uint iLevelIndex = 0; iLevelIndex < m_iLevelCount; ++iLevelIndex)
	{
		for (auto& rPair : m_pLayers[iLevelIndex])
		{
			iProgressNum = rPair.second->Tick(dTimeDelta);

			if (EVENT_ERROR == iProgressNum)
				return iProgressNum;
		}
	}

	for (_uint iLevelIndex = 0; iLevelIndex < m_iLevelCount; ++iLevelIndex)
	{
		for (auto& rPair : m_pLayers[iLevelIndex])
			rPair.second->Remove_DeadObject();
	}

	return iProgressNum;
}

_int CGameObject_Manager::Late_Tick(_double dTimeDelta)
{
	_int iProgressNum = 0;

	for (_uint iLevelIndex = 0; iLevelIndex < m_iLevelCount; ++iLevelIndex)
	{
		for (auto& rPair : m_pLayers[iLevelIndex])
		{
			iProgressNum = rPair.second->Late_Tick(dTimeDelta);

			if (EVENT_ERROR == iProgressNum)
				return iProgressNum;
		}
	}

	return iProgressNum;
}

void CGameObject_Manager::Clear(_uint iLevelIndex)
{
	NULL_CHECK(iLevelIndex < m_iLevelCount);

	for (auto& rPair : m_pLayers[iLevelIndex])
		Safe_Release(rPair.second);
	m_pLayers[iLevelIndex].clear();

	for (auto& rPair : m_pPrototypes[iLevelIndex])
	{
		delete[] rPair.first;
		Safe_Release(rPair.second);
	}
	m_pPrototypes[iLevelIndex].clear();
}

void CGameObject_Manager::Clear_All()
{
	for (_uint iIndex = 0; iIndex < m_iLevelCount; ++iIndex)
	{
		for (auto& rPair : m_pLayers[iIndex])
			Safe_Release(rPair.second);

		m_pLayers[iIndex].clear();

		for (auto& rPair : m_pPrototypes[iIndex])
		{
			delete[] rPair.first;
			Safe_Release(rPair.second);
		}
		m_pPrototypes[iIndex].clear();
	}
}

void CGameObject_Manager::Clear_Layer(_uint iLayerLevelIndex, const _tchar * pLayerTag)
{
	NULL_CHECK(iLayerLevelIndex < m_iLevelCount);

	auto iter = find_if(m_pLayers[iLayerLevelIndex].begin(), m_pLayers[iLayerLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLayerLevelIndex].end())
		return;

	Safe_Release(iter->second);

	m_pLayers[iLayerLevelIndex].erase(iter);
}

CGameObject * CGameObject_Manager::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	NULL_CHECK_RETURN(iLevelIndex < m_iLevelCount, nullptr);

	auto iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CLayer * CGameObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	NULL_CHECK_RETURN(iLevelIndex < m_iLevelCount, nullptr);

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CGameObject_Manager::Free()
{
	Safe_Delete_Array(m_pLayers);
	Safe_Delete_Array(m_pPrototypes);
}
