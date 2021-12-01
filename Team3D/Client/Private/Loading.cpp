#include "stdafx.h"
#include "..\Public\Loading.h"

#pragma region Headers

/* Framework */
#include "PlayerActor.h"
#include "CameraActor.h"
#include "SepdStaticActor.h"

/* Se */
#include "GravityPath.h"
/* Jung */
#include "Effect_Generator.h"
/* Hye */
#include "Environment_Generator.h"
/* Won */
#include "Terrain.h" /*Test*/
#include "Sky.h"
#include "ToyBoxButton.h"
#include "MoonBaboonCore.h"
#include "MoonBaboon_MainLaser.h"
#include "SpaceRail.h"
/* Yoon */
#include "Cody.h"
#include "May.h"
#include "UFO.h"
#include "MoonBaboon.h"
#include "Rocket.h"
#include "StarBuddy.h"
#include "Robot.h"
#include "RobotHead.h"
#include "RobotBattery.h"
#include "RobotLever.h"
#include "NoBatterySign.h"
#include "SecurityCameraHandle.h"
#include "SecurityCamera.h"
#include "BigButton.h"
#include "TutorialDoor.h"
#include "SpaceValve.h"
#include "BigPlanet.h"
#include "HookUFO.h"
/* Jin */
#include "UI_Generator.h"
/* Jun */
#include "MainCamera.h"
#include "SubCamera.h"
#include "Cam_Helper.h"

#pragma endregion

CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY ThreadMain(void* pArg)
{
	CLoading::THREAD_ARG* pThreadArg = (CLoading::THREAD_ARG*)pArg;

	CLoading* pLoading = pThreadArg->pLoading;
	_uint		iThreadIndex = pThreadArg->iThreadIndex;

	EnterCriticalSection(pLoading->Get_CriticalSectionPtr(iThreadIndex));

	Level::ID ePreLevel = pLoading->Get_PreLevelID();
	Level::ID eNextLevel = pLoading->Get_NextLevelID();

	FAILED_CHECK_RETURN(pLoading->Loading(ePreLevel, eNextLevel, iThreadIndex), EVENT_ERROR);

	LeaveCriticalSection(pLoading->Get_CriticalSectionPtr(iThreadIndex));

	return NO_EVENT;
}

CRITICAL_SECTION* CLoading::Get_CriticalSectionPtr(_uint iThreadIndex)
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
	m_ePreLevelID = ePreLevelID;
	m_eNextLevelID = eNextLevelID;

	m_iThreadCount = 16;
	m_arrThreads = new HANDLE[m_iThreadCount];
	m_arrCriticalSections = new CRITICAL_SECTION[m_iThreadCount];
	m_arrThreadArgs = new THREAD_ARG[m_iThreadCount];
	m_arrFinished = new _bool[m_iThreadCount];
	ZeroMemory(m_arrFinished, sizeof(_bool) * m_iThreadCount);

	for (_uint iIndex = 0; iIndex < m_iThreadCount; ++iIndex)
	{
		m_arrThreadArgs[iIndex].pLoading = this;
		m_arrThreadArgs[iIndex].iThreadIndex = iIndex;

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

#ifdef __16THREADS
HRESULT CLoading::LoadingForStage(_uint iThreadIndex)
{
	if (0 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), CStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), CTriggerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), CDynamicActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), CCameraActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_PlayerActor"), CPlayerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_SepdStaticActor"), CSepdStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), CCam_Helper::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		_matrix	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Sky_Space"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Sky/"), TEXT("Sky_Space"), TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), CSky::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

