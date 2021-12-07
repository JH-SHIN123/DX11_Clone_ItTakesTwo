#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CElectricWall final : public CDynamic_Env
{
private:
	explicit CElectricWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CElectricWall(const CElectricWall& rhs);
	virtual ~CElectricWall() = default;

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

public:
	_bool Get_Electric() { return m_bElectric; }

private:
	CStaticActor*	m_pStaticActorCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_bool			m_bElectric = false;
	_float3			m_vOriginPos = {};

	_double			m_dElectricTime = 0.0;
	_double			m_dCoolTime = 0.0;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CElectricWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END