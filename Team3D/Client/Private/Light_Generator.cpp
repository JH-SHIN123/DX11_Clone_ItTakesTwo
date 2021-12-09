#include "stdafx.h"
#include "..\Public\Light_Generator.h"
#include "EffectLight.h"
#include "Effect_Generator.h"

IMPLEMENT_SINGLETON(CLight_Generator)

HRESULT CLight_Generator::Add_Light(const _tchar* pLightTag, CEffectLight* pEffectLight)
{
	if (nullptr == pLightTag || nullptr == pEffectLight) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	
	if (iter != m_EffectLights.end())
	{
		MSG_BOX("Already Exist Effect Light");
		return E_FAIL;
	}

	m_EffectLights.emplace(pLightTag, pEffectLight);

	return S_OK;
}

HRESULT CLight_Generator::Clear_Light()
{
	for (auto& Pair : m_EffectLights)
	{
		Safe_Release(Pair.second);
	}
	m_EffectLights.clear();

	return S_OK;
}

#ifdef __INSTALL_LIGHT
HRESULT CLight_Generator::Set_Light(const _tchar* pLightTag, LIGHT_DESC& LightDesc, _uint eEffectColor)
{
	if (nullptr == pLightTag) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	if (iter == m_EffectLights.end())
	{
		MSG_BOX("Already Exist Effect Light");
		return E_FAIL;
	}

	return iter->second->Set_Light(LightDesc, eEffectColor);
}

HRESULT CLight_Generator::Delete_Light(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	if (iter == m_EffectLights.end())
	{
		MSG_BOX("Already Exist Effect Light");
		return E_FAIL;
	}
	
	Safe_Release(iter->second);
	m_EffectLights.erase(pLightTag);

	return S_OK;
}

HRESULT CLight_Generator::Save_Light()
{
	return S_OK;
}

HRESULT CLight_Generator::Load_Light()
{
	return S_OK;
}
#endif

void CLight_Generator::Free()
{
	Clear_Light();
}

