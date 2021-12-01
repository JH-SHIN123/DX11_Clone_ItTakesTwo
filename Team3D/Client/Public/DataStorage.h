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

	void Set_ValveCount(_uint _iValveRotateCount) { m_iValveRotateCount += _iValveRotateCount; }
	void Set_ValveTarget(_uint _iValveTarget) { m_iValveTarget = _iValveTarget; }
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
	CGameObject* m_pPressureBigPlate = nullptr;
	CGameObject* m_pUmbrella_Joystick = nullptr;

private:
	// Interactive Objects
	vector<class CPressurePlate*>	m_vecPipeCurve;


	CGameObject* m_pSTGravityRobot = nullptr;
	CGameObject* m_pSTPinBallRobot = nullptr;
	CGameObject* m_pSTPlanetRobot = nullptr;


private:
	/* For.Valve */
	_uint m_iValveRotateCount = 0;
	_uint m_iValveTarget = 0;

public:
	virtual void Free() override;
};

END