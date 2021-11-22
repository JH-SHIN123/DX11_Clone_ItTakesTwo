#include "..\public\Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

HRESULT CTimer_Manager::Add_Timer(const _tchar * pTimerTag)
{
	NOT_NULL_CHECK_RETURN(Find_Timer(pTimerTag), E_FAIL);

	CTimer*	pTimer = CTimer::Create();
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	m_Timers.emplace(pTimerTag, pTimer);

	return S_OK;
}

_double CTimer_Manager::Compute_TimeDelta(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.0);

	return pTimer->Compute_TimeDelta();
}

CTimer * CTimer_Manager::Find_Timer(const _tchar * pTimerTag)
{
	auto iter = find_if(m_Timers.begin(), m_Timers.end(), CTagFinder(pTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);
	m_Timers.clear();
}
