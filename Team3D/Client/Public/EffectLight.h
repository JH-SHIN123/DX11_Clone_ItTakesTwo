#pragma once

#include "Client_Defines.h"
#include "Light.h"
#include "VolumeObject.h"

BEGIN(Client)
class CEffectLight final : public CLight
{
public:
	typedef struct tagEffectLightDesc
	{
		LIGHT_DESC		tLightDesc;
		_float			fSize;
		_uint			iEffectColor;
	}EFFECTLIGHT_DESC;

private:
	explicit CEffectLight() = default;
	virtual ~CEffectLight() = default;

public:
	virtual HRESULT	NativeConstruct(const _tchar * pLightTag, void* pArgs) override;
	virtual _int	Tick_Light(_double dTimeDelta) override;

private:
	EFFECTLIGHT_DESC m_tEffectDesc;

public:
	static CEffectLight* Create(const _tchar * pLightTag, void* pArgs);
	virtual void Free() override;
};
END