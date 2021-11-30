#pragma once

#include "Client_Defines.h"
#include "RobotParts.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
class CTriggerActor;
END

BEGIN(Client)

class CRobotLever final : public CRobotParts
{
private:
	explicit CRobotLever(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRobotLever(const CRobotParts& rhs);
	virtual ~CRobotLever() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	void Set_BatteryCharged(_bool _bBatteryCharged) { m_bBatteryCharged = _bBatteryCharged; }

private:
	void Activate_Lever(_double dTimeDelta);

private:
	_bool		m_bRotate = false;
	_bool		m_IsCollide = false;
	_bool		m_bBatteryCharged = false;
	_bool		m_bUpdate = true;
	_float		m_fStopDelay = 0.f;

	_bool		m_bNoBatteryHit = false;

	ROBOTDESC	m_tRobotPartsDesc;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;

public:
	static CRobotLever* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END