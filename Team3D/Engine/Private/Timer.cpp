#include "..\public\Timer.h"

HRESULT CTimer::NativeConstruct()
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

_double CTimer::Compute_TimeDelta()
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		m_FixTime = m_FrameTime;
		QueryPerformanceFrequency(&m_CpuTick);
	}

	m_dTimeDelta	= _double(m_FrameTime.QuadPart - m_LastTime.QuadPart) / m_CpuTick.QuadPart;
	m_LastTime		= m_FrameTime;

	return m_dTimeDelta;
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer;

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Create Instance - CTimer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}