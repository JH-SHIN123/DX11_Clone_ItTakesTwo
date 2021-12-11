#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CLaserActivation final : public CDynamic_Env
{
private:
	explicit CLaserActivation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserActivation(const CLaserActivation& rhs);
	virtual ~CLaserActivation() = default;

public:/* Setter */
	void Change_State() { m_bMovement = true; }

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

	_bool			m_bActivate		  = false;
	_bool			m_bMovement		  = false;
	_bool			m_bCreateLaser	  = false;

	_float			m_fDistance		  = 1.3f;
	_float			m_fDistanceCheck  = 0.f;
	_float			m_fMaxPosY		  = 0.f;
	_float			m_fMinPosY		  = 0.f;

private:
	void Movement(_double dTimeDelta);
	void CreateLaser(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserActivation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*     Clone_GameObject(void* pArg);
	virtual void			 Free();
};
END