#pragma once

#ifndef __EFFECT_ROBOT_BATTERY_SPARK_H__

// ÆøÇ³Àº µÎ¹øÄ£´Ù.
// ÆøÇ³Àº µÎ¹øÄ£´Ù.

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Robot_Battery_Spark final : public CInGameEffect
{
private:
	explicit CEffect_Robot_Battery_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Robot_Battery_Spark(const CEffect_Robot_Battery_Spark& rhs);
	virtual ~CEffect_Robot_Battery_Spark() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg); 
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	HRESULT Ready_Instance();

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	void Check_Particle_UV(_double TimeDelta);

private:
	_vector Get_RandDir(_int3 iRandPower, _vector vDir);

private:
	const _float4 m_vColor = { 0.490196078f, 0.874509803f, 1.000000000f, 1.000000000f };
	_int	m_iEffectCount = 0;
	_float	m_fResetTime = 0.f;
	_float	m_fChange_UV = 0.2f;
	_bool	m_IsNoRender = false;

public:
	static CEffect_Robot_Battery_Spark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
#define __EFFECT_ROBOT_BATTERY_SPARK_H__
#endif // !__EFFECT_ROBOT_BATTERY_SPARK_H__
