#pragma once

#ifndef __EFFECT_BOSS_CORE_H__

#include "InGameEffect.h"

BEGIN(Client)

class CEffect_Boss_Core final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Core(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Core(const CEffect_Boss_Core& rhs);
	virtual ~CEffect_Boss_Core() = default; public:

public:
	void HitOn() { m_IsHit = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Pos(_fvector vPos);

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	_int Explosion();

private:
	HRESULT Ready_Instance();
	_float4 Check_UV();

private:
	CTextures* m_pTextureCom_Color = nullptr;
	_bool	m_IsHit				= false;
	_float	m_fTextureUV_Time	= 0.f;
	XMINT2 m_vTexUV = { 0, 0 };
	const XMINT2 m_vTexUV_Max			= { 8, 8 };
	_float3 m_vOffSet_Pos		= { 0.f, -4.f, 0.f };
	_float	m_fDistortion_Time	= 0.f;
	_float	m_fDegree_Angle		= 0.f;

public:
	static CEffect_Boss_Core* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_BOSS_CORE_H__
#endif // !__EFFECT_BOSS_CORE_H__
