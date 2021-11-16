#pragma once

#ifndef __RESPAWNTUNNEL_SMOKE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CRespawnTunnel_Smoke final : public CInGameEffect
{
private:
	explicit CRespawnTunnel_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRespawnTunnel_Smoke(const CRespawnTunnel_Smoke& rhs);
	virtual ~CRespawnTunnel_Smoke() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

public:
	static CRespawnTunnel_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __RESPAWNTUNNEL_SMOKE_H__
#endif
