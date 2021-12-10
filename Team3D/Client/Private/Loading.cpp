#include "stdafx.h"
#include "..\Public\Loading.h"

#pragma region Headers

/* Framework */
#include <mutex>
#include "PlayerActor.h"
#include "CameraActor.h"
#include "SepdStaticActor.h"

/* Se */
#include "GravityPath.h"
#include "Laser_TypeA.h"
#include "Laser_TypeB.h"

/* Jung */
#include "Effect_Generator.h"
#include "WarpGate.h"
#include "Boss_Missile.h"
#include "WallLaserTrap.h"
#include "WallLaserTrap_Button.h"
#include "Space_Valve_Star.h"
#include "Space_Valve_Door.h"

/* Hye */
#include "Environment_Generator.h"
#include "HangingPlanet.h"
#include "RotationCylinder.h"
#include "Press.h"
#include "RotationBox.h"
#include "RotationFan_Base.h"
#include "RotationFan.h"
#include "Pedal.h"
#include "MoonUFO.h"
#include "BossFloor.h"
#include "BossDoor.h"

/* Taek */
#include "Terrain.h" /*Test*/
#include "Sky.h"
#include "ToyBoxButton.h"
#include "MoonBaboonCore.h"
#include "MoonBaboon_MainLaser.h"
#include "SpaceRail.h"

/* Yoon */
#include "May.h"
#include "UFO.h"
#include "Cody.h"
#include "Robot.h"
#include "Rocket.h"
#include "HookUFO.h"
#include "StarBuddy.h"
#include "RobotHead.h"
#include "BigButton.h"
#include "DummyWall.h"
#include "SpaceValve.h"
#include "MoonBaboon.h"
#include "RobotLever.h"
#include "RobotParts.h"
#include "MayJumpWall.h"
#include "PipeJumpWall.h"
#include "RobotBattery.h"
#include "TutorialDoor.h"
#include "NoBatterySign.h"
#include "SecurityCamera.h"
#include "BigButtonFrame.h"
#include "RotatedRobotBody.h"
#include "RotatedRobotHead.h"
#include "RotatedRobotLever.h"
#include "RotatedRobotParts.h"
#include "RotatedRobotBattery.h"
#include "SecurityCameraHandle.h"
#include "RotatedNoBatterySign.h"

/* Jin */
#include "SplashScreen.h"
#include "UI_Generator.h"
#include "PipeCurve.h"
#include "PressurePlate.h"
#include "PressurePlateFrame.h"
#include "PressurePlateLock.h"
#include "SupportFrame.h"
#include "PressureBigPlate.h"
#include "ControlRoom_Door.h"
#include "BatteryBox.h"
#include "ControlRoom_Battery.h"
#include "ControlRoom_Glass.h"
#include "UmbrellaBeam.h"
#include "UmbrellaBeam_Base.h"
#include "UmbrellaBeam_Stand.h"
#include "UmbrellaBeam_Joystick.h"

/* Jun */
#include "MainCamera.h"
#include "SubCamera.h"
#include "Cam_Helper.h"
#include "CutScenePlayer.h"
#include"Performer.h"

#include "Moon.h"
#include "MoonBaboon_SpaceShip.h"

#pragma endregion

std::mutex g_mutex;

CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY AssignThread(void* pArg)
{
	CLoading::THREAD_ARG* pThreadArg = (CLoading::THREAD_ARG*)pArg;

	CLoading*	pLoading = pThreadArg->pLoading;

	EnterCriticalSection(pLoading->Get_CriticalSectionPtr(0));

	pLoading->Assign();

	LeaveCriticalSection(pLoading->Get_CriticalSectionPtr(0));
	DeleteCriticalSection(pLoading->Get_CriticalSectionPtr(0));

	return NO_EVENT;
}

_uint APIENTRY ThreadMain(void* pArg)
{
	CLoading::THREAD_ARG* pThreadArg = (CLoading::THREAD_ARG*)pArg;

	CLoading*		pLoading = pThreadArg->pLoading;
	_uint			iWorkIndex = pThreadArg->iThreadIndex;
	list<_uint>&	ProcessingThreadList = pLoading->Get_ProcessingThreadList();

	EnterCriticalSection(pLoading->Get_CriticalSectionPtr(iWorkIndex));

	Level::ID ePreLevel = pLoading->Get_PreLevelID();
	Level::ID eNextLevel = pLoading->Get_NextLevelID();

	FAILED_CHECK_RETURN(pLoading->Loading(ePreLevel, eNextLevel, iWorkIndex), EVENT_ERROR);

	g_mutex.lock();
	auto iter = find(ProcessingThreadList.begin(), ProcessingThreadList.end(), iWorkIndex);
	NULL_CHECK_RETURN(iter != ProcessingThreadList.end(), E_FAIL);
	ProcessingThreadList.erase(iter);
	g_mutex.unlock();

	LeaveCriticalSection(pLoading->Get_CriticalSectionPtr(iWorkIndex));
	DeleteCriticalSection(pLoading->Get_CriticalSectionPtr(iWorkIndex));

	return NO_EVENT;
}

