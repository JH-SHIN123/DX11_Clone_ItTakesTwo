#include "stdafx.h"
#include "..\Public\Light_Generator.h"
#include "EffectLight.h"

IMPLEMENT_SINGLETON(CLight_Generator)

HRESULT CLight_Generator::Add_Light(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, EPoint_Color eEffectColor, _bool isActive)
{
	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	if (iter != m_EffectLights.end()) return E_FAIL;
	m_EffectLights.emplace(pLightTag, CEffectLight::Create(LightDesc, eEffectColor, isActive));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return FAILED(pGameInstance->Add_Light(pLightTag, LightDesc, isActive));
}

void CLight_Generator::Clear_Lights()
{
	for (auto& Pair : m_EffectLights)
		Safe_Release(Pair.second);
	m_EffectLights.clear();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Clear_Lights();
}

void CLight_Generator::Free()
{
	for (auto& Pair : m_EffectLights)
		Safe_Release(Pair.second);
	m_EffectLights.clear();
}
