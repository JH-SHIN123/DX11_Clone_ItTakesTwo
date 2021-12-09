#pragma once

#include "Client_Defines.h"
#include "Base.h"

#define __INSTALL_LIGHT

BEGIN(Client)
class CLight_Generator final : public CBase
{
	DECLARE_SINGLETON(CLight_Generator)

public:
	CLight_Generator() = default;
	virtual ~CLight_Generator() = default;
	
public:
	HRESULT	Add_Light(const _tchar* pLightTag, class CEffectLight* pEffectLight);
	HRESULT Clear_Light();

#ifdef __INSTALL_LIGHT
public: /* For. 조명설치 */
	HRESULT Set_Light(const _tchar* pLightTag, LIGHT_DESC& LightDesc, _uint eEffectColor);
	HRESULT Delete_Light(const _tchar* pLightTag);
	HRESULT Save_Light();
	HRESULT Load_Light();
#endif

private:
	typedef unordered_map<const _tchar*, class CEffectLight*> EFFECTLIGHTS;

private:
	EFFECTLIGHTS m_EffectLights;

public:
	virtual void Free() override;
};
END