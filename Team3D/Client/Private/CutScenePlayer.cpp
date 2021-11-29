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
	Safe_AddRef(m_pGameInstance);
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	//¼ÒÇ°µé
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Performer"), CPerformer::Create(m_pDevice, m_pDeviceContext)), E_FAIL);


	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody_CutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("CodyCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ToyBox1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("ToyBoxCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May_CutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("MayCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RemoteContollerCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("RemoteContollerCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityBootsCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("GravityBootsCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SizeBeltCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("SizeBeltCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SizeBeltRemoteControllerCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("SizeBeltRemoteControllerCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);




	const _tchar* pLayerTag = L"Layer_Performer";
	CPerformer::PERFORMERDESC tDesc;
	CGameObject* pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_Cody_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc,&pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_Cody_CutScene1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag =TEXT("Component_Model_May_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_May_CutScene1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_ToyBox1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_ToyBox1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_RemoteContollerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_RemoteContollerCutScene1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_GravityBootsCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_GravityBootsCutScene1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_SizeBeltCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_SizeBeltCutScene1"), pPerformer);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_SizeBeltRemoteControllerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	Add_Performer(TEXT("Component_Model_SizeBeltRemoteControllerCutScene1"), pPerformer);



	FAILED_CHECK_RETURN(Add_CutScene(TEXT("CutScene_Intro"),CCutScene::Create(CCutScene::CutSceneOption::CutScene_Intro)),E_FAIL);
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
_bool CCutScenePlayer::Tick_CutScene()
{
	if (nullptr == m_pCurCutScene)
		return false;

	m_bIsPlayingCutScene = m_pCurCutScene->Tick_CutScene(m_dTimeDelta);
	if (false == m_bIsPlayingCutScene)
	{
		Safe_Release(m_pCurCutScene);
		m_pCurCutScene = nullptr;
	}
	return true;
}

void CCutScenePlayer::OffSetTimeDelta()
{
	m_dTimeDelta = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60"))	 * 3252.0* 1.02693;
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
