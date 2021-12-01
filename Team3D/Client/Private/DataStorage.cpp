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

void CDataStorage::Free()
{
	m_pCody = nullptr;
	m_pMay = nullptr;
	m_pMainCam = nullptr;
	m_pSubCam = nullptr;

	// UFO
	m_pUFO = nullptr;

	// Interactive Objects
	m_pRobot = nullptr;
	m_pRobotLever = nullptr;
	m_pRobotBattery = nullptr;
	m_pRobotHead = nullptr;
	m_pNoBatterySign = nullptr;

	m_SpaceRails.clear();
}
