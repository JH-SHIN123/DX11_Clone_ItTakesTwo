#include "stdafx.h"
#include "EffectLight.h"
#include "Effect_Generator.h"

HRESULT CEffectLight::NativeConstruct(const LIGHT_DESC& LightDesc, _uint eEffectColor, _bool isActive)
{
	m_LightDesc = LightDesc;
	m_isActive = isActive;

	// Generate Effect
	FAILED_CHECK_RETURN(EFFECT->Add_PointLight(&CEffect_Generator::Effect_PointLight_Desc(20.f, 0.25f, 1.f, LightDesc.vPosition, (EPoint_Color)eEffectColor)), E_FAIL);

	return S_OK;
}

CEffectLight* CEffectLight::Create(const LIGHT_DESC& LightDesc, _uint eEffectColor, _bool isActive)
{
	CEffectLight* pInstance = new CEffectLight();

	if (FAILED(pInstance->NativeConstruct(LightDesc, eEffectColor, isActive)))
	{
		MSG_BOX("Failed to Create Instance - CEffectLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectLight::Free()
{
}
