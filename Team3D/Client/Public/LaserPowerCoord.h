#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CLaserPowerCoord final : public CDynamic_Env
{
private:
	explicit CLaserPowerCoord(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserPowerCoord(const CLaserPowerCoord& rhs);
	virtual ~CLaserPowerCoord() = default;

public:/* Setter */
	void Change_State() { m_bMovement = true; }
	void Set_UICehck_Cody(_bool bCheck) { m_bUICheck_Cody = bCheck; }
	void Set_UICehck_May(_bool bCheck) { m_bUICheck_May = bCheck; }

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

	_bool			m_bReady;
	_bool			m_bMovement = false;
	_bool			m_bActive = false;
	_bool			m_bCollision_Cody = false;
	_bool			m_bCollision_May = false;
	_bool			m_bUICheck_Cody = false;
	_bool			m_bUICheck_May = false;

	_float			m_fDistance = 2.5f;
	_float			m_fResetPosZ = 0.f;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserPowerCoord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END