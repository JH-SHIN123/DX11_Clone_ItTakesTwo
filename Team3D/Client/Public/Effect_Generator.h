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
	Cody_Rail,
	Cody_PinBall_Move, Cody_PinBall_Explosion, Cody_PinBall_Explosion_Particle, 
	Cody_Size_ShokeWave,
	May_Dead, May_Dead_Fire,
	May_Revive,
	May_Rail,
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

	BossMissile_Smoke_Test,
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

	Boss_BrokenLaser_Smoke,
	Boss_BrokenLaser_Particle,
	Boss_BrokenLaser_Lightning,
	Boss_BrokenLaser_Flow,

	Hit_Planet_Smoke,
	Hit_Planet_Particle,

	Hit_BossLaser,
	Hit_BossLaser_Particle,
	Hit_BossLaser_Particle_Star,

	WarpGate_Clear,

	MoonBaboon_Shield,
	MoonBaboon_Booster,

	StarBuddy_Explosion_Pillar,
	StarBuddy_Explosion_BigBang,
	StarBuddy_Explosion_Particle,

	PipeLocker_Connected,
	PipeLocker_Ball,
	PipeLocker_Particle,

	Gate_Smoke,

	EndingRocket_Smoke,
	EndingRocket_Boost,
	EndingRocket_Circle,


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

enum class Effect_Value_CutScene
{
	Cody_Size_ML,
	Cody_Size_LS,
	Cody_Size_SL,
	Cody_Size_SM,
	MoonBaboon_Land,

	Effect_Value_End
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

	typedef struct tagCutSceneTimer_Desc
	{
		_float fTime = 0.f;
		Effect_Value_CutScene eEffect_Value_CS = Effect_Value_CutScene::Effect_Value_End;

		tagCutSceneTimer_Desc(){}
		tagCutSceneTimer_Desc(_float fTime, Effect_Value_CutScene eEffect_Value_CS)
			:fTime(fTime), eEffect_Value_CS(eEffect_Value_CS){}
	}CutSceneTimer_Desc;

public:
	HRESULT Add_Effect(Effect_Value eEffect, _fmatrix WorldMatrix = XMMatrixIdentity(), void* pArg = nullptr);
	HRESULT Add_PointLight(Effect_PointLight_Desc* pLightArg = nullptr, CGameObject** ppOut = nullptr);
	HRESULT Add_Effect_CutScene(Effect_Value_CutScene eEffect, _fmatrix WorldMatrix = XMMatrixIdentity());
	HRESULT Add_Effect_CutScene_Timer(_float fTime, Effect_Value_CutScene eEffect);

public:
	enum EPlayer_Type { Type_May, Type_Cody, Type_End };
	_bool	Get_PlayerRail_Effect(EPlayer_Type eType) { return m_IsPlayerRail_Effect[eType]; }
	void	Set_PlayerRail_Effect(EPlayer_Type eType, _bool IsEffect) { m_IsPlayerRail_Effect[eType] = IsEffect; }
	_bool	m_IsPlayerRail_Effect[EPlayer_Type::Type_End] = { false, false };

public:
	HRESULT Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Create_Prototype_Resource_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Create_CutScene_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

public:
	void CutScene_Effect_Timer(_double dDeltaTime);

public:
	CEffect_Generator();
	virtual ~CEffect_Generator() = default;

private:
	EFFECT_DESC_CLONE::PLAYER_VALUE Check_Cody_Size(_fmatrix WorldMatrix);
	list<tagCutSceneTimer_Desc> m_listEffectTimer;

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
