#pragma once

#ifndef __EFFECT_BOSS_LASER_PARTICLE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_Laser_Particle final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Laser_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Laser_Particle(const CEffect_Boss_Laser_Particle& rhs);
	virtual ~CEffect_Boss_Laser_Particle() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_Position(_fvector vPos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos); }


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

private:
	_double m_dControlTime = 0.0; //
	_bool m_IsActivate = true;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT		= nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT		= nullptr;
	_double*							m_pInstance_Parabola_Time	= nullptr;
	_float3*							m_pInstanceBiffer_Dir		= nullptr;

	const _float  m_fAlphaTime_Power			= 1.f;
	const _float  m_fInstance_SpeedPerSec		= 2.5f;
	const _double m_dInstance_Pos_Update_Time	= 1.5;
	const _float2 m_vDefaultSize				= { 0.2f, 1.f };

public:
	static CEffect_Boss_Laser_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_BOSS_LASER_PARTICLE_H__
#endif // !__EFFECT_BOSS_LASER_PARTICLE_H__
