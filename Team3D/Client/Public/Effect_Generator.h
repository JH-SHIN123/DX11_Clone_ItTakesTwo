#pragma once

#ifndef __EFFECT_GENERATOR_H__

#include "Client_Defines.h"
#include "Base.h"
#include "InGameEffect.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)

enum class Effect_Value
{
	Walking_Smoke, Landing_Smoke, 
	Dash,
	Cody_Dead,
	Cody_Revive,
	May_Dead,
	Effect_Value_End
};

class CEffect_Generator final : public CBase
{
	DECLARE_SINGLETON(CEffect_Generator)

public:
	HRESULT Add_Effect(Effect_Value eEffect, _fmatrix WorldMatrix, void* pArg = nullptr);
	// 맵과 같이 배치되는 상호작용 없는 환경 이펙트는 다른걸로

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
