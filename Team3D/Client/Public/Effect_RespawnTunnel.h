#pragma once

#ifndef __EFFECT_RESPAWNTUNNEL_H__

#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_RespawnTunnel final : public CInGameEffect_Model
{
private:
	explicit CEffect_RespawnTunnel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_RespawnTunnel(const CEffect_RespawnTunnel& rhs);
	virtual ~CEffect_RespawnTunnel() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;
	virtual void SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

public:
	static CEffect_RespawnTunnel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_RESPAWNTUNNEL_H__
#endif // !__EFFECT_RESPAWNTUNNEL_H__
