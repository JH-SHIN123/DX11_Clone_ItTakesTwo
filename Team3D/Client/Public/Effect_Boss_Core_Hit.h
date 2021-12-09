#pragma once

#ifndef __EFFECT_BOSS_CORE_HIT_H__

#include "InGameEffect.h"
class CEffect_Boss_Core_Hit final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Core_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Core_Hit(const CEffect_Boss_Core_Hit& rhs);
	virtual ~CEffect_Boss_Core_Hit() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }


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
	_double m_dControlTime = 0.5; //
	_bool m_IsActivate = true;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	_double*							m_pInstance_Parabola_Time = nullptr;
	_float3*							m_pInstanceBiffer_Dir = nullptr;

	const _float  m_fAlphaTime_Power = 1.f;
	const _float  m_fInstance_SpeedPerSec = 2.5f;
	const _double m_dInstance_Pos_Update_Time = 1.5;
	const _float2 m_vDefaultSize = { 0.1f, 0.3f };



public:
	static CEffect_Boss_Core_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __EFFECT_BOSS_CORE_HIT_H__
#endif // !__EFFECT_BOSS_CORE_HIT_H__
