#include "stdafx.h"
#include "..\Public\BasicLight.h"
#include "Light.h"

HRESULT CBasicLight::NativeConstruct(const _tchar* pLightTag)
{
	/* Set Light */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Add_Light(m_szLightTag, LightDesc, true, (void**)&m_pLight);

	return S_OK;
}

CBasicLight* CBasicLight::Create(const _tchar* pLightTag)
{
	CBasicLight* pInstance = new CBasicLight();

	if (FAILED(pInstance->NativeConstruct(pLightTag)))
	{
		MSG_BOX("Failed to Create Instance - CBasicLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBasicLight::Free()
{
	Safe_Release(m_pLight);
}
