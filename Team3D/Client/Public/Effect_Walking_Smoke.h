#pragma once

#ifndef __EFFECT_WALKING_SMOKE_H__

#include "InGameEffect.h"
class CEffect_Walking_Smoke final : public CInGameEffect
{
private:
	explicit CEffect_Walking_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Walking_Smoke(const CEffect_Walking_Smoke& rhs);
	virtual ~CEffect_Walking_Smoke() = default; public:

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
	void Control_Alpha(_double TimeDelta);
	void Control_Instance(_float TimeDelta);

private:
	_float m_fSizeTime = 0.f;
	_double m_dAlphaTime = 0.0;
	_bool	m_IsDisapear = false;


public:
	static CEffect_Walking_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __EFFECT_WALKING_SMOKE_H__
#endif // !__WALKING_SMOKE_H__
