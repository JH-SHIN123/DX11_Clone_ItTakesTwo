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

private:
	typedef unordered_map<const _tchar*, class CLight*> STATIC_CLONE_LIGHTS;
	typedef list<class CLight*>							DYNAMIC_CLONE_LIGHTS;

private:
	STATIC_CLONE_LIGHTS		m_StaticLights;
	DYNAMIC_CLONE_LIGHTS	m_DynamicLights;

public:
	virtual void Free() override;
};
END