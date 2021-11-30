#pragma once

#include "Client_Defines.h"
#include "RobotParts.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRobotHead final : public CRobotParts
{
protected:
	explicit CRobotHead(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRobotHead(const CRobotParts& rhs);
	virtual ~CRobotHead() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;

	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;


	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	void Set_Battery_Charged(_bool _bBatteryCharged) { m_bBatteryCharged = _bBatteryCharged; }
	void Set_Lever_Active(_bool _bLeverActive) { m_bLeverActive = _bLeverActive; }
	void Set_Lever_Hit_When_NoBattery(_bool _bHitLeverInActive) { m_bHitLeverInActive = _bHitLeverInActive; }

private:
	void Hit_Lever_InActive(_double dTimeDelta);

private:
	_bool		m_bBatteryCharged = false;
	_bool		m_bLeverActive = false;

	// 배터리업는데 레버쳤을때
	_bool		m_bHitLeverInActive = false;
	ROBOTDESC	m_tRobotPartsDesc;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

public:
	static CRobotHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END