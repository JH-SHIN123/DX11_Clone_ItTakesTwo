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

	return S_OK;
}

_int CVolumeLight::Tick_Light(_double dTimeDelta)
{
	if (false == lstrcmp(m_szLightTag, L"Static_Volume_Test"))
	{
		_vector vPos = XMVectorZero();
		TCHAR szBuff[256] = L"";
		GetPrivateProfileString(L"Section_1", L"Key_1", L"0", szBuff, 256, L"../light.ini");
		_float posX = _wtof(szBuff);
		GetPrivateProfileString(L"Section_1", L"Key_2", L"0", szBuff, 256, L"../light.ini");
		_float posY = _wtof(szBuff);
		GetPrivateProfileString(L"Section_1", L"Key_3", L"0", szBuff, 256, L"../light.ini");
		_float posZ = _wtof(szBuff);
		GetPrivateProfileString(L"Section_2", L"Key_1", L"0", szBuff, 256, L"../light.ini");
		_float range = _wtof(szBuff);

		GetPrivateProfileString(L"Section_3", L"Key_1", L"0", szBuff, 256, L"../light.ini");
		_float difR = _wtof(szBuff);
		GetPrivateProfileString(L"Section_3", L"Key_2", L"0", szBuff, 256, L"../light.ini");
		_float difG = _wtof(szBuff);
		GetPrivateProfileString(L"Section_3", L"Key_3", L"0", szBuff, 256, L"../light.ini");
		_float difB = _wtof(szBuff);
		GetPrivateProfileString(L"Section_3", L"Key_4", L"0", szBuff, 256, L"../light.ini");
		_float specR = _wtof(szBuff);
		GetPrivateProfileString(L"Section_3", L"Key_5", L"0", szBuff, 256, L"../light.ini");
		_float specG = _wtof(szBuff);
		GetPrivateProfileString(L"Section_3", L"Key_6", L"0", szBuff, 256, L"../light.ini");
		_float specB = _wtof(szBuff);

		GetPrivateProfileString(L"Section_4", L"Key_1", L"0", szBuff, 256, L"../light.ini");
		_float volScaleX = _wtof(szBuff);
		GetPrivateProfileString(L"Section_4", L"Key_2", L"0", szBuff, 256, L"../light.ini");
		_float volScaleY = _wtof(szBuff);
		GetPrivateProfileString(L"Section_4", L"Key_3", L"0", szBuff, 256, L"../light.ini");
		_float volScaleZ = _wtof(szBuff);

		GetPrivateProfileString(L"Section_5", L"Key_1", L"0", szBuff, 256, L"../light.ini");
		_float volInnR = _wtof(szBuff);
		GetPrivateProfileString(L"Section_5", L"Key_2", L"0", szBuff, 256, L"../light.ini");
		_float volInnG = _wtof(szBuff);
		GetPrivateProfileString(L"Section_5", L"Key_3", L"0", szBuff, 256, L"../light.ini");
		_float volInnB = _wtof(szBuff);
		GetPrivateProfileString(L"Section_5", L"Key_4", L"0", szBuff, 256, L"../light.ini");
		_float volOtR = _wtof(szBuff);
		GetPrivateProfileString(L"Section_5", L"Key_5", L"0", szBuff, 256, L"../light.ini");
		_float volOtG = _wtof(szBuff);
		GetPrivateProfileString(L"Section_5", L"Key_6", L"0", szBuff, 256, L"../light.ini");
		_float volOtB = _wtof(szBuff);

		m_LightDesc.vPosition = { posX, posY, posZ };
		m_LightDesc.fRange = range;
		m_LightDesc.vDiffuse = { difR, difG, difB, 1.f };
		m_LightDesc.vSpecular = { specR, specG, specB, 1.f };

		_matrix WorldMat = XMMatrixScaling(volScaleX, volScaleY, volScaleZ);
		WorldMat *= XMMatrixTranslation(posX, posY, posZ);
		if (m_pVolumeObject) {
			m_pVolumeObject->Set_WorldMatrix(WorldMat);
			_float3 vInn = { volInnR, volInnG, volInnB };
			_float3 vOut = { volOtR, volOtG, volOtB };
			m_pVolumeObject->Set_Color(vInn, vOut);
		}
	}

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
		Safe_Release(m_pVolumeObject);
	}

	CLight::Free();
}
