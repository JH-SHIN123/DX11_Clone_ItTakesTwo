#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Pinball_Explosion final : public CInGameEffect
{
private:
	explicit CEffect_Pinball_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Pinball_Explosion(const CEffect_Pinball_Explosion& rhs);
	virtual ~CEffect_Pinball_Explosion() = default; public:

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
	HRESULT Ready_Smoke_Effect();
	_float4 Check_UV_Smoke(_double TimeDelta);
	void Check_Target_Matrix();

private:
	_float2 m_vTexUV_Smoke = { 0.f, 0.f };
	_float	m_fAlphaTime = 0.75f;
	class CPinBall* m_pTargetObject = nullptr;

public:
	static CEffect_Pinball_Explosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END