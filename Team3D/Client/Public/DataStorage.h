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

	void Set_BridgePtr(CGameObject* pBridge) { m_pBridge = pBridge; }
	CGameObject* Get_Bridge() { return m_pBridge; }

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
	void Set_ValveTarget(_uint _iValveTarget) { m_iValveTarget = _iValveTarget; }
	_uint Get_ValveCount() { return m_iValveRotateCount; }
	
	/* For.PinBall */
	void Set_Pinball(CGameObject* pPinball) { m_pPinball = pPinball; }
	CGameObject* Get_Pinball() { return m_pPinball; }

	void Set_Pinball_Handle(CGameObject* pPinball_Handle) { m_pPinball_Handle = pPinball_Handle; }
	CGameObject* Get_Pinball_Handle() { return m_pPinball_Handle; }

	void Set_Pinball_HandleBase(CGameObject* pPinball_HandleBase) { m_pPinball_HandleBase = pPinball_HandleBase; }
	CGameObject* Get_Pinball_HandleBase() { return m_pPinball_HandleBase; }

	void Set_Pinball_Spring(CGameObject* pPinball_Spring) { m_pPinball_Spring = pPinball_Spring; }
	CGameObject* Get_Pinball_Spring() { return m_pPinball_Spring; }

	void Set_Pinball_BallDoor(CGameObject* pPinball_BallDoor) { m_pPinBall_BallDoor = pPinball_BallDoor; }
	CGameObject* Get_Pinball_BallDoor() { return m_pPinBall_BallDoor; }

	void Set_Pinball_Blocked(CGameObject* pPinball_Blocked) { m_pPinBall_Blocked = pPinball_Blocked; }
	CGameObject* Get_Pinball_Blocked() { return m_pPinBall_Blocked; }

	void Set_Pinball_Door(CGameObject* pPinBall_Door) { m_pPinBall_Door = pPinBall_Door; }
	CGameObject* Get_Pinball_Door() { return m_pPinBall_Door; }

private:
	CGameObject* m_pCody = nullptr;
	CGameObject* m_pMay = nullptr;
	CGameObject* m_pMainCam = nullptr;
	CGameObject* m_pSubCam = nullptr;

	// UFO
	CGameObject* m_pUFO = nullptr;

	/* For.Bridge */
	CGameObject* m_pBridge = nullptr;

	// Interactive Objects
	CGameObject* m_pRobot = nullptr;
	CGameObject* m_pRobotLever = nullptr;
	CGameObject* m_pRobotBattery = nullptr;
	CGameObject* m_pRobotHead = nullptr;
	CGameObject* m_pNoBatterySign = nullptr;

	/* For.PinBall */
	CGameObject* m_pPinball = nullptr;
	CGameObject* m_pPinball_Handle = nullptr;
	CGameObject* m_pPinball_HandleBase = nullptr;
	CGameObject* m_pPinball_Spring = nullptr;
	CGameObject* m_pPinBall_BallDoor = nullptr;
	CGameObject* m_pPinBall_Blocked = nullptr;
	CGameObject* m_pPinBall_Door = nullptr;

private:
	/* For.Valve */
	_uint m_iValveRotateCount = 0;
	_uint m_iValveTarget = 0;

public:
	virtual void Free() override;
};

END