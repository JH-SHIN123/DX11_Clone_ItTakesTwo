#pragma once

#ifndef __EFFECT_GENERATOR_H__

#include "Client_Defines.h"
#include "Base.h"
#include "InGameEffect.h"

BEGIN(Client)

#define EFFECT CEffect_Generator::GetInstance()

enum class Effect_Value
{
	Walking_Smoke, Landing_Smoke, 
	Dash,
	Cody_Dead, Cody_Dead_Fire,
	Cody_Revive,
	May_Dead, May_Dead_Fire,
	May_Revive,
	May_Boots_Walking,
	Player_Dead_Explosion,
	RobotBattery_Spark,
	BossCore_Hit,
	BossCore_Smoke,
	BossCore_Explosion,
	BossCore_Lightning,
	BossCore_Lightning_Big,
	BossBomb,
	BossBomb_Pillar,
	BossBomb_Particle,
	BossBomb_Explosion,
	BossLaser_Charge,
	BossLaser_Explosion,
	BossGroundPound,
	BossGroundPound_Ring,
	BossGroundPound_Smoke,
	BossMissile_Smoke,
	BossMissile_Explosion,
	BossMissile_Particle,
	Boss_UFO_Flying,
	Boss_UFO_Flying_Particle,
	Boss_UFO_Flying_Particle_Flow,
	UFO_Inside_Battery_Spark,
	UFO_Inside_Battery_Particle,
	UFO_Inside_Battery_Explosion,
	UFO_Inside_ElectricWall_Spark,
	UFO_Inside_ElectricWall_Particle,
	UFO_Inside_ElectricWall_Explosion,
	UFO_Inside_PressWall_Smoke,
	UFO_Inside_PressWall_Particle,
	Effect_Value_End
};

enum class EPoint_Color	// Color_Ramp_%d
{					
	Violet	= 1,				
	Blue	= 2,				
	Red		= 3,				
	Lime	= 6,				
	Crimson = 7,			
	Green	= 8,				
	Yellow	= 9,				
	White	= 13,				
	Sepia	= 14,				
	Color_End = 0
};


class CEffect_Generator final : public CBase
{
	DECLARE_SINGLETON(CEffect_Generator)


	typedef struct tagEffectPointLight_Desc
	{
		_float3			vPosition;
		_float			fPointRadius;
		_float			fPointSaturation_Power;
		_float			fPointContrast_Power;
		EPoint_Color	ColorRampNumber;

		tagEffectPointLight_Desc() {}
		tagEffectPointLight_Desc(_float fRadius, _float Saturation_Power, _float fContrast_Power, _float3 vPos, EPoint_Color ColorNumber)
			:vPosition(vPos), ColorRampNumber(ColorNumber), fPointRadius(fRadius), fPointSaturation_Power(Saturation_Power), fPointContrast_Power(fContrast_Power){}
	}Effect_PointLight_Desc;

public:
	HRESULT Add_Effect(Effect_Value eEffect, _fmatrix WorldMatrix, void* pArg = nullptr);
	// 맵과 같이 배치되는 상호작용 없는 환경 이펙트는 다른걸로
	HRESULT Add_PointLight(Effect_PointLight_Desc* pLightArg = nullptr);

public:
	HRESULT Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Create_Prototype_Resource_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

public:
	void LoopSpawner(_double TimeDelta);
	_double m_dSpawnTerm = 5.0;

public:
	CEffect_Generator();
	virtual ~CEffect_Generator() = default;

private:
	EFFECT_DESC_CLONE::PLAYER_VALUE Check_Cody_Size(_fmatrix WorldMatrix);

private:
	CGameInstance* m_pGameInstance = nullptr;

private:
	HRESULT	Create_Prototype(_uint iLevelIndex, const _tchar* pPrototypeName, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EFFECT_DESC_PROTO* pData);
	_fmatrix Compute_Pivot(_vector vScale, _vector vRotate);


public:
	virtual void Free() override;
};
END
#define __EFFECT_GENERATOR_H__
#endif // !__EFFECT_GENERATOR_H__
