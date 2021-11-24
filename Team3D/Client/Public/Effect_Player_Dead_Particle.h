#pragma once

#ifndef __EFFECT_PLAYER_DEAD_PARTICLE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Player_Dead_Particle final : public CInGameEffect
{
private:
	explicit CEffect_Player_Dead_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Player_Dead_Particle(const CEffect_Player_Dead_Particle& rhs);
	virtual ~CEffect_Player_Dead_Particle() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	_float4 Set_particleUV(_int iIndex, _int U, _int V);
	void	Set_VtxColor(_int iIndex, _uint iVtxIndex);

private:
	_double m_dAlphaTime = 1.5;
	_bool	m_IsDisapear = false;
	CTextures* m_pTexturesCom_Particle = nullptr;
	CTextures* m_pTexturesCom_Particle_Mask = nullptr;
	CModel* m_pTargetModel = nullptr;

public:
	static CEffect_Player_Dead_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_PLAYER_DEAD_PARTICLE_H__
#endif
