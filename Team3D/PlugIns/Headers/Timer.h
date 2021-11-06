#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	explicit CTimer() = default;
	virtual ~CTimer() = default;

public:
	HRESULT	NativeConstruct();
	_double	Compute_TimeDelta();

private:
	LARGE_INTEGER	m_FrameTime;
	LARGE_INTEGER	m_FixTime;
	LARGE_INTEGER	m_LastTime;
	LARGE_INTEGER	m_CpuTick;
	_double			m_dTimeDelta = 0.0;

public:
	static CTimer* Create();
	virtual void Free() override;
};

END