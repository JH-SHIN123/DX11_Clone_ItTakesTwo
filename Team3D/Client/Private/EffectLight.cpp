#include "stdafx.h"
#include "EffectLight.h"
#include "Effect_PointLight.h"

HRESULT CEffectLight::NativeConstruct(const _tchar* pLightTag, void* pArgs)
{
	NULL_CHECK_RETURN(pLightTag, E_FAIL);

	if (nullptr != pArgs)
		memcpy(&m_tEffectDesc, pArgs, sizeof(m_tEffectDesc));

	/* Set Light */
	CLight::NativeConstruct(pLightTag, &m_tEffectDesc.tLightDesc);

	/* Set Effect */
	FAILED_CHECK_RETURN(EFFECT->Add_PointLight(&CEffect_Generator::Effect_PointLight_Desc(m_tEffectDesc.fSize, 0.25f, 1.f, m_LightDesc.vPosition, (EPoint_Color)m_tEffectDesc.iEffectColor), (CGameObject**)&m_pEffect), E_FAIL);
	m_pEffect->Set_Radius(m_tEffectDesc.fSize);

	return S_OK;
}

_int CEffectLight::Tick_Light(_double dTimeDelta)
{
	return CLight::Tick_Light(dTimeDelta);
}

CEffectLight* CEffectLight::Create(const _tchar* pLightTag, void* pArgs)
{
	CEffectLight* pInstance = new CEffectLight;

	if (FAILED(pInstance->NativeConstruct(pLightTag, pArgs)))
	{
		MSG_BOX("Failed to Create Instance - CEffectLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectLight::Free()
{
	if (m_pEffect)
	{
		m_pEffect->Set_Dead();
		Safe_Release(m_pEffect);
	}

	CLight::Free();
}
