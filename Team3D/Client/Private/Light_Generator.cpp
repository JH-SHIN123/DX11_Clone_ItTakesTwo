#include "stdafx.h"
#include "..\Public\Light_Generator.h"

IMPLEMENT_SINGLETON(CLight_Generator)

HRESULT CLight_Generator::Add_Light(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _bool isActive)
{



	return S_OK;
}

void CLight_Generator::Free()
{
}
