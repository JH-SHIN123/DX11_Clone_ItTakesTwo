#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPress final : public CDynamic_Env
{
private:
	explicit CPress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPress(const CPress& rhs);
	virtual ~CPress() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;
	virtual void	OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_bool			m_bSmash = true;
	_float			m_fOpenPos = 0.f;
	_float			m_fClosePos = 0.f;

	_float3			m_vOpenPos = {};
	_float3			m_vClosePos = {};

	_double			m_dDistance = 0.f;
	_double			m_dCoolTime = 0.0;

private:
	void Movement(_double dTimeDelta);
	void Open_Press(_double dTimeDelta);
	void Close_Press(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CPress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END