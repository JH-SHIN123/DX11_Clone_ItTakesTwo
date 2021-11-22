#pragma once

#include "GameObject.h"

BEGIN(Client)

#define DATABASE CDataStorage::GetInstance()

class CDataStorage final : public CBase
{
	DECLARE_SINGLETON(CDataStorage)
private:
	explicit CDataStorage() = default;
	virtual ~CDataStorage() = default;

public:
	void Set_CodyPtr(CGameObject* pCody) { m_pCody = pCody; }
	CGameObject* GetCody() { return m_pCody; }

	void Set_MayPtr(CGameObject* pMay) { m_pMay = pMay; }
	CGameObject* GetMay() { return m_pMay; }

	void Set_MainCamPtr(CGameObject* pMainCam) { m_pMainCam = pMainCam; }
	CGameObject* Get_MainCam() { return m_pMainCam; }

	void Set_SubCamPtr(CGameObject* pSubCam) { m_pSubCam = pSubCam; }
	CGameObject* Get_SubCam() { return m_pSubCam; }

	void Set_UFOPtr(CGameObject* pUFO) { m_pUFO = pUFO; }
	CGameObject* Get_UFO() { return m_pUFO; }

	void Set_RobotPtr(CGameObject* pRobot) { m_pRobot = pRobot; }
	CGameObject* Get_Robot() { return m_pRobot; }

	void Set_RobotLeverPtr(CGameObject* pRobotLever) { m_pRobotLever = pRobotLever; }
	CGameObject* Get_RobotLever() { return m_pRobotLever; }

	void Set_RobotBatteryPtr(CGameObject* pRobotBattery) { m_pRobotBattery = pRobotBattery; }
	CGameObject* Get_RobotBattery() { return m_pRobotBattery; }

	void Set_RobotHeadPtr(CGameObject* pRobotHead) { m_pRobotHead = pRobotHead; }
	CGameObject* Get_RobotHead() { return m_pRobotHead; }

	void Set_NoBatterySignPtr(CGameObject* pNoBatterySign) { m_pNoBatterySign = pNoBatterySign; }
	CGameObject* Get_NoBatterySign() { return m_pNoBatterySign; }

	void Set_ValveCount(_uint _iValveRotateCount) { m_iValveRotateCount += _iValveRotateCount; }
	_uint Get_ValveCount() { return m_iValveRotateCount; }

private:
	CGameObject* m_pCody = nullptr;
	CGameObject* m_pMay = nullptr;
	CGameObject* m_pMainCam = nullptr;
	CGameObject* m_pSubCam = nullptr;

	// UFO
	CGameObject* m_pUFO = nullptr;

	// Interactive Objects
	CGameObject* m_pRobot = nullptr;
	CGameObject* m_pRobotLever = nullptr;
	CGameObject* m_pRobotBattery = nullptr;
	CGameObject* m_pRobotHead = nullptr;
	CGameObject* m_pNoBatterySign = nullptr;


private:
	/* For.Valve */
	_uint m_iValveRotateCount = 0;

public:
	virtual void Free() override;
};

END