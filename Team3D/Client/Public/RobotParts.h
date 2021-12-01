#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CRobotBattery;
class CRobotLever;
class CRobot;
class CNoBatterySign;
class CRobotHead;

typedef struct tagRobotDesc
{
	_uint	iStageNum = ST_GRAVITYPATH;
	_vector vPosition = {};
	_float  RotX = 0.f;
	_float  RotY = 0.f;
	_float  RotZ = 0.f;

}ROBOTDESC;

class CRobotParts : public CGameObject
{
protected:
	explicit CRobotParts(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRobotParts(const CRobotParts& rhs);
	virtual ~CRobotParts() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CRobotParts* Get_Parent() { return this; }
	CRobotBattery* Get_Robot_Battery() { return m_pRobotBattery;  }
	CRobotLever* Get_Robot_Lever() { return m_pRobotLever; }
	CRobot* Get_Robot_Body() { return m_pRobotBody; }
	CNoBatterySign* Get_NoBatterySign() { return m_pNoBatterySign; }
	CRobotHead* Get_RobotHead() { return m_pRobotHead; }


private:
	CRobotBattery*		   	 m_pRobotBattery = nullptr;
	CRobotLever*			 m_pRobotLever = nullptr;
	CRobot*					 m_pRobotBody = nullptr;
	CNoBatterySign*			 m_pNoBatterySign = nullptr;
	CRobotHead*				 m_pRobotHead = nullptr;

private:
	ROBOTDESC m_tRobotDesc;

public:
	static CRobotParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END