#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CLaserButton final : public CDynamic_Env
{
private:
	explicit CLaserButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserButton(const CLaserButton& rhs);
	virtual ~CLaserButton() = default;

public:
	_bool Get_Active() { return m_bActive; }

public:
	void Active_Button() { m_bActive = true; }

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
	_bool			m_bCollision = false;

	_float			m_fDistance = 0.1f;
	_float			m_fResetPosY = 0.f;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END