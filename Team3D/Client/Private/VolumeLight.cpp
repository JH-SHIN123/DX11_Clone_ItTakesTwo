#include "stdafx.h"
#include "VolumeLight.h"

HRESULT CVolumeLight::NativeConstruct(const _tchar* pLightTag, void* pArgs)
{
	NULL_CHECK_RETURN(pLightTag, E_FAIL);

	if (nullptr != pArgs)
		memcpy(&m_tVolumeDesc, pArgs, sizeof(m_tVolumeDesc));

	/* Set Light */
	CLight::NativeConstruct(pLightTag, &m_tVolumeDesc.tLightDesc);

	/* Set Volume */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Volume"), Level::LEVEL_STAGE, TEXT("GameObject_Volume"), &m_tVolumeDesc.tVolumeDesc, (CGameObject**)&m_pVolumeObject), E_FAIL);
	Safe_Release(m_pVolumeObject);

	return S_OK;
}

_int CVolumeLight::Tick_Light(_double dTimeDelta)
{
	return CLight::Tick_Light(dTimeDelta);
}

CVolumeLight* CVolumeLight::Create(const _tchar* pLightTag, void* pArgs)
{
	CVolumeLight* pInstance = new CVolumeLight;

	if (FAILED(pInstance->NativeConstruct(pLightTag, pArgs)))
	{
		MSG_BOX("Failed to Create Instance - CVolumeLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVolumeLight::Free()
{
	if (m_pVolumeObject)
	{
		m_pVolumeObject->Set_Dead();
		m_pVolumeObject = nullptr;
	}

	CLight::Free();
}
