#pragma once
#include "InGameEffect.h"
class CPlayer_DeadParticle final : public CInGameEffect
{
private:
	explicit CPlayer_DeadParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer_DeadParticle(const CPlayer_DeadParticle& rhs);
	virtual ~CPlayer_DeadParticle() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	_float4 Set_particleUV(_int iIndex, _int U, _int V);

private: // 0 연기 1 ~ 파티클
	_double m_dAlphaTime = 1.5;
	_bool	m_IsDisapear = false;
	CTextures* m_pTexturesCom_Particle = nullptr;
	CTextures* m_pTexturesCom_Particle_Mask = nullptr;


public:
	static CPlayer_DeadParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

