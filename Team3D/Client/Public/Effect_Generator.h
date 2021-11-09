#pragma once

#ifndef __EFFECT_GENERATOR_H__

#include "Client_Defines.h"
#include "Base.h"
#include "GameEffect.h"

BEGIN(Client)

enum class Effect_Value
{

};

class CEffect_Generator final : public CBase
{
	DECLARE_SINGLETON(CEffect_Generator)

public:
	HRESULT Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Create_Prototype_Resource_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

public:
	CEffect_Generator();
	virtual ~CEffect_Generator() = default;

private:
	HRESULT Create_Prototype(_uint iLevelIndex, const _tchar* pPrototypeName, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EFFECT_DESC_PROTO* pData);

public:
	virtual void Free() override;
};
END
#define __EFFECT_GENERATOR_H__
#endif // !__EFFECT_GENERATOR_H__
