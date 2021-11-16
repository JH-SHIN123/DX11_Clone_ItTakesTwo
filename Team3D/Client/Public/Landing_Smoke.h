#pragma once

#ifndef __LANDING_SMOKE_H__

#include "InGameEffect.h"
class CLanding_Smoke final : public CInGameEffect
{
private:
	explicit CLanding_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLanding_Smoke(const CLanding_Smoke& rhs);
	virtual ~CLanding_Smoke() = default; public:

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
	void Control_Alpha(_double TimeDelta);
	void Control_Instance(_double TimeDelta);

private:
	_double m_dAlphaTime = 1.5;
	_bool	m_IsDisapear = false;

public:
	static CLanding_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __LANDING_SMOKE_H__
#endif // !__LANDING_SMOKE_H__
