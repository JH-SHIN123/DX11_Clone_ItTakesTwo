#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CEffectLight final : public CBase
{
private:
	explicit CEffectLight() = default;
	virtual ~CEffectLight() = default;

public:
	void	Set_Active(_bool bActive) { m_bActive = bActive; }
	HRESULT Set_Light(const LIGHT_DESC& LightDesc, _uint eEffectColor);

public:
	HRESULT	NativeConstruct(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _uint eEffectColor, _bool bActive);

private:
	_bool m_bActive = false;

private:
	_tchar							m_szLightTag[MAX_PATH] = L"";
	LIGHT_DESC						m_LightDesc;
	class CEffect_PointLight*		m_pEffect = nullptr;
	class CLight*					m_pLight = nullptr;

public:
	static CEffectLight* Create(const _tchar* pLightTag, const LIGHT_DESC & LightDesc, _uint eEffectColor, _bool bActive);
	virtual void Free() override;
};
END