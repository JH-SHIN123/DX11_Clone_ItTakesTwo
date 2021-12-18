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
	//EFFECT->Add_PointLight(CEff)

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
	//if (m_pVolumeObject)
	//{
	//	m_pVolumeObject->Set_Dead();
	//	Safe_Release(m_pVolumeObject);
	//}

	CLight::Free();
}
