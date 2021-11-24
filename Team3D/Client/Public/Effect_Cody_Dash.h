#pragma once

#ifndef __EFFECT_CODY_DASH_H__

#include "InGameEffect.h"

BEGIN(Client)

class CEffect_Cody_Dash final : public CInGameEffect
{
private:
	explicit CEffect_Cody_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Cody_Dash(const CEffect_Cody_Dash& rhs);
	virtual ~CEffect_Cody_Dash() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;


public:
	static CEffect_Cody_Dash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_CODY_DASH_H__
#endif // !__EFFECT_CODY_DASH_H__
