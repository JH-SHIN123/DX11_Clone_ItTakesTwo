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

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bSmash = false;

	_double			m_dDistance = 0.0;
	_double			m_dSmeshDistance = 0.5;
	_double			m_dCoolTime = 0.0;

private:
	void Movement(_double dTimeDelta);
	void Ready(_double dTimeDelta);
	void Smesh(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CPress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END