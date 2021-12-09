#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
private:
	explicit CLight() = default;
	virtual ~CLight() = default;

public: /* Getter */
	LIGHT_DESC* Get_LightDescPtr() { return &m_LightDesc; }

public: /* Setter */
	void TurnOn_Light() { m_isActive = true; }
	void TurnOff_Light() { m_isActive = false; }

public:
	HRESULT	NativeConstruct(const LIGHT_DESC& LightDesc, _bool isActive);
	HRESULT Render_Light(class CVIBuffer_RectRHW* pVIBuffer);

private:
	LIGHT_DESC	m_LightDesc;
	_bool		m_isActive = false;

public:
	static CLight* Create(const LIGHT_DESC& LightDesc, _bool isActive);
	virtual void Free() override;
};

END