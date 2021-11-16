#pragma once

#ifndef __EFFECT_FIREDOOR_H__
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_FireDoor final : public CInGameEffect
{
private:
	explicit CEffect_FireDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_FireDoor(const CEffect_FireDoor& rhs);
	virtual ~CEffect_FireDoor() = default;

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


private:
	_double dResetTime = 0.f;
	_float4 m_vWeight = { 0.f, 0.f, 0.125f, 0.125f };

	CTextures* m_pTexturesCom_ColorRamp = nullptr;

public:
	static CEffect_FireDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_FIREDOOR_H__
#endif 
