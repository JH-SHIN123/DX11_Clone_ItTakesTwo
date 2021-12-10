#pragma once

#ifndef __EFFECT_HIT_BOSSLASER_H__

#include "InGameEffect.h"
class CEffect_Hit_BossLaser final : public CInGameEffect
{
private:
	explicit CEffect_Hit_BossLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Hit_BossLaser(const CEffect_Hit_BossLaser& rhs);
	virtual ~CEffect_Hit_BossLaser() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;


public:
	static CEffect_Hit_BossLaser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __EFFECT_HIT_BOSSLASER_H__
#endif // !__EFFECT_HIT_BOSSLASER_H__
