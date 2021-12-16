#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CLight : public CBase
{
protected:
	explicit CLight() = default;
	virtual ~CLight() = default;

public:
	void Set_Dead(_bool bDead) { m_bDead = bDead; }

public: /* Getter */
	const _tchar*	Get_LightTag() { return m_szLightTag; };
	LIGHT_DESC*		Get_LightDescPtr() { return &m_LightDesc; }

public:
	virtual HRESULT	NativeConstruct(const _tchar* pLightTag, void* pArgs);
	virtual _int	Tick_Light(_double dTimeDelta);
	
public:
	HRESULT Render_Light(class CVIBuffer_RectRHW* pVIBuffer);

protected:
	_bool		m_bDead = false;

protected:
	_tchar		m_szLightTag[MAX_PATH] = L"";
	LIGHT_DESC	m_LightDesc;

public:
	static CLight* Create(const _tchar* pLightTag, void* pArgs);
	virtual void Free() override;
};
END