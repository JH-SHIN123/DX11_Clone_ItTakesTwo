#pragma once
#include "Dynamic_Env.h"
#include "LaserTennis_Manager.h"

BEGIN(Client)
class CLaserButtonLarge final : public CDynamic_Env
{
private:
	explicit CLaserButtonLarge(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserButtonLarge(const CLaserButtonLarge& rhs);
	virtual ~CLaserButtonLarge() = default;

public:
	void Activation() { m_bActiveMove = true; m_bActive = true; }

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

	_bool			m_bActive = false;
	_bool			m_bActiveMove = false;
	_bool			m_bCollision = false;
	_bool			m_bMovement = false;

	_float			m_fDistance = 0.6f;
	_float			m_fMaxPosY = 0.f;

	_uint			m_iLaserCount = 0;
	_double			m_dLaserCreateTime = 0.0;

	CLaserTennis_Manager::TARGET	m_eTarget = CLaserTennis_Manager::TARGET::TARGET_END;

private:
	void Movement(_double dTimeDelta);
	void Activaion_Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserButtonLarge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END