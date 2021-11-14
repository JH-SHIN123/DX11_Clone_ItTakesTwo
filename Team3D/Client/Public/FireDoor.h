#pragma once

#ifndef __FIREDOOR_H__
#include "InGameEffect.h"

BEGIN(Client)
class CFireDoor final : public CInGameEffect
{
private:
	explicit CFireDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFireDoor(const CFireDoor& rhs);
	virtual ~CFireDoor() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_double dResetTime = 0.f;
	_float4 m_vWeight = { 0.f, 0.f, 0.125f, 0.125f };

	CTextures* m_pTexturesCom_ColorRamp = nullptr;

public:
	static CFireDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __FIREDOOR_H__
#endif 
