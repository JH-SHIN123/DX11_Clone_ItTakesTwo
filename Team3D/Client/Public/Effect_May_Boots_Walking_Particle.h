#pragma once

#ifndef __EFFECT_MAY_BOOTS_WALKING_PARTICLE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_May_Boots_Walking_Particle final : public CInGameEffect
{
private:
	explicit CEffect_May_Boots_Walking_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_May_Boots_Walking_Particle(const CEffect_May_Boots_Walking_Particle& rhs);
	virtual ~CEffect_May_Boots_Walking_Particle() = default; public:

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
	HRESULT Ready_Smoke_Effect();
	HRESULT Ready_Instance();
	_float4 Check_UV_Smoke(_double TimeDelta);
	void	Check_Point_Move(_double TimeDelta, _int iIndex);
	void	Check_Point_Size(_double TimeDelta, _int iIndex);

private:
	_float2 m_vTexUV_Smoke = { 0.f, 0.f };
	_float	m_fSpeedPerSec = 1.f;
	VTXMATRIX_CUSTOM_ST* m_pInstanceBuffer_Smoke = nullptr;
	

public:
	static CEffect_May_Boots_Walking_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_MAY_BOOTS_WALKING_PARTICLE_H__
#endif
