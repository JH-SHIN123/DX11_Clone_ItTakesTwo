#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CRotatedRobotBattery;
class CRotatedRobotLever;
class CRotatedNoBatterySign;
class CRotatedRobotHead;
class CRotatedRobotBody;

typedef struct tagRotatedRobotDesc
{
	_uint	iStageNum = ST_PINBALL;
	_vector vPosition = {};
	_float  RotX = 0.f;
	_float  RotY = 0.f;
	_float  RotZ = 0.f;
}RTROBOTDESC;

class CRotatedRobotParts : public CGameObject
{
protected:
	explicit CRotatedRobotParts(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotatedRobotParts(const CRotatedRobotParts& rhs);
	virtual ~CRotatedRobotParts() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CRotatedRobotBody* Get_Robot_Body() { return m_pRTRobotBody; }
	CRotatedRobotBattery* Get_Robot_Battery() { return m_pRTRobotBattery; }
	CRotatedRobotLever* Get_Robot_Lever() { return m_pRTRobotLever; }
	CRotatedNoBatterySign* Get_NoBatterySign() { return m_pRTNoBatterySign; }
	CRotatedRobotHead* Get_RobotHead() { return m_pRTRobotHead; }

private:
	/* Child */
	CRotatedRobotBody*		m_pRTRobotBody = nullptr;
	CRotatedRobotBattery*	m_pRTRobotBattery = nullptr;
	CRotatedRobotLever*		m_pRTRobotLever = nullptr;
	CRotatedNoBatterySign*   m_pRTNoBatterySign = nullptr;
	CRotatedRobotHead*	    m_pRTRobotHead = nullptr;

private:
	RTROBOTDESC m_tRtRobotDesc;

public:
	static CRotatedRobotParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END