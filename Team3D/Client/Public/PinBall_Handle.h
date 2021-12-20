#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPinBall_Handle final : public CDynamic_Env
{
private:
	explicit CPinBall_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPinBall_Handle(const CPinBall_Handle& rhs);
	virtual ~CPinBall_Handle() = default;

public:/* Getter */
	_bool Get_RespawnAngle() { return m_bRespawnAngle; }
	_bool Get_RespawnPos()	 { return m_bRespawnPos; }
	_bool Get_Ready()		 { return m_bReady; }	
	_bool Get_Goal()		 { return m_bGoal; }	
	_bool Get_PlayerMove()	 { return m_bPlayerMove; }
	CTransform* Get_Transform() { return m_pTransformCom; }
public:/* Setter */
	void Set_PlayerMove(_bool _bPlayerMove)		{ m_bPlayerMove = _bPlayerMove; }
	void Set_RespawnAngle(_bool _bRespawnAngle) { m_bRespawnAngle = _bRespawnAngle; }
	void Set_RespawnPos(_bool _bRespawnPos)		{ m_bRespawnPos = _bRespawnPos; }
	void Set_Ready(_bool _bReady)				{ m_bReady = _bReady; }
	void Set_Goal()								{ m_bGoal = true; }

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
	CStaticActor*	m_pStaticActorCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_bool			m_bReady = false;
	_bool			m_bHandleReady = false;
	_bool			m_bRespawnAngleToMove = false;
	_bool			m_bPlayerMove = false;
	_bool			m_bRespawnAngle = false;
	_bool			m_bRespawnPos = false;
	_bool			m_bFinish = false;
	_bool			m_bGoal = false;
	_bool			m_bGoalTimeCheck = false;

	_bool			m_IsCollision = false;
	_float			m_fGoalTime = 0.f;
	_float			m_fReady = 0.f;
	_float			m_fHandleAngle = 0.f;
	_float			m_fRespawnPosX = 0.f;

private:
	void MoveMent(_double dTimeDelta);
	void PlayerMove();
	void Respawn_Angle(_double dTimeDelta);
	void Respawn_Pos(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static	CPinBall_Handle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	 Clone_GameObject(void* pArg) override;
	virtual void			 Free() override;
};
END