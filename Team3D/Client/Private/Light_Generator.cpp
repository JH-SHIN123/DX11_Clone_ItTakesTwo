#include "stdafx.h"
#include "..\Public\Light_Generator.h"
#include "EffectLight.h"
#include "Effect_Generator.h"

IMPLEMENT_SINGLETON(CLight_Generator)

HRESULT CLight_Generator::Add_Light(const _tchar* pLightTag, CEffectLight* pEffectLight)
{
	if (nullptr == pLightTag || nullptr == pEffectLight) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	
#ifndef __INSTALL_LIGHT
	if (iter != m_EffectLights.end())
	{
		MSG_BOX("Already Exist Effect Light");
		return E_FAIL;
	}
#endif

#ifdef __INSTALL_LIGHT
	_tchar* pTempTag = new _tchar[MAX_PATH];
	lstrcpy(pTempTag, pLightTag);
	m_EffectLights.emplace(pTempTag, pEffectLight);
#else
	m_EffectLights.emplace(pLightTag, pEffectLight);
#endif // __INSTALL_LIGHT

	return S_OK;
}

HRESULT CLight_Generator::Clear_Light()
{
	for (auto& Pair : m_EffectLights)
	{
#ifdef __INSTALL_LIGHT
		const _tchar* pTag = Pair.first;
		SAFE_DELETE_ARRAY(pTag);
#endif // __INSTALL_LIGHT
		Safe_Release(Pair.second);
	}

	m_EffectLights.clear();

	return S_OK;
}

#ifdef __INSTALL_LIGHT
HRESULT CLight_Generator::KeyInput(_double dTimeDelta)
{
	/* Section1 - LightTag */
	TCHAR szBuff[256] = L"";
	GetPrivateProfileString(L"Section_1", L"Key_1", L"0", szBuff, 256, L"../light.ini");
	TCHAR lightTag[256] = L"";
	lstrcpy(lightTag, szBuff);

	/* Section2 - Pos */
	GetPrivateProfileString(L"Section_2", L"Key_1", L"0", szBuff, 256, L"../light.ini");
	_float fPosX = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_2", L"Key_2", L"0", szBuff, 256, L"../light.ini");
	_float fPosY = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_2", L"Key_3", L"0", szBuff, 256, L"../light.ini");
	_float fPosZ = (_float)_wtof(szBuff);

	/* Section3 - Range */
	GetPrivateProfileString(L"Section_3", L"Key_1", L"0", szBuff, 256, L"../light.ini");
	_float fRange = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_3", L"Key_2", L"0", szBuff, 256, L"../light.ini");
	_float fEffectRadius = (_float)_wtof(szBuff);

	/* Section4 - EffectColor */
	GetPrivateProfileString(L"Section_4", L"Key_1", L"0", szBuff, 256, L"../light.ini");
	_int iEffectColor = (_int)_wtoi(szBuff);

	/* Section5 - Diffuse  */
	GetPrivateProfileString(L"Section_5", L"Key_1", L"0", szBuff, 256, L"../light.ini");
	_float fDiffuseX = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_5", L"Key_2", L"0", szBuff, 256, L"../light.ini");
	_float fDiffuseY = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_5", L"Key_3", L"0", szBuff, 256, L"../light.ini");
	_float fDiffuseZ = (_float)_wtof(szBuff);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Key_Down(DIK_F1)) // 조명설치
	{
		LIGHT_DESC lightDesc;
		lightDesc.eType = LIGHT_DESC::TYPE_POINT;
		lightDesc.vPosition = XMFLOAT3(fPosX, fPosY, fPosZ);
		lightDesc.vDiffuse = XMFLOAT4(fDiffuseX, fDiffuseY, fDiffuseZ, 1.f);
		lightDesc.vAmbient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		lightDesc.vSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		lightDesc.fRange = fRange;
		Add_Light(lightTag, CEffectLight::Create(lightTag, lightDesc, fEffectRadius, iEffectColor, true));
	}

	if (pGameInstance->Key_Down(DIK_F2)) // 조명배치
	{
		LIGHT_DESC lightDesc;
		lightDesc.eType = LIGHT_DESC::TYPE_POINT;
		lightDesc.vPosition = XMFLOAT3(fPosX, fPosY, fPosZ);
		lightDesc.vDiffuse = XMFLOAT4(fDiffuseX, fDiffuseY, fDiffuseZ, 1.f);
		lightDesc.vAmbient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		lightDesc.vSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		lightDesc.fRange = fRange;
		Set_Light(lightTag, lightDesc, fEffectRadius, iEffectColor);
	}

	if (pGameInstance->Key_Down(DIK_F3)) // 조명삭제
	{
		Delete_Light(lightTag);
	}

	return S_OK;
}

HRESULT CLight_Generator::Set_Light(const _tchar* pLightTag, LIGHT_DESC& LightDesc, _float fEffectRadius, _uint eEffectColor)
{
	if (nullptr == pLightTag) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	if (iter == m_EffectLights.end())
	{
		MSG_BOX("Failed to Set_Light");
		return E_FAIL;
	}

	if (nullptr == iter->second) return E_FAIL;

	return iter->second->Set_Light(LightDesc, fEffectRadius, eEffectColor);
}

HRESULT CLight_Generator::Delete_Light(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return E_FAIL;

	auto& iter = find_if(m_EffectLights.begin(), m_EffectLights.end(), CTagFinder(pLightTag));
	if (iter == m_EffectLights.end())
	{
		MSG_BOX("Failed to Delete_Light");
		return E_FAIL;
	}
	
	Safe_Release(iter->second);
	m_EffectLights.erase(pLightTag);

	return S_OK;
}
#endif

void CLight_Generator::Free()
{
	Clear_Light();
}

