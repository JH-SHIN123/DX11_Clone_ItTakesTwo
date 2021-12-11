#pragma once
#include "Client_Defines.h"

#include "LaserActivation.h"
#include "LaserButton.h"
#include "LaserButtonLarge.h"
#include "LaserButtonLarge_Gate.h"
#include "LaserPowerCoord.h"
#include "Timer_LaserTennis.h"

BEGIN(Client)
#define LASERTENNIS CLaserTennis_Manager::GetInstance()
class CLaserTennis_Manager final : public CBase
{
	DECLARE_SINGLETON(CLaserTennis_Manager)

private:
	explicit CLaserTennis_Manager();
	virtual ~CLaserTennis_Manager() = default;

public:
	enum TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:/* Setter */
	void Create_LaserTrigger(_fvector vPosition, TARGET eTarget);
	void Create_LaserTrigger_LargeButton(_fvector vPosition, TARGET eTarget);
	void Check_PowerCoord();

public:
	void Start_Game();

public:
	HRESULT Add_LaserActivation(CLaserActivation* pLaserActivation);
	HRESULT Add_LaserButton(CLaserButton* pLaserActivation);
	HRESULT Add_LaserPowerCoord(_uint iIndex, CLaserPowerCoord* pLaserPowerCoord);
	HRESULT Add_LaserButtonLarge(_uint iIndex, CLaserButtonLarge* pLaserButtonLarge);
	HRESULT Add_LaserButtonLarge_Gate(CLaserButtonLarge_Gate* pLaserButtonLarge_Gate);

	void Set_Timer(CTimer_LaserTennis* pTimer_LaserTennis) { m_pTimer_LaserTennis = pTimer_LaserTennis; }

public:
	HRESULT NativeConstruct_LaserTennis();

private:
	typedef vector<CLaserActivation*>			LASERACTIVATION;
	typedef vector<CLaserButton*>				LASERBUTTON;
	typedef vector<CLaserButtonLarge_Gate*>		LASERBUTTONLARGE_GATE;

	LASERACTIVATION			m_LaserActivation;
	LASERBUTTON				m_LaserButton;
	LASERBUTTONLARGE_GATE	m_LaserButtonLarge_Gate;

	CLaserButtonLarge*		m_pLaserButtonLarge[2];
	CLaserPowerCoord*		m_pLaserPowerCoord[2];

	CTimer_LaserTennis*		m_pTimer_LaserTennis = nullptr;

	_uint					m_iPowerCoordCount = 0;

public:
	virtual void Free();
};
END
