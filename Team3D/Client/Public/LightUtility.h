#pragma once

#include "Client_Defines.h"

class CLightUtility
{
public:
	enum LOAD_LIGHTTYPE { LOAD_BASICLIGHT, LOAD_VOLUMELIGHT, LOAD_EFFECTLIGHT };

public:
	static HRESULT Load_StaticLightData(LOAD_LIGHTTYPE eLoadType, const _tchar* pFilePath);
};

