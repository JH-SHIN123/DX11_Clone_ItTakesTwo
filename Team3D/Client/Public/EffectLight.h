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
	HRESULT	NativeConstruct(const LIGHT_DESC & LightDesc, _uint eEffectColor, _bool isActive);

private:
	_bool		m_isActive = false;

private:
	LIGHT_DESC	m_LightDesc;
	// Effect

public:
	static CEffectLight* Create(const LIGHT_DESC & LightDesc, _uint eEffectColor, _bool isActive); // 이펙트 enum값 던지도록 설정
	virtual void Free() override;
};
END