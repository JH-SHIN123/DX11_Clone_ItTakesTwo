#pragma once

#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_Boss_Gravitational_Bomb final : public CInGameEffect_Model
{
private:
	explicit CEffect_Boss_Gravitational_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Gravitational_Bomb(const CEffect_Boss_Gravitational_Bomb& rhs);
	virtual ~CEffect_Boss_Gravitational_Bomb() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;


private:
	void Scale_Check(_double TimeDelta);
	void Explosion_Check();

private:
	CTextures* m_pTexture_Distortion = nullptr;

private:
	_bool m_IsExplosion = false;
	_bool m_IsParticleCreate = true;

private:
	_bool	m_IsScaling_End = false;
	_float	m_fTime = 0.f;
	_float	m_fAlphaTime = 1.f;
	_double m_dLifeTime = 20.0;
	_float4 m_vDefulat_Scale = { 0.25f, 0.25f, 0.25f, 0.f };
	_float4 m_vColor = { 0.980392218f, 0.201960814f, 0.147058856f, 1.000000000f };
	_float m_fJumpTime = 0.f;
	_float m_fJumpY = 0.f;

	_float m_fScale_Pow = 7.f;
	const _float m_fScale_Max = 13.f;
	PxRaycastBuffer	m_RaycastBuffer;
	
private:
	class CEffect_Boss_Gravitational_Bomb_Particle* m_pParticle = nullptr;

public:
	static CEffect_Boss_Gravitational_Bomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END

