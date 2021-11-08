#include "stdafx.h"
#include "..\public\Loading.h"
#include "GameInstance.h"
#include "MainCamera.h"
#include "SubCamera.h"
#include "Player.h"
#include "Terrain.h"

#include "GameEffect.h"
#include "DataLoader.h"

CLoading::CLoading(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice			(pDevice)
	, m_pDeviceContext	(pDeviceContext)
	, m_pGameInstance	(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY ThreadMain(void* pArg)
{
	CLoading::THREAD_ARG* pThreadArg = (CLoading::THREAD_ARG*)pArg;

	CLoading*	pLoading		= pThreadArg->pLoading;
	_uint		iThreadIndex	= pThreadArg->iThreadIndex;

	EnterCriticalSection(pLoading->Get_CriticalSectionPtr(iThreadIndex));

	Level::ID ePreLevel		= pLoading->Get_PreLevelID();
	Level::ID eNextLevel	= pLoading->Get_NextLevelID();

	FAILED_CHECK_RETURN(pLoading->Loading(ePreLevel, eNextLevel, iThreadIndex), EVENT_ERROR);

	LeaveCriticalSection(pLoading->Get_CriticalSectionPtr(iThreadIndex));

	return NO_EVENT;
}

CRITICAL_SECTION * CLoading::Get_CriticalSectionPtr(_uint iThreadIndex)
{
	if (m_iThreadCount <= iThreadIndex)
		return nullptr;

	return &m_arrCriticalSections[iThreadIndex];
}

const _bool CLoading::Is_FinishedToLoading() const
{
	for (_uint iIndex = 0; iIndex < m_iThreadCount; ++iIndex)
	{
		if (false == m_arrFinished[iIndex])
			return false;
	}

	return true;
}

HRESULT CLoading::NativeConstruct(Level::ID ePreLevelID, Level::ID eNextLevelID)
{
	m_ePreLevelID	= ePreLevelID;
	m_eNextLevelID	= eNextLevelID;

	m_iThreadCount			= 8;
	m_arrThreads			= new HANDLE[m_iThreadCount];
	m_arrCriticalSections	= new CRITICAL_SECTION[m_iThreadCount];
	m_arrThreadArgs			= new THREAD_ARG[m_iThreadCount];
	m_arrFinished			= new _bool[m_iThreadCount];
	ZeroMemory(m_arrFinished, sizeof(_bool) * m_iThreadCount);

	for (_uint iIndex = 0; iIndex < m_iThreadCount; ++iIndex)
	{
		m_arrThreadArgs[iIndex].pLoading		= this;
		m_arrThreadArgs[iIndex].iThreadIndex	= iIndex;

		InitializeCriticalSection(&m_arrCriticalSections[iIndex]);
		m_arrThreads[iIndex] = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, &m_arrThreadArgs[iIndex], 0, nullptr);
	}

	return S_OK;
}

HRESULT CLoading::Loading(Level::ID ePreLevelID, Level::ID eNextLevelID, _uint iThreadIndex)
{
	if (Level::LEVEL_STAGE == eNextLevelID)
		FAILED_CHECK_RETURN(LoadingForStage(iThreadIndex), E_FAIL);

	m_arrFinished[iThreadIndex] = true;

	return S_OK;
}

HRESULT CLoading::LoadingForStage(_uint iThreadIndex)
{
	if (0 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 129, 129, 1.f, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), "DefaultTechnique")), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_TGA, TEXT("../Bin/Resources/Texture/Grass_0.tga"))), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	}
	else if (1 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	}
	else if (2 == iThreadIndex)
	{
		WaitForSingleObject(m_arrThreads[1], INFINITE);
		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Mesh/", "Test2.fbx", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CPlayer::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

#pragma region Effects
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom")
			, CVIBuffer_PointInstance_Custom::Create(m_pDevice, m_pDeviceContext, 1000, TEXT("../Bin/ShaderFiles/Shader_PointCustom.hlsl"), "DefaultTechnique")), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop_01")
			, CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_TGA, TEXT("../Bin/Resources/Effect/2D/Fire_Loop_01.tga"))), E_FAIL);

		CDataLoader::Load_EffectData(TEXT("../Bin/Data/Effect/TestSave.txt"), m_pDevice, m_pDeviceContext);
#pragma endregion

	}

	return S_OK;
}

CLoading * CLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID)
{
	CLoading*	pInstance = new CLoading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(ePreLevelID, eNextLevelID)))
	{
		MSG_BOX("Failed to Create Instance - CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	for (_uint iIndex = 0; iIndex < m_iThreadCount; ++iIndex)
	{
		WaitForSingleObject(m_arrThreads[iIndex], INFINITE);
		DeleteCriticalSection(&m_arrCriticalSections[iIndex]);
		CloseHandle(m_arrThreads[iIndex]);
	}

	Safe_Delete_Array(m_arrThreads);
	Safe_Delete_Array(m_arrCriticalSections);
	Safe_Delete_Array(m_arrThreadArgs);
	Safe_Delete_Array(m_arrFinished);
}
