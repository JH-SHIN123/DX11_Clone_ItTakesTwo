#include "stdafx.h"
#include "EffectLight.h"

HRESULT CEffectLight::NativeConstruct(const LIGHT_DESC& LightDesc, _bool isActive)
{
	m_LightDesc = LightDesc;
	m_isActive = isActive;

	// Generate Effect

	return S_OK;
}

CEffectLight* CEffectLight::Create(const LIGHT_DESC& LightDesc, _bool isActive)
{
	CEffectLight* pInstance = new CEffectLight();

	if (FAILED(pInstance->NativeConstruct(LightDesc, isActive)))
	{
		MSG_BOX("Failed to Create Instance - CEffectLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectLight::Free()
{
}
