#include "..\public\Level_Manager.h"
#include "Level.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

const _uint CLevel_Manager::Get_CurrentLevelStep()
{
	return m_pCurrentLevel->Get_LevelStep();
}

HRESULT CLevel_Manager::Change_CurrentLevel(CLevel * pCurrentLevel)
{
	NULL_CHECK_RETURN(pCurrentLevel, E_FAIL);

	Safe_Release(m_pCurrentLevel);
	m_pCurrentLevel = pCurrentLevel;

	return S_OK;
}

_int CLevel_Manager::Tick(_double dTimedelta)
{
	NULL_CHECK_RETURN(m_pCurrentLevel, EVENT_ERROR);
	return m_pCurrentLevel->Tick(dTimedelta);
}

HRESULT CLevel_Manager::Render()
{
	NULL_CHECK_RETURN(m_pCurrentLevel, S_OK);
	return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Clear_Level()
{
	Safe_Release(m_pCurrentLevel);
	return S_OK;
}

void CLevel_Manager::Go_NextLevelStep()
{
	m_pCurrentLevel->Go_NextLevelStep(); 
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}