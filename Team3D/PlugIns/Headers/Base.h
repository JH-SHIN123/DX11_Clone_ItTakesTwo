#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	explicit CBase() = default;
	virtual ~CBase() = default;

public:
	unsigned long AddRef();
	unsigned long Release();

public:
	virtual void Free() PURE;

private:
	unsigned long m_dwRefCnt = 0;
};

END