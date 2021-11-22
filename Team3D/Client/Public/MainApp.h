#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Loading.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
public:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	NativeConstruct();
	HRESULT Run_App();

private:
	CGameInstance*			m_pGameInstance		= nullptr;
	CRenderer*				m_pRenderer			= nullptr;
	ID3D11Device*			m_pDevice			= nullptr;
	ID3D11DeviceContext*	m_pDeviceContext	= nullptr;
	/* For.Timer */
	_double					m_dFrameAcc			= 0.0; /* 1프레임시간 누적 */
private:
	_int	Tick(_double dTimeDelta);
	HRESULT	Render(_double dTimeDelta);
	HRESULT Ready_Timer();
	HRESULT	Ready_Prototype_ForStatic();
	HRESULT	Ready_DefaultLevel(Level::ID eLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;

#ifdef _DEBUG
	/* For.FPS */
private:
	_double	m_dSecondAcc		= 0.0; /* 1초 누적  */
	_tchar	m_szFPS[MAX_PATH]	= TEXT("");
	_uint	m_iRenderCount		= 0;
private:
	void	Show_FPS(_double dTimeDelta);
#endif
};

END