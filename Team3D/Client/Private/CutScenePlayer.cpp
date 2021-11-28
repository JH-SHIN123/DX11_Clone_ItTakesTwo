#include "stdafx.h"
#include "..\Public\CutScenePlayer.h"
#include"DataStorage.h"
#include"Performer.h"
#include"GameInstance.h"
IMPLEMENT_SINGLETON(CCutScenePlayer)

CCutScenePlayer::CCutScenePlayer()
{
}

HRESULT CCutScenePlayer::NativeConstruct()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	const _tchar* pLayerTag = L"Layer_Performer";
	CGameObject* pPerformer = nullptr;
	CPerformer::PERFORMERDESC tDesc;
	tDesc.strModelTag = L"Component_Model_Cody";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_May";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_ToyBox1";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_RemoteContollerCutScene1";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_GravityBootsCutScene1";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_SizeBeltCutScene1";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = L"Component_Model_SizeBeltRemoteControllerCutScene1";
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	return S_OK;
}

HRESULT CCutScenePlayer::Add_CutScene(const _tchar * pCutSceneTag, CCutScene * pCutScene)
{
	if (pCutSceneTag == nullptr ||
		pCutScene == nullptr ||
		Find_CutScene(pCutSceneTag) != nullptr)
		return E_FAIL;
	m_CutScenes.emplace(CUTSCENES::value_type(pCutSceneTag, pCutScene));
	return S_OK;
}
HRESULT CCutScenePlayer::Add_Performer(const _tchar * pPerformerTag, CGameObject * pPerformer)
{
	if (pPerformer == nullptr ||
		pPerformerTag == nullptr ||
		Find_Performer(pPerformerTag) != nullptr)
		return E_FAIL;

	m_Performers.emplace(PERFORMERS::value_type(pPerformerTag, pPerformer));
	Safe_AddRef(pPerformer);
	return S_OK;
}
_bool CCutScenePlayer::Tick_CutScene(_double dTimeDelta)
{
	if (nullptr == m_pCurCutScene)
		return false;

	m_bIsPlayingCutScene = m_pCurCutScene->Tick_CutScene(dTimeDelta);
	if (false == m_bIsPlayingCutScene)
	{
		Safe_Release(m_pCurCutScene);
		m_pCurCutScene = nullptr;
	}
	return true;
}

HRESULT CCutScenePlayer::Start_CutScene(const _tchar* pCutSceneTag)
{
	CCutScene* pCutScene = Find_CutScene(pCutSceneTag);
	if (nullptr == pCutScene || 
		nullptr != m_pCurCutScene)
		return E_FAIL;
	m_pCurCutScene = pCutScene;
	Safe_AddRef(m_pCurCutScene);
	if (FAILED(m_pCurCutScene->Start_CutScene()))
	{
		Safe_Release(m_pCurCutScene);
		m_pCurCutScene = nullptr;
		return E_FAIL;
	}
	return S_OK;
}



CGameObject * CCutScenePlayer::Find_Performer(const _tchar * pPerformerTag)
{
	auto& iter = find_if(m_Performers.begin(), m_Performers.end(), CTagFinder(pPerformerTag));
	if (iter == m_Performers.end())
		return nullptr;

	return iter->second;
}
CCutScene * CCutScenePlayer::Find_CutScene(const _tchar * pCutSceneTag)
{
	auto& iter = find_if(m_CutScenes.begin(), m_CutScenes.end(), CTagFinder(pCutSceneTag));
	if (iter == m_CutScenes.end())
		return nullptr;

	return iter->second;
}
void CCutScenePlayer::Free()
{
	for (auto& rPair : m_Performers)
		Safe_Release(rPair.second);
	m_Performers.clear();
	for (auto& rPair : m_CutScenes)
		Safe_Release(rPair.second);
	m_CutScenes.clear();
	Safe_Release(m_pCurCutScene);

	Safe_Release(m_pGameInstance);
}
