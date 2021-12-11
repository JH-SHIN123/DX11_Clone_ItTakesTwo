#include "stdafx.h"
#include "EffectLight.h"
#include "Effect_Generator.h"
#include "Effect_PointLight.h"
#include "Light.h"

HRESULT CEffectLight::Set_Light(const LIGHT_DESC& LightDesc, _float fEffectRadius, _uint eEffectColor)
{
	NULL_CHECK_RETURN(m_pLight, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	// Set Light
	LIGHT_DESC* pLightDesc = m_pLight->Get_LightDescPtr();
	NULL_CHECK_RETURN(pLightDesc, E_FAIL);
	memcpy(pLightDesc, &LightDesc, sizeof(LIGHT_DESC));

	// Set Effect
	m_pEffect->Set_Pos(LightDesc.vPosition);
	m_pEffect->Set_Radius(fEffectRadius);
	m_pEffect->Set_Color((EPoint_Color)eEffectColor);

	return S_OK;
}

HRESULT CEffectLight::NativeConstruct(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _float fEffectRadius, _uint eEffectColor, _bool bEffectActive)
{
	m_bEffectActive = bEffectActive;
	lstrcpy(m_szLightTag, pLightTag);
	m_LightDesc = LightDesc;

	// Set Light
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Add_Light(m_szLightTag, LightDesc, true, (void**)&m_pLight);

	// Set Effect
	if (0.f != fEffectRadius || false == m_bEffectActive)
	{
		FAILED_CHECK_RETURN(EFFECT->Add_PointLight(&CEffect_Generator::Effect_PointLight_Desc(fEffectRadius, 0.25f, 1.f, LightDesc.vPosition, (EPoint_Color)eEffectColor), (CGameObject**)&m_pEffect), E_FAIL);
		m_pEffect->Set_Radius(fEffectRadius);
	}

	return S_OK;
}

CEffectLight* CEffectLight::Create(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _float fEffectRadius, _uint eEffectColor, _bool bEffectActive)
{
	CEffectLight* pInstance = new CEffectLight();

	if (FAILED(pInstance->NativeConstruct(pLightTag, LightDesc, fEffectRadius, eEffectColor, bEffectActive)))
	{
		MSG_BOX("Failed to Create Instance - CEffectLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectLight::Free()
{
	if(m_pEffect) m_pEffect->Set_Dead();
	Safe_Release(m_pEffect);

	/* 원본 자체는 안사라짐 */
	Safe_Release(m_pLight);
	CGameInstance::GetInstance()->Remove_Light(m_szLightTag);
}