CRITICAL_SECTION* CLoading::Get_CriticalSectionPtr(_uint iWorkIndex)
{
	if (m_iWorkCount <= iWorkIndex)
		return nullptr;

	return &m_arrCriticalSections[iWorkIndex];
}

const _bool CLoading::Is_FinishedToLoading() const
{
	for (_uint iIndex = 0; iIndex < m_iWorkCount; ++iIndex)
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

	m_iThreadCount = __THREAD_NUM;
	m_iCurWorkIndex = 0;

	if (eNextLevelID == Level::LEVEL_STAGE)
		m_iWorkCount = 238;
	else if (eNextLevelID == Level::LEVEL_LOGO)
		m_iWorkCount = 1;

	m_arrThreads = new HANDLE[m_iWorkCount];
	m_arrCriticalSections = new CRITICAL_SECTION[m_iWorkCount];
	m_arrThreadArgs = new THREAD_ARG[m_iWorkCount];
	m_arrFinished = new _bool[m_iWorkCount];
	ZeroMemory(m_arrFinished, sizeof(_bool) * m_iWorkCount);

	m_arrThreadArgs[0].pLoading = this;
	m_arrThreadArgs[0].iThreadIndex = 0;

	InitializeCriticalSection(&m_arrCriticalSections[0]);
	m_arrThreads[0] = (HANDLE)_beginthreadex(nullptr, 0, AssignThread, &m_arrThreadArgs[0], 0, nullptr);
	CloseHandle(m_arrThreads[0]);

	return S_OK;
}

HRESULT CLoading::Assign()
{
	++m_iCurWorkIndex;

	while (m_iWorkCount > m_iCurWorkIndex)
	{
		if (m_ProcessingThreadList.size() >= m_iThreadCount)
		{
			continue;
		}
		else
		{
			_uint iWorkIndex = m_iCurWorkIndex++;

			g_mutex.lock();
			m_ProcessingThreadList.emplace_back(iWorkIndex);
			g_mutex.unlock();

			m_arrThreadArgs[iWorkIndex].pLoading = this;
			m_arrThreadArgs[iWorkIndex].iThreadIndex = iWorkIndex;

			InitializeCriticalSection(&m_arrCriticalSections[iWorkIndex]);
			m_arrThreads[iWorkIndex] = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, &m_arrThreadArgs[iWorkIndex], 0, nullptr);
			CloseHandle(m_arrThreads[iWorkIndex]);
		}
	}

	m_arrFinished[0] = true;

	return S_OK;
}

HRESULT CLoading::Loading(Level::ID ePreLevelID, Level::ID eNextLevelID, _uint iWorkIndex)
{
	if (Level::LEVEL_STAGE == eNextLevelID)
	{
		FAILED_CHECK_RETURN(LoadingForStage(iWorkIndex), E_FAIL);
	}
	else if (Level::LEVEL_LOGO == eNextLevelID)
	{
		FAILED_CHECK_RETURN(LoadingForLogo(iWorkIndex), E_FAIL);
	}

	m_arrFinished[iWorkIndex] = true;

	return S_OK;
}

HRESULT CLoading::LoadingForLogo(_uint iThreadIndex)
{
	if (1 == iThreadIndex)
	{
		UI_Generator->Add_Prototype_LogoTexture();
		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/Menu.dat"), Level::LEVEL_LOGO), E_FAIL);
		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/ChapterSelect.dat"), Level::LEVEL_LOGO, 1), E_FAIL);
	}

	return S_OK;
}

