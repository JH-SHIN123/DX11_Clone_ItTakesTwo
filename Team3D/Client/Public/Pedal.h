#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPedal final : public CDynamic_Env
{
private:
	explicit CPedal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPedal(const CPedal& rhs);
	virtual ~CPedal() = default;

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

	_bool			m_bDelay = false;
	_bool			m_bSmash = false;

	_float			m_fProgressAnim = 0.f;
	_float			m_fAddAngle = 0.f;
	_float4x4		m_matResetWorld;

	_bool			m_bSoundCheck = false;
	_bool			m_bSoundOnePlay = false;

private:
	void Movement(_double dTimeDelta);
	void PlaySound_by_PlayerDistance();

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CPedal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END