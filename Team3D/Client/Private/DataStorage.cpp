 #include "stdafx.h"
#include "..\public\DataStorage.h"
#include "Cody.h"
#include "May.h"
#include "MainCamera.h"
#include "SubCamera.h"

IMPLEMENT_SINGLETON(CDataStorage)

void CDataStorage::Set_SpaceRails(const _tchar* pRailTag, CGameObject* pRail)
{
	auto& iter = find_if(m_SpaceRails.begin(), m_SpaceRails.end(), CTagFinder(pRailTag));
	if (iter != m_SpaceRails.end()) return;

	m_SpaceRails.emplace(pRailTag, pRail);
}

CGameObject* CDataStorage::Get_SpaceRail(const _tchar* pRailTag)
{
	auto& iter = find_if(m_SpaceRails.begin(), m_SpaceRails.end(), CTagFinder(pRailTag));
	if (iter == m_SpaceRails.end()) return nullptr;

	return iter->second;
}

_uint CDataStorage::Get_ValveCount_Cody(_bool IsCody)
{
	if (true == IsCody)
		return m_iCody_Valve_Count;
	else
		return m_iMay_Valve_Count;
}

void CDataStorage::Free()
{
	m_pCody = nullptr;
	m_pMay = nullptr;
	m_pMainCam = nullptr;
	m_pSubCam = nullptr;

	// UFO
	m_pUFO = nullptr;

	m_SpaceRails.clear();
}