HRESULT CLoading::LoadingForStage(_uint iThreadIndex)
{
	if (1 == iThreadIndex)
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

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), CSubCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_May"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("May"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_May"), CMay::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("Cody"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Cody"), CCody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

		FAILED_CHECK_RETURN(UI_Generator->Load_Data(TEXT("../Bin/Resources/Data/UIData/UI.dat"), Level::LEVEL_STAGE), E_FAIL);

		__threadbreak;
	}
	else if (2 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Se(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Hye(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Yoon(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jung(), E_FAIL);

		__threadbreak;
	}
	else if (3 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jin(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Taek(), E_FAIL);
		FAILED_CHECK_RETURN(Create_GameObjects_SpaceStage_Jun(), E_FAIL);
		
		__threadbreak;
	}
	else if (4 == iThreadIndex)
	{
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Instancing_TXT(), E_FAIL);
		FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Prototype_GameObject_TXT(), E_FAIL);

		CEffect_Generator::GetInstance()->Create_Prototype_Resource_Stage1(m_pDevice, m_pDeviceContext);
		CEffect_Generator::GetInstance()->Load_EffectData(TEXT("../Bin/Resources/Data/EffectData/Stage1_Effect.dat"), m_pDevice, m_pDeviceContext);

		__threadbreak;
	}
	else if (10 <= iThreadIndex)
	{
		CEnvironment_Generator::GetInstance()->Load_Prototype_Model_Others_Space(iThreadIndex);
	}

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Se()
{
#ifndef __MAPLOADING_OFF
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_GravityPath"), CGravityPath::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#else
#ifdef __TEST_SE
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA"), CLaser_TypeA::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeB"), CLaser_TypeB::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#endif
#endif

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jung()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), CWarpGate::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), CBoss_Missile::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_WallLaserTrap_Button"), CWallLaserTrap_Button::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_WallLaserTrap"), CWallLaserTrap::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Space_Valve_Star"), CSpace_Valve_Star::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Space_Valve_Door"), CSpace_Valve_Door::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	_matrix PivotMatrix = XMMatrixIdentity();	
	PivotMatrix = XMMatrixScaling(0.0035f, 0.0035f, 0.0035f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValveBase"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Door_Right"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Door_Left"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve_Door_Left"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	PivotMatrix = XMMatrixScaling(0.00275f, 0.00275f, 0.00275f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Star_Valve"), CModel_Instance::Create(m_pDevice, m_pDeviceContext, 12, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_Star"), TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	_float fScale = 0.00275f;

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Hye()
{
	/* For.Alien Screen Texture */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Alien"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TEXTURE_TYPE::TYPE_TGA, TEXT("../Bin/Resources/Model/Environment/Others/Alien_Screen_01/Material/Alien_Screen_01%d.tga"), 4)), E_FAIL);

#ifdef __TEST_HYE
	/* For.MoonUFO */
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonUFO"), CMoonUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* For.Moon */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Plushie"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Plushie"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Moon"), CMoon::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Test Terrain*/
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TEXTURE_TYPE::TYPE_TGA, TEXT("../Bin/Resources/_Test/Texture/Grass_0.tga"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 129, 129, 1.f, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), "DefaultTechnique")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#endif //__TEST_HYE
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Taek()
{
#ifdef __TEST_TAEK
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Terrain"), CTextures::Create(m_pDevice, m_pDeviceContext, CTextures::TEXTURE_TYPE::TYPE_TGA, TEXT("../Bin/Resources/_Test/Terrain/Grass_0.tga"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 129, 129, 1.f, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), "DefaultTechnique")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rail Model */
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("GrindRail02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);

	/* Rail Path */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail01"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail1.Anim"), TEXT("GrindRail01"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail02"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail2.Anim"), TEXT("GrindRail02"), PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail03"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail3.Anim"), TEXT("GrindRail03"), PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail04"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail4.Anim"), TEXT("GrindRail04"), PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail05"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail5.Anim"), TEXT("GrindRail05"), PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail06"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail6.Anim"), TEXT("GrindRail06"), PivotMatrix)), E_FAIL);

	/* Rail Object */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SpaceRail"), CSpaceRail::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#else
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), CToyBoxButton::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboonCore"), CMoonBaboonCore::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon_MainLaser"), CMoonBaboon_MainLaser::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rail Path */
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail01"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail1.Anim"), TEXT("GrindRail01"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail02"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail2.Anim"), TEXT("GrindRail02"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail03"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail3.Anim"), TEXT("GrindRail03"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail04"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail4.Anim"), TEXT("GrindRail04"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail05"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail5.Anim"), TEXT("GrindRail05"), PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail06"), CPath::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Data/PathData/Space/rail6.Anim"), TEXT("GrindRail06"), PivotMatrix)), E_FAIL);
#endif // !__TEST_TAEK

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Yoon() 
{
	_matrix PivotMatrix = XMMatrixIdentity();

#ifndef __MAPLOADING_OFF
#pragma region Complete

	/* Robot_Body */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Robot"), CRobot::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Head */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotHead"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("RobotHead"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotHead"), CRobotHead::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Battery */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotBattery"), CRobotBattery::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Robot_Lever */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(90.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RealRobotLever"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotLever"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotLever"), CRobotLever::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* NoBatterySign */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 0.5f ,0.f));
 	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_NoBatterySignPivot"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("NoBatterySign"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign"), CNoBatterySign::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* SecurityCameraHandle */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SecurityCameraHandle"), CSecurityCameraHandle::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SecurityCamera"), CSecurityCamera::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Rocket"), CRocket::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), CStarBuddy::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_BigButton"), CBigButton::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_BigButtonFrame"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("BigButton_Frame"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_BigButtonFrame"), CBigButtonFrame::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_TutorialDoor"), CTutorialDoor::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* SpaceValve */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValveTwo"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), CSpaceValve::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_HookUFO"), CHookUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Bind Robot */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RobotParts"), CRobotParts::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rotated Robot_Body */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotBody"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RotatedRobot"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotBody"), CRotatedRobotBody::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rotated_Robot_Head */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotHead"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("RobotHead"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotHead"), CRotatedRobotHead::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rotated_Battery */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotBattery"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources//Model/Environment/Others/"), TEXT("RobotBattery"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotBattery"), CRotatedRobotBattery::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rotated_Robot_Lever */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-40.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotLever"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotLever"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotLever"), CRotatedRobotLever::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Rotated NoBatterySign */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) /** XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f))*/  * XMMatrixTranslation(0.5f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedNoBatterySign"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("NoBatterySign"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedNoBatterySign"), CRotatedNoBatterySign::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Bind RotatedRobot */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotParts"), CRotatedRobotParts::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#pragma endregion

	/* Dummy Wall*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_DummyWall"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Instancing/"), TEXT("ToyBox08_Variation"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_DummyWall"), CDummyWall::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* May Wall*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f)));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MayJumpWall"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Instancing/"), TEXT("ToyBox08_Variation"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MayJumpWall"), CMayJumpWall::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* Pipe Wall*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PipeJumpWall"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Instancing/"), TEXT("ToyBox08_Variation"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PipeJumpWall"), CPipeJumpWall::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	/* For. UFO */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UFO"), CUFO::Create(m_pDevice, m_pDeviceContext)), E_FAIL);



	/* For. MoonBaboon */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/AnimationModels/"), TEXT("MoonBaboon"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon"), CMoonBaboon::Create(m_pDevice, m_pDeviceContext)), E_FAIL);


