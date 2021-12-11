#include "stdafx.h"
#include "..\Public\LaserTennis_Manager.h"

IMPLEMENT_SINGLETON(CLaserTennis_Manager)

CLaserTennis_Manager::CLaserTennis_Manager()
{
}

HRESULT CLaserTennis_Manager::NativeConstruct_LaserTennis()
{
	m_LaserActivation.reserve(7);
	m_LaserButton.reserve(24);
	m_LaserButtonLarge_Gate.reserve(8);

	ZeroMemory(m_pLaserPowerCoord, sizeof(m_pLaserPowerCoord));
	ZeroMemory(m_pLaserButtonLarge, sizeof(m_pLaserButtonLarge));

	return S_OK;
}

void CLaserTennis_Manager::Create_LaserTrigger(_fvector vPosition, TARGET eTarget)
{
	/* LaserTrigger 생성 */
}

void CLaserTennis_Manager::Create_LaserTrigger_LargeButton(_fvector vPosition, TARGET eTarget)
{
	/* 큰버튼 눌렀을 때 LaserTrigger 생성 */
}

void CLaserTennis_Manager::Check_PowerCoord()
{
	++m_iPowerCoordCount;

	if (2 == m_iPowerCoordCount)
		Start_Game();
}

void CLaserTennis_Manager::Start_Game()
{
	m_pTimer_LaserTennis->OnOff_Timer(true);
}

HRESULT CLaserTennis_Manager::Add_LaserActivation(CLaserActivation * pLaserActivation)
{
	if (nullptr == pLaserActivation || 7 <= m_LaserActivation.size())
		return E_FAIL;

	m_LaserActivation.emplace_back(pLaserActivation);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButton(CLaserButton * pLaserActivation)
{
	if (nullptr == pLaserActivation || 24 <= m_LaserButton.size())
		return E_FAIL;

	m_LaserButton.emplace_back(pLaserActivation);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserPowerCoord(_uint iIndex, CLaserPowerCoord * pLaserPowerCoord)
{
	if (nullptr == pLaserPowerCoord || 2 <= iIndex)
		return E_FAIL;

	m_pLaserPowerCoord[iIndex] = pLaserPowerCoord;

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButtonLarge(_uint iIndex, CLaserButtonLarge * pLaserButtonLarge)
{
	if (nullptr == pLaserButtonLarge || 2 <= iIndex)
		return E_FAIL;

	m_pLaserButtonLarge[iIndex] = pLaserButtonLarge;

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButtonLarge_Gate(CLaserButtonLarge_Gate * pLaserButtonLarge_Gate)
{
	if (nullptr == pLaserButtonLarge_Gate || 8 <= m_LaserButtonLarge_Gate.size())
		return E_FAIL;

	m_LaserButtonLarge_Gate.emplace_back(pLaserButtonLarge_Gate);

	return S_OK;
}

void CLaserTennis_Manager::Free()
{
}
