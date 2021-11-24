#include "..\public\Component_Manager.h"
#include <mutex>

std::mutex g_mutex;

IMPLEMENT_SINGLETON(CComponent_Manager)

HRESULT CComponent_Manager::Reserve_Container(_uint iLevelCount)
{
	NOT_NULL_CHECK_RETURN(m_pPrototypes, E_FAIL);

	m_iLevelCount = iLevelCount;
	m_pPrototypes = new PROTOTYPES[iLevelCount];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Component_Prototype(_uint iLevelIndex, const _tchar * const pPrototypeTag, CComponent * pComponent)
{
	g_mutex.lock();

	NULL_CHECK_RETURN(pComponent, E_FAIL);
	NULL_CHECK_RETURN(iLevelIndex < m_iLevelCount, E_FAIL);

	_tchar* pTag = new _tchar[lstrlenW(pPrototypeTag) + 1];
	lstrcpyW(pTag, pPrototypeTag);

	CComponent*	pPrototype = Find_Prototype(iLevelIndex, pTag);
	NOT_NULL_CHECK_RETURN(pPrototype, E_FAIL);

	m_pPrototypes[iLevelIndex].emplace(pTag, pComponent);

	g_mutex.unlock();

	return S_OK;
}

CComponent * CComponent_Manager::Add_Component_Clone(_uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(iPrototypeLevelIndex < m_iLevelCount, nullptr);

	CComponent* pPrototype = Find_Prototype(iPrototypeLevelIndex, pPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, nullptr);

	return pPrototype->Clone_Component(pArg);
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	NULL_CHECK(iLevelIndex < m_iLevelCount);

	for (auto& rPair : m_pPrototypes[iLevelIndex])
	{
		delete[] rPair.first;
		Safe_Release(rPair.second);
	}
	m_pPrototypes[iLevelIndex].clear();
}

void CComponent_Manager::Clear_All()
{
	for (_uint iIndex = 0; iIndex < m_iLevelCount; ++iIndex)
	{
		for (auto& rPair : m_pPrototypes[iIndex])
		{
			delete[] rPair.first;
			Safe_Release(rPair.second);
		}
		m_pPrototypes[iIndex].clear();
	}
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	NULL_CHECK_RETURN(iLevelIndex < m_iLevelCount, nullptr);

	auto iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	Clear_All();

	Safe_Delete_Array(m_pPrototypes);
}
