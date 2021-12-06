#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPinBall_Door final : public CDynamic_Env
{
private:
	explicit CPinBall_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPinBall_Door(const CPinBall_Door& rhs);
	virtual ~CPinBall_Door() = default;

public:/* Setter */
	/* false:Open, true:Close*/
	void Set_DoorState(_bool bState);
	void Set_Goal() { m_bGoal = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CTriggerActor*	m_pTriggerActorCom = nullptr;
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bGoal = false;
	_bool			m_bTrigger = false;
	_bool			m_bDoorState = false;
	_float			m_fDistance = 0.f;
	_float3			m_ResetPos = {};

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CPinBall_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*   Clone_GameObject(void* pArg) override;
	virtual void		   Free() override;
};
END