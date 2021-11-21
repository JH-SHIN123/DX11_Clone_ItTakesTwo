#pragma once

#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
private:
	explicit CTimer_Manager() = default;
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Ready_TimeManager();
	_double Compute_ImmediateTimeDelta();

public:
	HRESULT	Add_Timer(const _tchar* pTimerTag);
	_double	Compute_TimeDelta(const _tchar* pTimerTag);

private: /* Typedef */
	typedef unordered_map<const _tchar*, CTimer*> TIMERS;
private:
	TIMERS	m_Timers;
	CTimer* m_pImmediateTimer = nullptr;

private:
	CTimer*	Find_Timer(const _tchar* pTimerTag);

public:
	virtual void Free() override;
};

END