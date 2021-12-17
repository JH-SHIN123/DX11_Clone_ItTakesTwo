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
	if (TYPE_STATIC == m_tVolumeDesc.eType)
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StaticVolume"), Level::LEVEL_STAGE, TEXT("GameObject_StaticVolume"), &m_tVolumeDesc.tVolumeDesc_Static, (CGameObject**)&m_pStaticVolume), E_FAIL);
	}
	else if (TYPE_DYNAMIC == m_tVolumeDesc.eType)
	{
		FAILED_CHECK_RETURN(pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_DynamicVolume"), Level::LEVEL_STAGE, TEXT("GameObject_DynamicVolume"), &m_tVolumeDesc.tVolumeDesc_Dynamic, (CGameObject**)&m_pDynamicVolume), E_FAIL);
	}

	return S_OK;
}

_int CVolumeLight::Tick_Light(_double dTimeDelta)
{
	//if(TYPE_DYNAMIC == m_tVolumeDesc.eType)
	
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
	Safe_Release(m_pStaticVolume);
	Safe_Release(m_pDynamicVolume);

	CLight::Free();
}
