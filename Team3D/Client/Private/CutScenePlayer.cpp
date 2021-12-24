#include "stdafx.h"
#include "..\Public\CutScenePlayer.h"
#include"DataStorage.h"
#include"Performer.h"
#include"GameInstance.h"
IMPLEMENT_SINGLETON(CCutScenePlayer)

CCutScenePlayer::CCutScenePlayer()
{
}

HRESULT CCutScenePlayer::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	m_pGameInstance = CGameInstance::GetInstance();

	ID3D11Device* m_pDevice = pDevice;

	ID3D11DeviceContext* m_pDeviceContext = pDeviceContext;

	ZeroMemory(m_bIsPlayedCutScene, sizeof(_bool) * (_uint)CCutScene::CutSceneOption::CutScene_End);

	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Intro"),CCutScene::Create(CCutScene::CutSceneOption::CutScene_Intro)),E_FAIL);
	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Active_GravityPath_01"), CCutScene::Create(CCutScene::CutSceneOption::CutScene_Active_GravityPath_01)), E_FAIL);
	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Clear_Umbrella"), CCutScene::Create(CCutScene::CutSceneOption::CutScene_Clear_Umbrella)), E_FAIL);
	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Clear_Rail"), CCutScene::Create(CCutScene::CutSceneOption::CutScene_Clear_Rail)), E_FAIL);
	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Boss_Intro"), CCutScene::Create(CCutScene::CutSceneOption::CutScene_Boss_Intro)), E_FAIL);
	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Eject_InUFO"), CCutScene::Create(CCutScene::CutSceneOption::CutScene_Eject_InUFO)), E_FAIL);

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
		m_pCurCutScene = nullptr;

	return true;
}



HRESULT CCutScenePlayer::Start_CutScene(const _tchar* pCutSceneTag)
{
	CCutScene* pCutScene = Find_CutScene(pCutSceneTag);
	if (nullptr == pCutScene || 
		nullptr != m_pCurCutScene)
		return E_FAIL;
	m_pCurCutScene = pCutScene;
	
	if (FAILED(m_pCurCutScene->Start_CutScene()))
	{
		m_pCurCutScene = nullptr;
		return E_FAIL;
	}
	m_bIsPlayingCutScene = true;
	return S_OK;
}

void CCutScenePlayer::Stop_CutScene()
{
	m_pCurCutScene = nullptr;
	m_bIsPlayingCutScene = false;
}

void CCutScenePlayer::Set_ViewPort(_fvector vLScreenDesc, _fvector vRScreenDesc, _bool bIsLerp,_float fLerpSpeed)
{
	if (nullptr == m_pGameInstance)
		return;
	bIsLerp ? m_pGameInstance->Set_GoalViewportInfo(vLScreenDesc, vRScreenDesc, fLerpSpeed) :
		m_pGameInstance->Set_ViewportInfo(vLScreenDesc, vRScreenDesc);
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


}
