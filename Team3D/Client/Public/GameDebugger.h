#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CGameDebugger final : public CBase
{
private:
	explicit CGameDebugger() = default;
	virtual ~CGameDebugger() = default;

public: /* Getter */
	CRITICAL_SECTION* Get_CriticalSectionPtr() { return &m_CriticalSection; }

public:
	HRESULT NativeConstruct(class CMainApp* pMainApp);
	void	Debugging();

private:
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;
	class CMainApp*			m_pMainApp = nullptr;

public:
	static CGameDebugger* Create(class CMainApp* pMainApp);
	virtual void Free() override;
};

END