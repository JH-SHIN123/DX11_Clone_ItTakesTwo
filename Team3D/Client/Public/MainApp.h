#pragma once

#include "Client_Defines.h"
#include "Loading.h"

BEGIN(Client)

/**
* 건드리지 마세요.
*/
class CMainApp final : public CBase
{
public:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public: /* Getter */
	/* For.GameDebugger */
	_bool&					Get_DebuggerExit() { return m_bDebuggerExit; }
	_bool&					Get_DebuggerBreak() { return m_bDebuggerBreak; }
	CGameObject*&			Get_DebuggerTarget() { return m_pDebuggerTarget; }
	CGameInstance*&			Get_DebuggerGameInstance() { return m_pGameInstance; }
	CGameObject_Manager*&	Get_DebuggerGameObjectManager() { return m_pGameObject_Manager; }

public: /* Setter */
	/* For.GameDebugger */
	//void Initialize_Debugger() { m_bDebuggerBreak = true; m_pDebuggerTarget = nullptr; }
	//void Exit_Debugger() { m_bDebuggerExit = true; }
	void Set_DebuggerTarget(CGameObject* pTarget) { m_pDebuggerTarget = pTarget; }

public:
	HRESULT	NativeConstruct();
	HRESULT Run_App();
	void	Lock_Mouse();

private:
	CGameInstance*			m_pGameInstance		= nullptr;
	CRenderer*				m_pRenderer			= nullptr;
	ID3D11Device*			m_pDevice			= nullptr;
	ID3D11DeviceContext*	m_pDeviceContext	= nullptr;
	/* For.Timer */
	_double					m_dFrameAcc			= 0.0; /* 1프레임시간 누적 */
	_double					m_dTimeDelta		= 0.0;
	class CPxEventCallback* m_pPxEventCallback  = nullptr;
	/* For.MouseLock */
	_bool					m_bMouseLock		= false;
	/* For.GameDebugger */
	class CGameDebugger*	m_pDebugger = nullptr;
	_bool					m_bDebuggerExit = false;
	_bool					m_bDebuggerBreak = false;
	CGameObject*			m_pDebuggerTarget = nullptr;
	CGameObject_Manager*	m_pGameObject_Manager = nullptr;
private:
	_int	Tick(_double dTimeDelta);
	HRESULT	Render(_double dTimeDelta);
	HRESULT Ready_Timer();
	HRESULT	Ready_Prototype_ForStatic();
	HRESULT	Ready_DefaultLevel(Level::ID eLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;

//#ifdef _DEBUG
	/* For.FPS */
private:
	_double	m_dSecondAcc		= 0.0; /* 1초 누적  */
	_tchar	m_szFPS[MAX_PATH]	= TEXT("");
	_uint	m_iRenderCount		= 0;
private:
	void	Show_FPS(_double dTimeDelta);
//#endif
};

END