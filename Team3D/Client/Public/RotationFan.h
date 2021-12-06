#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CRotationFan final : public CDynamic_Env
{
private:
	explicit CRotationFan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotationFan(const CRotationFan& rhs);
	virtual ~CRotationFan() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRotationFan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END