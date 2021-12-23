#pragma once
#include "Client_Defines.h"

BEGIN(Client)
#define LASERTENNIS CLaserTennis_Manager::GetInstance()

class CLaserActivation;
class CLaserButton;
class CLaserButtonLarge;
class CLaserButtonLarge_Gate;
class CLaserPowerCoord;
class CTimer_LaserTennis;
class CWall_LaserTennis;

class CLaserTennis_Manager final : public CBase
{
	DECLARE_SINGLETON(CLaserTennis_Manager)

private:
	explicit CLaserTennis_Manager();
	virtual ~CLaserTennis_Manager() = default;

public:
	enum TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:/* Getter */
	_bool	Get_StartGame() { return m_bStartGame; }
	_bool	Get_PushCoord() { return m_bPushCoord; }
	TARGET	Get_Winner()	{ return m_eWinner; }

public:/* Setter */
	void Increase_PowerCoord();
	void Decrease_PowerCoord();
	void KeyCheck(TARGET eTarget);
	void Set_MayCount();
	void Set_CodyCount();

public:
	/* InGame */
	void Push_Coord();
	void Start_Game();
	void Reset_Game();
	void Active_LaserActivation(_uint iOption);
	void Active_LaserButton();
	void Active_LaserButtonLarge_Gate(_bool bActive);
	void Active_LaserButtonLarge_Gate(_bool bActive, TARGET eTarget);
	void Active_LaserButtonLarge(_bool bActive);
	HRESULT Create_LaserTrigger(_fvector vPosition, TARGET eTarget);
	HRESULT Create_LaserTrigger_LargeButton(_fvector vPosition, TARGET eTarget);
	HRESULT Create_Laser(_uint iIndex, TARGET eTarget);

public:
	HRESULT Add_LaserActivation(CLaserActivation* pLaserActivation);
	HRESULT Add_LaserButton(CLaserButton* pLaserActivation);
	HRESULT Add_LaserPowerCoord(_uint iIndex, CLaserPowerCoord* pLaserPowerCoord);
	HRESULT Add_LaserButtonLarge(_uint iIndex, CLaserButtonLarge* pLaserButtonLarge);
	HRESULT Add_LaserButtonLarge_Gate(CLaserButtonLarge_Gate* pLaserButtonLarge_Gate);
	HRESULT Add_Wall(CWall_LaserTennis* pWall);
	void Set_Timer(CTimer_LaserTennis* pTimer_LaserTennis) { m_pTimer_LaserTennis = pTimer_LaserTennis; }

public:
	HRESULT NativeConstruct_LaserTennis();

private:
	typedef vector<CLaserActivation*>			LASERACTIVATION;
	typedef vector<CLaserButton*>				LASERBUTTON;
	typedef vector<CLaserButtonLarge_Gate*>		LASERBUTTONLARGE_GATE;
	typedef vector<CWall_LaserTennis*>			WALL;

	CGameInstance*			m_pGameInstance = nullptr;

	LASERACTIVATION			m_LaserActivation;
	LASERACTIVATION			m_LaserActivation_Cody;
	LASERACTIVATION			m_LaserActivation_May;
	LASERBUTTON				m_LaserButton_Cody;
	LASERBUTTON				m_LaserButton_May;
	LASERBUTTONLARGE_GATE	m_LaserButtonLarge_Gate;
	WALL					m_Wall;

	CLaserButtonLarge*		m_pLaserButtonLarge[2];
	CLaserPowerCoord*		m_pLaserPowerCoord[2];
	CTimer_LaserTennis*		m_pTimer_LaserTennis = nullptr;

	_bool					m_bKeyCheck[TARGET_END];
	_bool					m_bStartGame = false;
	_bool					m_bReady = false;
	_bool					m_bPushCoord = false;
	_uint					m_iPowerCoordCount = 0;

	_uint					m_iCodyCount = 0;
	_uint					m_iMayCount = 0;
	TARGET					m_eWinner = TARGET_END;

	_uint*					m_pRandomArr = nullptr;

private:
	/* UI »ý¼º */
	HRESULT Create_StartUI();
	HRESULT Create_ResultUI();

	void Active_CollisionWall();
	void Get_RandomArr(_uint iIndex);

public:
	virtual void Free();
};
END
