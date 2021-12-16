#pragma once

#include "Client_Defines.h"
#include "Base.h"

#define __INSTALL_LIGHT

BEGIN(Client)
class CLight_Generator final : public CBase
{
	DECLARE_SINGLETON(CLight_Generator)

public:
	enum LIGHT_TYPE { TYPE_BASIC, TYPE_EFFECT, TYPE_VOLUME, TYPE_END };

public:
	CLight_Generator() = default;
	virtual ~CLight_Generator() = default;
	
public:
	HRESULT	Add_Light(const _tchar* pLightTag, class CEffectLight* pEffectLight);
	HRESULT Clear_Light();

#ifdef __INSTALL_LIGHT
public: /* For. 조명설치 */
	HRESULT KeyInput(_double dTimeDelta);
	HRESULT Set_Light(const _tchar* pLightTag, LIGHT_DESC& LightDesc, _float fEffectRadius, _uint eEffectColor);
	HRESULT Delete_Light(const _tchar* pLightTag);
#endif

private:
	typedef vector<class CBasicLight*>	STATIC_LIGHTS;
	typedef list<class CBasicLight*>	DYNAMIC_LIGHTS;

private:
	STATIC_LIGHTS	m_StaticLights;
	DYNAMIC_LIGHTS	m_DynamicLights;

public:
	virtual void Free() override;
};
END