#ifndef __MAPLOADING_OFF
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others15.txt")), E_FAIL);
#endif

		__threadbreak;
	}
	else if (1 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/UI.dat")), E_FAIL);

		CEffect_Generator::GetInstance()->Create_Prototype_Resource_Stage1(m_pDevice, m_pDeviceContext);
		CEffect_Generator::GetInstance()->Load_EffectData(TEXT("../Bin/Resources/Data/EffectData/Stage1_Effect.dat"), m_pDevice, m_pDeviceContext);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("May"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_May"), CMay::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("Cody"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CCody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		__threadbreak;
	}
#ifndef __MAPLOADING_OFF
	else if (2 == iThreadIndex)
	{
		/* For. Map Resources */
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Instancing_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_GameObject_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others01.txt")), E_FAIL);

		__threadbreak;
	}
	else if (3 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others02.txt")), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Se(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jung(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Hye(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Won(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Yoon(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jin(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jun(), E_FAIL);

		__threadbreak;
	}
	else if (4 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others03.txt")), E_FAIL);

		__threadbreak;
	}
	else if (5 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others04.txt")), E_FAIL);

		__threadbreak;
	}
	else if (6 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others05.txt")), E_FAIL);

		__threadbreak;
	}
	else if (7 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others06.txt")), E_FAIL);

		__threadbreak;
	}
	else if (8 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others07.txt")), E_FAIL);

		__threadbreak;
	}
	else if (9 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others08.txt")), E_FAIL);

		__threadbreak;
	}
	else if (10 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others09.txt")), E_FAIL);

		__threadbreak;
	}
	else if (11 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others10.txt")), E_FAIL);

		__threadbreak;
	}
	else if (12 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others11.txt")), E_FAIL);

		__threadbreak;
	}
	else if (13 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others12.txt")), E_FAIL);

		__threadbreak;
	}
	else if (14 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others13.txt")), E_FAIL);

		__threadbreak;
	}
	else if (15 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others14.txt")), E_FAIL);

		__threadbreak;
	}
#endif

	return S_OK;
}
#elif defined __8THREADS
HRESULT CLoading::LoadingForStage(_uint iThreadIndex)
{
	if (0 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), CStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), CTriggerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), CDynamicActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_PlayerActor"), CPlayerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_SepdStaticActor"), CSepdStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), CCameraActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), CCam_Helper::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		_matrix	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Sky_Space"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Sky/"), TEXT("Sky_Space"), TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), CSky::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		
#ifndef __MAPLOADING_OFF
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others15.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others02.txt")), E_FAIL);
#else
		Create_GameObjects_SpaceStage_Test(); /* Test */
#endif

		__threadbreak;
	}
	else if (1 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/UI.dat")), E_FAIL);

		CEffect_Generator::GetInstance()->Create_Prototype_Resource_Stage1(m_pDevice, m_pDeviceContext);
		CEffect_Generator::GetInstance()->Load_EffectData(TEXT("../Bin/Resources/Data/EffectData/Stage1_Effect.dat"), m_pDevice, m_pDeviceContext);

		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("May"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_May"), CMay::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("Cody"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CCody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

#ifndef __MAPLOADING_OFF
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others05.txt")), E_FAIL);
#endif
		__threadbreak;
	}
#ifndef __MAPLOADING_OFF
	else if (2 == iThreadIndex)
	{
		/* For. Map Resources */
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Instancing_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_GameObject_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others01.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others04.txt")), E_FAIL);

		__threadbreak;
	}
	else if (3 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others09.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others14.txt")), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Se(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jung(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Hye(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Won(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Yoon(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jin(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jun(), E_FAIL);

		__threadbreak;
	}
	else if (4 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others06.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others11.txt")), E_FAIL);

		__threadbreak;
	}
	else if (5 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others08.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others10.txt")), E_FAIL);

		__threadbreak;
	}
	else if (6 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others07.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others12.txt")), E_FAIL);

		__threadbreak;
	}
	else if (7 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others03.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others13.txt")), E_FAIL);

		__threadbreak;
	}
#endif

	return S_OK;
}
#elif defined __6THREADS
HRESULT CLoading::LoadingForStage(_uint iThreadIndex)
{
	if (0 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), CStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), CTriggerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), CDynamicActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_SepdStaticActor"), CSepdStaticActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_PlayerActor"), CPlayerActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), CCameraActor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), CCam_Helper::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		_matrix	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Sky_Space"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Sky/"), TEXT("Sky_Space"), TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), CSky::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

