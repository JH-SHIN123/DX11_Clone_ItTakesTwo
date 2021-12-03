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

	void Set_MenuScreen(CGameObject* pMenuScreen) { m_pMenuScreen = pMenuScreen; }
	CGameObject* Get_MenuScreen() { return m_pMenuScreen; }

	/* Robots */
	void Set_STGravityRobotPtr(CGameObject* pRobot) { m_pSTGravityRobot = pRobot; }
	CGameObject* Get_STGravityRobot() { return m_pSTGravityRobot; }

	void Set_STPinBallRobotPtr(CGameObject* pRobot) { m_pSTPinBallRobot = pRobot; }
	CGameObject* Get_STPinBallRobot() { return m_pSTPinBallRobot; }

	void Set_STPlanetRobotPtr(CGameObject* pRobot) { m_pSTPlanetRobot = pRobot; }
	CGameObject* Get_STPlanetRobot() { return m_pSTPlanetRobot; }


	void Set_PressureBigPlate(CGameObject* pPressureBigPlate) { m_pPressureBigPlate = pPressureBigPlate; }
	CGameObject* Get_PressureBigPlate() { return m_pPressureBigPlate; }

	void Set_PipeCurvePtr(vector<class CPressurePlate*> pPipeCurve) { m_vecPipeCurve = pPipeCurve; }
	class vector<class CPressurePlate*> Get_PressurePlate() { return m_vecPipeCurve;  }

	void Set_Umbrella_JoystickPtr(CGameObject* pUmbrella_Joystick) { m_pUmbrella_Joystick = pUmbrella_Joystick; }
	CGameObject* Get_Umbrella_JoystickPtr() { return m_pUmbrella_Joystick; }

	/* For.Valve */
	void Set_ValveCount(_uint _iValveRotateCount) { m_iValveRotateCount += _iValveRotateCount; }
	void Set_ValveTarget(_uint _iValveTarget) { m_iValveTarget = _iValveTarget; }
	_uint Get_ValveCount() { return m_iValveRotateCount; }
	/*For.Valve_Star*/void	Add_ValveCount_Cody(_bool IsCody) { true == IsCody ? ++m_iCody_Valve_Count : ++m_iMay_Valve_Count; }
	/*For.Valve_Star*/_uint	Get_ValveCount_Cody(_bool IsCody);
	/*For.Valve_Star*/void	Reset_ValveCount_Cody(_bool IsCody) { true == IsCody ? m_iCody_Valve_Count = 0 : m_iMay_Valve_Count = 0; }
	
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

	void Set_SlideDoor(CGameObject* pSlideDoor) { m_pSlideDoor = pSlideDoor; }
	CGameObject* Get_SlideDoor() { return m_pSlideDoor; }

	// Stage Flow
	void Set_Valve_Activate(_bool bValveActivate) { m_IsValveActivated = bValveActivate; }
	_bool Get_IsValve_Activated() { return m_IsValveActivated; }

	void Set_May_Stage(_uint iNumStage) { m_iMayStageNum = iNumStage; }
	_uint Get_May_Stage() {	return m_iMayStageNum; }
	void Set_Cody_Stage(_uint iNumStage) { m_iCodyStageNum = iNumStage; }
	_uint Get_Cody_Stage() { return m_iCodyStageNum; }

	void Set_BigButtonPressed(_bool IsBigButtonPressed) { m_IsBigButtonPressed = IsBigButtonPressed; }
	_bool Get_BigButtonPressed() { return m_IsBigButtonPressed; }
	
	void Set_GravityStageClear(_bool bGravityStageClear) { m_bGravityStageClear = bGravityStageClear; }
	_bool Get_GravityStageClear() { return m_bGravityStageClear; }
	
	void Set_PinBallStageClear(_bool bPinBallStageClear) { m_bPinBallStageClear = bPinBallStageClear; }
	_bool Get_PinBallStageClear() { return m_bPinBallStageClear; }
	
	void Set_RailStageClear(_bool bRailStageClear) { m_bRailStageClear = bRailStageClear; }
	_bool Get_RailStageClear() { return m_bRailStageClear; }

	void Set_HangingPlanet(CGameObject* pHangingPlanet) { m_pHangingPlanet = pHangingPlanet; }
	CGameObject* Get_HangingPlanet() { return m_pHangingPlanet; }


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
	CGameObject* m_pPressureBigPlate = nullptr;
	CGameObject* m_pUmbrella_Joystick = nullptr;
	CGameObject* m_pMenuScreen = nullptr;

private:
	// Interactive Objects
	vector<class CPressurePlate*>	m_vecPipeCurve;

	CGameObject* m_pSTGravityRobot = nullptr;
	CGameObject* m_pSTPinBallRobot = nullptr;
	CGameObject* m_pSTPlanetRobot = nullptr;

	/* For.PinBall */
	CGameObject* m_pPinball = nullptr;
	CGameObject* m_pPinball_Handle = nullptr;
	CGameObject* m_pPinball_HandleBase = nullptr;
	CGameObject* m_pPinball_Spring = nullptr;
	CGameObject* m_pPinBall_BallDoor = nullptr;
	CGameObject* m_pPinBall_Blocked = nullptr;
	CGameObject* m_pPinBall_Door = nullptr;
	CGameObject* m_pSlideDoor = nullptr;

	/* For.HangingPlanet */
	CGameObject* m_pHangingPlanet = nullptr;

private:
	/* For.Valve */
	_uint m_iValveRotateCount = 0;
	_uint m_iValveTarget = 0;
	_uint m_iCody_Valve_Count = 0;
	_uint m_iMay_Valve_Count = 0;

	_bool m_IsBigButtonPressed = false;
	_bool m_bGravityStageClear = false;
	_bool m_bPinBallStageClear = false;
	_bool m_bRailStageClear = false;
	_bool m_IsValveActivated = false;

	_uint m_iMayStageNum = 0;
	_uint m_iCodyStageNum = 0;


public:
	virtual void Free() override;
};

END