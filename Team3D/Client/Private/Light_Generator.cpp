#include "stdafx.h"
#include "..\Public\Light_Generator.h"
#include "EffectLight.h"
#include "Effect_Generator.h"

IMPLEMENT_SINGLETON(CLight_Generator)

HRESULT CLight_Generator::Add_Light(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _uint eEffectColor)
{
	// Create Effect
	FAILED_CHECK_RETURN(EFFECT->Add_PointLight(&CEffect_Generator::Effect_PointLight_Desc(20.f, 0.25f, 1.f, LightDesc.vPosition, (EPoint_Color)eEffectColor)), E_FAIL);

	// Create Light
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	FAILED_CHECK_RETURN(pGameInstance->Add_Light(pLightTag, LightDesc), E_FAIL);

	 return S_OK;
}

void CLight_Generator::Free()
{
}

