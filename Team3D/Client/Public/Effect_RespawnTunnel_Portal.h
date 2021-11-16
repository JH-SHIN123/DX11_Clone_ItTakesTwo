#pragma once

#ifndef __EFFECT_RESPAWNTUNNEL_PORTAL_H__

#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_RespawnTunnel_Portal final : public CInGameEffect_Model
{
private:
	explicit CEffect_RespawnTunnel_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_RespawnTunnel_Portal(const CEffect_RespawnTunnel_Portal& rhs);
	virtual ~CEffect_RespawnTunnel_Portal() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;
	virtual void SetUp_WorldMatrix(_fmatrix WorldMatrix) override;


public:
	HRESULT Ready_Instance();

public:
	static CEffect_RespawnTunnel_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_RESPAWNTUNNEL_PORTAL_H__
#endif // !__EFFECT_RESPAWNTUNNEL_PORTAL_H__