#ifndef __MAPLOADING_OFF
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others02.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others07.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others15.txt")), E_FAIL);
#endif

		__threadbreak;
	}
	else if (1 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/UI.dat")), E_FAIL);

		CEffect_Generator::GetInstance()->Create_Prototype_Resource_Stage1(m_pDevice, m_pDeviceContext);
		CEffect_Generator::GetInstance()->Load_EffectData(TEXT("../Bin/Resources/Data/EffectData/Stage1_Effect.dat"), m_pDevice, m_pDeviceContext);

		_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("May"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_May"), CMay::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("Cody"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CCody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

#ifndef __MAPLOADING_OFF
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others05.txt")), E_FAIL);
#endif

		__threadbreak;
	}
#ifndef __MAPLOADING_OFF
	else if (2 == iThreadIndex)
	{
		/* For. Map Resources */
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Instancing_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_GameObject_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others01.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others04.txt")), E_FAIL);

		__threadbreak;
	}
	else if (3 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others09.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others12.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others14.txt")), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Se(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jung(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Hye(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Won(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Yoon(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jin(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jun(), E_FAIL);

		__threadbreak;
	}
	else if (4 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others03.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others06.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others11.txt")), E_FAIL);

		__threadbreak;
	}
	else if (5 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others08.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others10.txt")), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_TXT(TEXT("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Others13.txt")), E_FAIL);

		__threadbreak;
	}
#endif

	return S_OK;
}
#endif

HRESULT CLoading::Create_GameObjects_SpaceStage_Test()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TEXTURE_TYPE::TYPE_TGA, TEXT("../Bin/Resources/_Test/Texture/Grass_0.tga"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 129, 129, 1.f, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), "DefaultTechnique")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rail Model */
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail04"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail04"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail05"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail05"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail06"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail06"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);

	/* Rail Path */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail01"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail1.Anim"), TEXT("GrindRail01"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail02"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Test.Anim"), TEXT("GrindRail02"), PivotMatrix)), E_FAIL);

	/* Rail Object */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SpaceRail"), CSpaceRail::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Se()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_GravityPath"), CGravityPath::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jung()
{
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Hye()
{
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Won()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), CToyBoxButton::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboonCore"), CMoonBaboonCore::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon_MainLaser"), CMoonBaboon_MainLaser::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	_matrix PivotMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	//_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_SpaceRail0"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Test.Anim"), TEXT("SpaceRail0"), PivotMatrix)), E_FAIL);

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Yoon()
{
	_matrix PivotMatrix = XMMatrixIdentity();

	/* Robot_Body */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Robot"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Robot"), CRobot::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Head */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotHead"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("RobotHead"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotHead"), CRobotHead::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Battery */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotBattery"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("RobotBattery"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotBattery"), CRobotBattery::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Lever */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotLever"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotLever"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotLever"), CRobotLever::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* NoBatterySign */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_NoBatterySign"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("NoBatterySign"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign"), CNoBatterySign::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* SecurityCameraHandle */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-270.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SecurityCameraHandle"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SecurityCameraHandle"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SecurityCameraHandle"), CSecurityCameraHandle::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* SecurityCamera */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SecurityCamera"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SecurityCamera"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SecurityCamera"), CSecurityCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rocket */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Rocket"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Rocket"), CRocket::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* StarBuddy */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("StarBuddy"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), CStarBuddy::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	//
	/* BigButton(Only Use Cody) */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_BigButton"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("BigButton"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_BigButton"), CBigButton::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Tutorial Door (Only Use May) + 테스트용으로 회전 시켜 놓음 */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_TutorialDoor"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("DoorWay"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_TutorialDoor"), CTutorialDoor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* SpaceValve */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), CSpaceValve::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* BigPlanet */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_BigPlanet"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("HangingPlanet"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_HangingPlanet"), CBigPlanet::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Hook_UFO */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Hook_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Hook_UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_HookUFO"), CHookUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	////Test
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_TestObject01"), CTestObject01::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	///* For. UFO */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UFO"), CUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	///* For. MoonBaboon */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	//PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("MoonBaboon"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon"), CMoonBaboon::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jin()
{
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jun()
{
	return S_OK;
}

CLoading* CLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID)
{
	CLoading* pInstance = new CLoading(pDevice, pDeviceContext);

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
