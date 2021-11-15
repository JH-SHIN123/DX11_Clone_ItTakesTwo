#pragma once

#ifndef __RESPAWNTUNNEL_PORTAL_H__

#include "InGameEffect_Model.h"

BEGIN(Client)
class CRespawnTunnel_Portal final : public CInGameEffect_Model
{
private:
	explicit CRespawnTunnel_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRespawnTunnel_Portal(const CRespawnTunnel_Portal& rhs);
	virtual ~CRespawnTunnel_Portal() = default;

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
	static CRespawnTunnel_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __RESPAWNTUNNEL_PORTAL_H__
#endif // !__RESPAWNTUNNEL_PORTAL_H__
