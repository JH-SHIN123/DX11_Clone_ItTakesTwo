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

	/* For.Logo */
	void Set_MenuScreen(CGameObject* pMenuScreen) { m_pMenuScreen = pMenuScreen; }
	CGameObject* Get_MenuScreen() { return m_pMenuScreen; }

	void Set_ReturnButton(CGameObject* pReturnButton) { m_pReturnButton = pReturnButton; }
	CGameObject* Get_ReturnButton() const { return m_pReturnButton; }

	void Set_Participation_Cody(CGameObject* pParticipation_Cody) { m_pParticipation_Cody = pParticipation_Cody; }
	CGameObject* Get_Participation_Cody() const { return m_pParticipation_Cody; }

	void Set_Participation_May(CGameObject* pParticipation_May) { m_pParticipation_May = pParticipation_May; }
	CGameObject* Get_Participation_May() const { return m_pParticipation_May; }

	void Set_Script(CGameObject* pScript) { m_pScript = pScript; }
	CGameObject* Get_Script() { return m_pScript; }

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

	/* For.BossRoom */
	void Set_BossFloor(CGameObject* pBossFloor) { m_pBossFloor = pBossFloor; }
	CGameObject* Get_BossFloor() { return m_pBossFloor; }

	void Set_BossDoor01(CGameObject* pBossdoor01) { m_pBossDoor01 = pBossdoor01; }
	CGameObject* Get_BossDoor01() { return m_pBossDoor01; }

	void Set_BossDoor02(CGameObject* pBossdoor02) { m_pBossDoor02 = pBossdoor02; }
	CGameObject* Get_BossDoor02() { return m_pBossDoor02; }

	void Set_BossSlideDoor(CGameObject* pBossSlideDoor) { m_pBossSlideDoor = pBossSlideDoor; }
	CGameObject* Get_BossSlideDoor() { return m_pBossSlideDoor; }

	/* 보스 방 문 조작 함수들 */
	void GoUp_BossFloor(_float fMaxdistance = 50.f, _float fSpeed = 15.f);	/* fMaxDistanc : 올릴 높이, fSpeed : 올라가는 속도 */
	void Close_BossDoor(); 													/* 보스 등장시 바닥 문 닫을 때 호출 */
	void Close_BossSlideDoor(); 											/* 보스방에 있는 중력발판 사용끝나면 호출 */

	/* For.SpaceShip */
	void Set_MoonBaboon_SpaceShip(CGameObject* pMoonBaboonSpaceShip) { m_pMoonBaboonSpaceShip = pMoonBaboonSpaceShip; }
	CGameObject* Get_MoonBaboon_SpaceShip() { return m_pMoonBaboonSpaceShip; }

	/* For.MoonUFO */
	void Set_MoonUFO(CGameObject* pMoonUFO) { m_pMoonUFO = pMoonUFO; }
	CGameObject* Get_MoonUFO() { return m_pMoonUFO; }

	void Set_Moon(CGameObject* pMoon) { m_pMoon = pMoon; }
	CGameObject* Get_Mooon() { return m_pMoon; }

	void Set_RunningMoonBaboon(CGameObject* pRunningMoonBaboon) { m_pRunningMoonBaboon = pRunningMoonBaboon; }
	CGameObject* Get_RunningMoonBaboon() { return m_pRunningMoonBaboon; }

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

	void Set_SpaceRails(const _tchar* pRailTag, CGameObject* pRail);
	CGameObject* Get_SpaceRail(const _tchar* pRailTag);

	void Set_ControlRoom_Battery(CGameObject* pControlRoom_Battery) { m_pControlRoom_Battery = pControlRoom_Battery; }
	CGameObject* Get_ControlRoom_Battery() { return m_pControlRoom_Battery; }

	void Set_HookUFO(CGameObject* pHookUFO) { m_pHookUFO = pHookUFO; }
	CGameObject* Get_HookUFO() { return m_pHookUFO; }

	void Set_UFORadarSet(CGameObject* pUFORadarSet) { m_pUFORadarSet = pUFORadarSet; }
	CGameObject* Get_UFORadarSet() { return m_pUFORadarSet; }

	/* For. Boss */
	void Set_MoonBaboon_MainLaser(CGameObject* pMoonBaboon_MainLaser) { m_pMoonBaboonMainLaser = pMoonBaboon_MainLaser; }
	CGameObject* Get_MoonBaboon_MainLaser() { return m_pMoonBaboonMainLaser; }

	void Set_BossUFO(CGameObject* pBossUFO) { m_pBossUFO = pBossUFO; }
	CGameObject* Get_BossUFO() { return m_pBossUFO; }

	void Set_LaserTypeA(CGameObject* pLaserTypeA) { m_pLaserTypeA = pLaserTypeA; }
	CGameObject* Get_LaserTypeA() { return m_pLaserTypeA; }

	void Set_LaserTypeC(CGameObject* pLaserTypeC) { m_pLaserTypeC = pLaserTypeC; }
	CGameObject* Get_LaserTypeC() { return m_pLaserTypeC; }

	void Set_LaserTypeB_Recovery(_bool IsCheck) { m_IsLaserTypeB_Recovery = IsCheck; }
	_bool Get_LaserTypeB_Recovery() const { return m_IsLaserTypeB_Recovery; }

	void Set_MoonBaboonCore(CGameObject* pGameObject) { m_vecMoonBaboonCore.emplace_back(pGameObject); }
	CGameObject* Get_MoonBaboonCore(_uint iIndex) { return m_vecMoonBaboonCore[iIndex]; }

	void Set_MoonBaboon(CGameObject* pMoonBaboon) { m_pMoonBaboon = pMoonBaboon; }
	CGameObject* Get_MoonBaboon() { return m_pMoonBaboon; }
	
	/* For. MainLaser */
	_uint Get_FloorIndex() const { return m_iFloorIndex; }

	/* For.Pixel Radar */
	void Set_PixelUFO(CGameObject* pPixelUFO) { m_pPixelUFO = pPixelUFO; }
	CGameObject* Get_PixelUFO() { return m_pPixelUFO; }

	void Set_PixelCrossHair(CGameObject* pPixelCrossHair) { m_pPixelCrossHair = pPixelCrossHair; }
	CGameObject* Get_PixelCrossHair() { return m_pPixelCrossHair; }

	void Set_PixelLaser(CGameObject* pPixelLaser) { m_pPixelLaser = pPixelLaser; }
	CGameObject* Get_PixelLaser() { return m_pPixelLaser; }

	void Set_PixelBaboon(CGameObject* pPixelBaboon) { m_pPixelBaboon = pPixelBaboon; }
	CGameObject* Get_PixelBaboon() { return m_pPixelBaboon; }

	void Set_LaserGauge(_float fGauge) { m_fLaserGauge = fGauge; }
	_float Get_LaserGauge() { return m_fLaserGauge; }

	/* For.Minigame */
	void Set_CodyMinigameWinCount(_uint iScore) { m_iCodyMinigameWinCount += iScore; }
	_uint Get_CodyMinigameWinCount() { return m_iCodyMinigameWinCount; }
	void Set_MayMinigameWinCount(_uint iScore) { m_iMayMinigameWinCount += iScore; }
	_uint Get_MayMinigameWinCount() { return m_iMayMinigameWinCount; }

