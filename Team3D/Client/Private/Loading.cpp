#include "stdafx.h"
#include "..\public\Loading.h"
#include "GameInstance.h"
#include "MainCamera.h"
#include "SubCamera.h"
#include "Cam_Helper.h"
#include "Terrain.h"
#include "TileBox.h"

#include "UI_Generator.h"
#include "Environment_Generator.h"
#include "Effect_Generator.h"

#include "Cody.h"
#include "May.h"
#include "UFO.h"
#include "MoonBaboon.h"

/* Interactive Objects */
#include "Rocket.h"
#include "StarBuddy.h"
#include "Robot.h"
#include "RobotHead.h"

/* Test */
#include "TestObject01.h"
#include "Sky.h"
#include "CameraActor.h"
#include"CutScenePlayer.h"

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

	m_iThreadCount			= 16;
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
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TYPE_TGA, TEXT("../Bin/Resources/_Test/Texture/Grass_0.tga"))), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), CStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), CTriggerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), CDynamicActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), CControllableActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), CCameraActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), CCam_Helper::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		CEffect_Generator::GetInstance()->Create_Prototype_Resource_Stage1(m_pDevice, m_pDeviceContext);

		_matrix	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Sky_Space"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Sky/"), TEXT("Sky_Space"), TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), CSky::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others15.txt")), E_FAIL);
	}
	else if (1 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		CEffect_Generator::GetInstance()->Load_EffectData(TEXT("../Bin/Resources/Data/EffectData/Stage1_Effect.dat"), m_pDevice, m_pDeviceContext);

		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("May"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_May"), CMay::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		CCutScenePlayer::GetInstance()->NativeConstruct(m_pDevice, m_pDeviceContext);
		/* For. Map */
		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		//PivotMatrix *= XMMatrixRotationX(XMConvertToRadians(90.f));
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_TileBox"), CModel_Instance::Create(m_pDevice, m_pDeviceContext, 10000, TEXT("../Bin/Resources/_Test/Model/"), TEXT("ToyBox01"), TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_TileBox"), CTileBox::Create(m_pDevice, m_pDeviceContext)), E_FAIL);


		/* Robot_Body */
		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/_Test/Model/"), TEXT("ControlRoom_Computer_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Robot"), CRobot::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		
		/*For. Interactive Objects */
		///* Robot_Body */
		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/InteractiveModels/"), TEXT("Robot"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Robot"), CRobot::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		/* Robot_Head */
		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotHead"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("RobotHead"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotHead"), CRobotHead::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	}
	else if (2 == iThreadIndex)
	{
		/* For. Character_Cody */
		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("Cody"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CCody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		/*For. Interactive Objects */

		/* Rocket */
		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/InteractiveModels/"), TEXT("Rocket"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Rocket"), CRocket::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		/* StarBuddy */
	/*	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath_01_Bend_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), CStarBuddy::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		*/////Test
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_TestObject01"), CTestObject01::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		//* For. UFO */

		//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		//PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UFO"), CUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		//* For. MoonBaboon */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("MoonBaboon"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon"), CMoonBaboon::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/UI.dat")), E_FAIL);

		/* For. Map Resources */
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Instancing_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others01.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_GameObject_Prototype(), E_FAIL);
	}
	else if (3 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others02.txt")), E_FAIL);
	}
	else if (4 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others03.txt")), E_FAIL);
	}
	else if (5 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others04.txt")), E_FAIL);
	}
	else if (6 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others05.txt")), E_FAIL);
	}
	else if (7 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others06.txt")), E_FAIL);
	}
	else if (8 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others07.txt")), E_FAIL);
	}
	else if (9 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others08.txt")), E_FAIL);
	}
	else if (10 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others09.txt")), E_FAIL);
	}
	else if (11 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others10.txt")), E_FAIL);
	}
	else if (12 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others11.txt")), E_FAIL);
	}
	else if (13 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others12.txt")), E_FAIL);
	}
	else if (14 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others13.txt")), E_FAIL);
	}
	else if (15 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Model_Data_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others14.txt")), E_FAIL);
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
