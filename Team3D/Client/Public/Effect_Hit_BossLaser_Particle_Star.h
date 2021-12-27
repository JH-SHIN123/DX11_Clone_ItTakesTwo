#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Hit_BossLaser_Particle_Star final : public CInGameEffect
{
private:
	explicit CEffect_Hit_BossLaser_Particle_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Hit_BossLaser_Particle_Star(const CEffect_Hit_BossLaser_Particle_Star& rhs);
	virtual ~CEffect_Hit_BossLaser_Particle_Star() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

public:
	_float2 Get_RandSize();

private:
	void Check_Instance(_double TimeDelta);

private:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0)		override;

private:
	void Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex = 0);

private:
	HRESULT Ready_InstanceBuffer();
	_float3 Get_Particle_Rand_Dir(_fvector vDefaultPos);

private:
	_double m_dControlTime = 0.5;
	_double m_dActivateTime = 0.0;
	_bool m_IsActivate = true;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT	= nullptr;

	_float  m_fInstance_SpeedPerSec = 1.5f;

	const _float  m_fAlphaTime_Power = 1.f;
	const _double m_dInstance_Pos_Update_Time = 1.0;
	const _float2 m_vDefaultSize = { 0.6f, 0.6f };

public:
	static CEffect_Hit_BossLaser_Particle_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END