private:
	CGameObject* m_pCody = nullptr;
	CGameObject* m_pMay = nullptr;
	CGameObject* m_pMainCam = nullptr;
	CGameObject* m_pSubCam = nullptr;

	/* For.Logo */
	CGameObject* m_pReturnButton = nullptr;
	CGameObject* m_pParticipation_Cody = nullptr;
	CGameObject* m_pParticipation_May = nullptr;

	/* For.Script */
	CGameObject* m_pScript = nullptr;

	// UFO
	CGameObject* m_pUFO = nullptr;

	/* For.Bridge */
	CGameObject* m_pBridge = nullptr;

	// Space Rails
	unordered_map<const _tchar*, CGameObject*> m_SpaceRails;

	// Interactive Objects
	CGameObject* m_pRobot = nullptr;
	CGameObject* m_pRobotLever = nullptr;
	CGameObject* m_pRobotBattery = nullptr;
	CGameObject* m_pRobotHead = nullptr;
	CGameObject* m_pNoBatterySign = nullptr;
	CGameObject* m_pPressureBigPlate = nullptr;
	CGameObject* m_pUmbrella_Joystick = nullptr;
	CGameObject* m_pMenuScreen = nullptr;
	CGameObject* m_pHookUFO = nullptr;

private:
	// Interactive Objects
	vector<class CPressurePlate*>	m_vecPipeCurve;
	vector<CGameObject*>			m_vecMoonBaboonCore;

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

	/* For.BossRoom */
	CGameObject* m_pBossFloor = nullptr;
	CGameObject* m_pBossDoor01 = nullptr;
	CGameObject* m_pBossDoor02 = nullptr;
	CGameObject* m_pBossSlideDoor = nullptr;

	/* For.MoonUFO */
	CGameObject* m_pMoonUFO = nullptr;
	CGameObject* m_pMoon = nullptr;
	CGameObject* m_pRunningMoonBaboon = nullptr;

	/* For.MoonBaboonSpaceShip*/
	CGameObject* m_pMoonBaboonSpaceShip = nullptr;

	/* For.ControlRoom_Battery */
	CGameObject* m_pControlRoom_Battery = nullptr;

	/* For. UFORadarSet */
	CGameObject* m_pUFORadarSet = nullptr;
	CGameObject* m_pPixelUFO = nullptr;
	CGameObject* m_pPixelCrossHair = nullptr;
	CGameObject* m_pPixelLaser = nullptr;
	CGameObject* m_pPixelBaboon = nullptr;

	_float		 m_fLaserGauge = 0.f;

	/* For.Boss */
	CGameObject* m_pMoonBaboonMainLaser = nullptr;
	CGameObject* m_pBossUFO = nullptr;
	CGameObject* m_pLaserTypeA = nullptr;
	CGameObject* m_pLaserTypeC = nullptr;
	CGameObject* m_pMoonBaboon = nullptr;

private:
	/* For.Valve */
	_uint m_iValveRotateCount = 0;
	_uint m_iValveTarget = 0;
	_uint m_iCody_Valve_Count = 0;
	_uint m_iMay_Valve_Count = 0;
	_uint m_iFloorIndex = 0;

	_bool m_IsBigButtonPressed = false;
	_bool m_bGravityStageClear = false;
	_bool m_bPinBallStageClear = false;
	_bool m_bRailStageClear = false;
	_bool m_IsValveActivated = false;
	_bool m_IsLaserTypeB_Recovery = false;

	_uint m_iMayStageNum = 0;
	_uint m_iCodyStageNum = 0;
	_uint m_iCodyMinigameWinCount = 0;
	_uint m_iMayMinigameWinCount = 0;


public:
	virtual void Free() override;
};

END