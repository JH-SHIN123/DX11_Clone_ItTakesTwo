#pragma once

#ifndef __WALKING_SMOKE_H__

#include "GameEffect.h"
class CWalking_Smoke final : public CGameEffect
{
private:
	explicit CWalking_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWalking_Smoke(const CWalking_Smoke& rhs);
	virtual ~CWalking_Smoke() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float m_fSizeTime = 0.f;

public:
	static CWalking_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __WALKING_SMOKE_H__
#endif // !__WALKING_SMOKE_H__
