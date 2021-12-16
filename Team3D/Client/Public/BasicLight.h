#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CBasicLight : public CBase
{
private:
	explicit CBasicLight() = default;
	virtual ~CBasicLight() = default;

public:
	virtual HRESULT	NativeConstruct(const _tchar * pLightTag);

private:
	_bool	m_bActive = false;
	_tchar	m_szLightTag[MAX_PATH] = L"";

private:
	class CLight*	m_pLight = nullptr;

public:
	static CBasicLight* Create(const _tchar * pLightTag);
	virtual void Free() override;
};
END