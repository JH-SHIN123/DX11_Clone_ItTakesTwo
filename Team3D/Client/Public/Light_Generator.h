#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Effect_Generator.h"

BEGIN(Client)
class CLight_Generator final : public CBase
{
	DECLARE_SINGLETON(CLight_Generator)

public:
	CLight_Generator() = default;
	virtual ~CLight_Generator() = default;

public:
	HRESULT	Add_Light(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, EPoint_Color eEffectColor, _bool isActive = true);
	void	Clear_Lights();

public:
	virtual void Free() override;

private:
	typedef unordered_map<const _tchar*, class CEffectLight*> EFFECT_LIGHTS;
	
private:
	EFFECT_LIGHTS	m_EffectLights;
};
END