#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPInBall_Blocked final : public CDynamic_Env
{
private:
	explicit CPInBall_Blocked(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPInBall_Blocked(const CPInBall_Blocked& rhs);
	virtual ~CPInBall_Blocked() = default;

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

public:
	static CPInBall_Blocked* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END