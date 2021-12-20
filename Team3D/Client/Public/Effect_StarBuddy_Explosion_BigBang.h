#pragma once
#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_StarBuddy_Explosion_BigBang final : public CInGameEffect_Model
{
private:
	explicit CEffect_StarBuddy_Explosion_BigBang(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_StarBuddy_Explosion_BigBang(const CEffect_StarBuddy_Explosion_BigBang& rhs);
	virtual ~CEffect_StarBuddy_Explosion_BigBang() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

private:
	_float3 m_vScale = { 0.5f, 0.5f, 0.5f };
	_bool  m_IsScaling = true;
	_double m_dLifeTime = 0.0;
	_float m_fAlphaTime = 0.0f;
	_float m_fScalePow = 20.f;

	_float4 m_vColor = { 0.780392218f, 0.700392218f, 0.147058856f, 1.000000000f };

public:
	static CEffect_StarBuddy_Explosion_BigBang* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END