#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_Gravitational_Bomb_Pillar final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Gravitational_Bomb_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Gravitational_Bomb_Pillar(const CEffect_Boss_Gravitational_Bomb_Pillar& rhs);
	virtual ~CEffect_Boss_Gravitational_Bomb_Pillar() = default; public:

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

private:
	HRESULT Ready_Instance();

private:
	_float m_fTime = 1.5f;
	_float2 m_vSize = { 0.01f, 0.5f };
	const _float2 m_vSize_Max = { 1.f, 20.f };
	_float4 m_vColor = { 0.980392218f, 0.201960814f, 0.147058856f, 1.000000000f };


public:
	static CEffect_Boss_Gravitational_Bomb_Pillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END