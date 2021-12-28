#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_WallLaser_Trap_Plane final : public CInGameEffect
{
private:
	explicit CEffect_WallLaser_Trap_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_WallLaser_Trap_Plane(const CEffect_WallLaser_Trap_Plane& rhs);
	virtual ~CEffect_WallLaser_Trap_Plane() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;


public:
	void Set_Activate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix);
	void Update_Trigger_Position();

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	_bool	m_IsActivate = false;
	_double m_dActivateTime = 0.0;
	_double dResetTime = 0.f;
	_float4 m_vWeight = { 0.f, 0.f, 0.125f, 0.125f };

	const _float3 m_vOffSet_Pos = { 0.f, 0.f, 0.f };

private:
	CTextures*		m_pTexturesCom_ColorRamp = nullptr;

public:
	static CEffect_WallLaser_Trap_Plane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END