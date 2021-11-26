#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CRobotBattery;
class CRobotLever;
class CRobot;
class CNoBatterySign;
class CRobotHead;

class CRobotParts abstract : public CGameObject
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

public:
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	CRobotBattery* m_pRobotBattery = nullptr;
	CRobotLever* m_pRobotLever = nullptr;
	CRobot* m_pRobotBody = nullptr;
	CNoBatterySign* m_pNoBatterySign = nullptr;
	CRobotHead* m_pRobotHead = nullptr;

public:
	virtual void Free() override;
};
END