#endif //__MAPLOADING_OFF
	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jin()
{
#ifndef __MAPLOADING_OFF
	_matrix PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(-2.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PipeCurve"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PipeCurve_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PipeCurve"), CPipeCurve::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlate"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PressurePlate"), CPressurePlate::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PressureBigPlate"), CPressureBigPlate::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlateFrame"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlateFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateFrame"), CPressurePlateFrame::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlateLock"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlateLock_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateLock"), CPressurePlateLock::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SupportFrame"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_SupportFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SupportFrame"), CSupportFrame::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(15.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Door"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Door"), CControlRoom_Door::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_BatteryBox"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("BatteryBox"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_BatteryBox"), CBatteryBox::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(270.f)) * XMMatrixTranslation(-0.7f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Battery"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotBattery"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Battery"), CControlRoom_Battery::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.06f, 0.09f, 0.04f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Glass"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Instancing/"), TEXT("GlassWall01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Glass"), CControlRoom_Glass::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) *  XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Umbrella"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam"), CUmbrellaBeam::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationX(XMConvertToRadians(90.f))*  XMMatrixRotationY(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam_Base"), CUmbrellaBeam_Base::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Stand"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Stand"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam_Stand"), CUmbrellaBeam_Stand::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Joystick"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_Joystick"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam_Joystick"), CUmbrellaBeam_Joystick::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
#endif

	return S_OK;
}

HRESULT CLoading::Create_GameObjects_SpaceStage_Jun()
{
	CCutScenePlayer* pCutScenePlayer = CCutScenePlayer::GetInstance();
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
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityBootsCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("GravityBootsCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SizeBeltCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("SizeBeltCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SizeBeltRemoteControllerCutScene1"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/CutSceneModels/"), TEXT("SizeBeltRemoteControllerCutScene1"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, PivotMatrix)), E_FAIL);




	const _tchar* pLayerTag = TEXT("Layer_Performer");
	CPerformer::PERFORMERDESC tDesc;
	CGameObject* pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_Cody_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_Cody_CutScene1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_May_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_May_CutScene1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_ToyBox1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_ToyBox1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_RemoteContollerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_RemoteContollerCutScene1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_GravityBootsCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_GravityBootsCutScene1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_SizeBeltCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_SizeBeltCutScene1"), pPerformer), E_FAIL);

	pPerformer = nullptr;
	tDesc.strModelTag = TEXT("Component_Model_SizeBeltRemoteControllerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Performer"), Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc, &pPerformer), E_FAIL);
	FAILED_CHECK_RETURN(pCutScenePlayer->Add_Performer(TEXT("Component_Model_SizeBeltRemoteControllerCutScene1"), pPerformer), E_FAIL);

	CCutScenePlayer::GetInstance()->NativeConstruct(m_pDevice, m_pDeviceContext);

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

	Safe_Delete_Array(m_arrThreads);
	Safe_Delete_Array(m_arrCriticalSections);
	Safe_Delete_Array(m_arrThreadArgs);
	Safe_Delete_Array(m_arrFinished);
}
