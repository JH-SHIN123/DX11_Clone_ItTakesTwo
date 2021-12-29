#include "stdafx.h"
#include "..\Public\Level_Stage.h"

#pragma region Headers

/* Framework */
#include "Loading.h"
#include "EndingCredit_Manager.h"
#include "Level_Loading.h"
#include "UI_Generator.h"
/* Se */
/* Jung */
#include "Effect_Generator.h"
#include "InGameEffect.h"
#include "WarpGate.h"
#include "Effect_Env_Particle_Field.h"
/* Hye */
#include "Environment_Generator.h"
#include "Dynamic_Env.h"
#include "HangingPlanet.h"
/* Taek */
#include "MoonBaboonCore.h"
#include "VolumeLight.h"
#include "LightUtility.h"
/* Yoon */
#include "RotatedRobotParts.h"
#include "RobotParts.h"
#include "ToyBoxButton.h"
#include "Rope.h"
#include "UFORadarSet.h"
#include "Boss_Missile.h"
/* Jin */
/* Jun */
#include "Camera.h"
#include"CutScenePlayer.h"
#include"Performer.h"
#include "Level_Loading.h"

#pragma endregion

CLevel_Stage::CLevel_Stage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}
 
HRESULT CLevel_Stage::NativeConstruct()
{
	/* Sound */
	m_pGameInstance->Stop_SoundAll();

	CLevel::NativeConstruct();

	/* Priority */
	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Sky(TEXT("Layer_Sky")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Cody(TEXT("Layer_Cody")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_May(TEXT("Layer_May")), E_FAIL);

#ifndef __MAPLOADING_OFF
	/* Se */
	FAILED_CHECK_RETURN(Ready_Layer_GravityPath(TEXT("Layer_GravityPath")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Earth(TEXT("Layer_Earth")), E_FAIL);
	/* Jung */
	FAILED_CHECK_RETURN(Ready_Layer_WarpGate(TEXT("Layer_WarpGate")), E_FAIL);	
	FAILED_CHECK_RETURN(Ready_Layer_Wormhole(TEXT("Layer_Wormhole")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_WallLaserTrap(TEXT("Layer_WallLaserTrap")), E_FAIL);	
	FAILED_CHECK_RETURN(Ready_Layer_TutorialDoor(TEXT("Layer_TutorialDoor")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GravityPipe(TEXT("Layer_GravityPipe")), E_FAIL); 
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles_Main(TEXT("Layer_Env_Particle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles_PinBall(TEXT("Layer_Env_Particle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles_Rail(TEXT("Layer_Env_Particle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles_Boss(TEXT("Layer_Env_Particle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles_Moon(TEXT("Layer_Env_Particle")), E_FAIL);
	/* Hye */
	FAILED_CHECK_RETURN(Ready_Layer_Planet(TEXT("Layer_Planet")), E_FAIL);
	/* Taek */
	FAILED_CHECK_RETURN(Ready_Layer_ToyBoxButton(TEXT("Layer_ToyBoxButton")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboonCore(TEXT("Layer_MoonBaboonCore")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon_MainLaser(TEXT("Layer_MoonBaboon_MainLaser")), E_FAIL);
	/* Yoon */
	FAILED_CHECK_RETURN(Ready_Layer_Rocket(TEXT("Layer_Rocket")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_StarBuddy(TEXT("Layer_StarBuddy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigButton(TEXT("Layer_BigButton")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigButtonFrame(TEXT("Layer_BigButtonFrame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RobotParts(TEXT("Layer_RobotParts")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RotatedRobotParts(TEXT("Layer_RotatedRobotParts")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCameraHandle(TEXT("Layer_SecurityCameraHandle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCamera(TEXT("Layer_SecurityCamera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SpaceValve(TEXT("Layer_SpaceValve")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Hook_UFO(TEXT("Layer_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_DummyWall(TEXT("Layer_DummyWall")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MayJumpWall(TEXT("Layer_MayJumpWall")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PipeJumpWall(TEXT("Layer_PipeJumpWall")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Rope(TEXT("Layer_Rope")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UFORadarSet(TEXT("Layer_UFORadarSet")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_TestRocket(TEXT("Layer_BossMissile")), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_EndingRocket(TEXT("Layer_EndingRocket")), E_FAIL);

	/* Jin */
	FAILED_CHECK_RETURN(Ready_Layer_ControlRoomPuzzle(TEXT("Layer_PressureBigPlate"), TEXT("GameObject_PressureBigPlate")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_ControlRoom_Glass(TEXT("Layer_ControlRoom_Glass")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Umbrella_Joystick(TEXT("Layer_UmbrellaBeam_Joystick")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UFO(TEXT("Layer_UFO")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon(TEXT("Layer_MoonBaboon")), E_FAIL);

	/* Jun */
	FAILED_CHECK_RETURN(Ready_Layer_Performer(TEXT("Layer_Performer")), E_FAIL);
	
	/* For. Environment */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Stage_Space(), E_FAIL);

	/* For.Environment_EndingCredit */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_EndingCredit(), E_FAIL);

#else
	FAILED_CHECK_RETURN(Ready_Test(), E_FAIL);
#endif
	/* Script */
	FAILED_CHECK_RETURN(Ready_Layer_Script(TEXT("Layer_Script")), E_FAIL);

#ifdef __FIRST_CUTSCENE_OFF
	CGameInstance::GetInstance()->Play_Sound(TEXT("Bgm_Main.wav"), CHANNEL_BGM, 0.f, true);
	CGameInstance::GetInstance()->Sound_FadeIn(CHANNEL_BGM, 0.15f, 3.f);
#endif // __FIRST_CUTSCENE_OFF

	return S_OK;
}

_int CLevel_Stage::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

	CGameInstance* pGameinstance = CGameInstance::GetInstance();

#ifdef __INSTALL_LIGHT
	CLight_Generator::GetInstance()->KeyInput(dTimedelta);

#endif // __TEST_TAEK
	
#ifdef __PLAY_CUTSCENE

#ifndef __FIRST_CUTSCENE_OFF
	if (false == CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Intro))
	{
		CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Intro");
		CCutScenePlayer::GetInstance()->Set_IsCutScenePlayer(CCutScene::CutSceneOption::CutScene_Intro, true);
	}

#endif // __FIRST_CUTSCENE_OFF
#endif // __PLAY_CUTSCENE

	/* For.EndingCredit */
	if (m_pGameInstance->Key_Down(DIK_END))
	{
		m_pGameInstance->Sound_FadeOut(CHANNEL_BGM, 0.f, 1.f);
		m_iLevelStep = 2; 
		m_pGameInstance->Play_Sound(TEXT("EndingCredit_BGM.wav"), CHANNEL_TYPE::CHANNEL_ENDINGCREDIT, 0.2f);
		ENDINGCREDIT->Create_Environment();
	}
	if (m_iLevelStep == 2) { Tick_EndingCredit(dTimedelta); }

	if (m_iLevelStep == 3)
	{
		m_pGameInstance->Stop_SoundAll();
		FAILED_CHECK_RETURN(m_pGameInstance->Change_CurrentLevel(CLevel_Loading::Create(m_pDevice, m_pDeviceContext, Level::LEVEL_STAGE, Level::LEVEL_LOGO)), EVENT_ERROR);
	}

	return NO_EVENT;
}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage::Clone_StaticGameObjects_ByFile(const _tchar * pFilePath, const _tchar * pLayerTag, const _tchar* pGameObjectTag, GameID::Enum eID, _float fCullRadius)
{
	DWORD					dwByte;
	_tchar					szPrototypeTag[MAX_PATH] = L"";
	_uint					iLevelIndex = 0;
	CStatic_Env::ARG_DESC	tStatic_Env_Desc;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte) break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tStatic_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		tStatic_Env_Desc.eGameID = eID;
		tStatic_Env_Desc.fCullRadius = fCullRadius;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, pGameObjectTag, &tStatic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}
  
HRESULT CLevel_Stage::Ready_Test()
{
	/* Se */
#ifdef __TEST_SE
#endif 

	/* Jung */
#ifdef __TEST_JUNG

	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;
	Arg_Desc.vPosition = { 60.f, 0.f, 30.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	Arg_Desc.vDefaultSize = { 1.5f, 1.5f };
	Arg_Desc.fSpeedPerSec = 0.5f;
	Arg_Desc.fReSizing_Power = 0.01f;
	Arg_Desc.fReSize = 0.05f;
	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.fResetPosTime = 10.f;
	Arg_Desc.vTextureUV = { 1, 1 };
	Arg_Desc.IsGrouping = true;
	Arg_Desc.iGrouping_Count = 25;
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field_Star"), &Arg_Desc), E_FAIL);
	//
	//Arg_Desc.vDefaultSize = { 0.5f, 0.5f };
	//Arg_Desc.IsGrouping = false;
	//Arg_Desc.vTextureUV = { 2, 2 };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field_Dust"), &Arg_Desc), E_FAIL);

#endif

	/* Hye */
#ifdef __TEST_HYE
	Ready_Layer_SecurityCamera(TEXT("Layer_Test"));
#endif // __TEST_HYE

	/* Teak */
#ifdef __TEST_TAEK
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_ToyBoxButton", Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton")), E_FAIL);
#endif

	/* Yoon */
#ifdef __TEST_YOON
#endif

	/* Jin */

	/* Jun */

	return S_OK;
}

#pragma region Se
HRESULT CLevel_Stage::Ready_Layer_GravityPath(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(Clone_StaticGameObjects_ByFile(TEXT("../Bin/Resources/Data/MapData/GravityPath_SelectStatic.dat"), pLayerTag, TEXT("GameObject_GravityPath"), GameID::eGRAVITYPATH_SIDE, 30.f), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Earth(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Earth", Level::LEVEL_STAGE, TEXT("GameObject_Earth")), E_FAIL);
	return S_OK;
}
#pragma endregion

#pragma region Jung
HRESULT CLevel_Stage::Ready_Layer_WarpGate(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::MAIN_UMBRELLA,	6.0)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::STAGE_UMBRELLA,	6.0)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::MAIN_PLANET,		6.0)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::STAGE_PLANET,	6.0)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::MAIN_TENNIS,		0.2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::STAGE_TENNIS,	0.2)), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Wormhole(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE Data;

	_float4 vPos = { 0.f, -99.f, -1000.f, 1.f };
	memcpy(&Data.WorldMatrix.m[3][0], &vPos, sizeof(_float4));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Wormhole"), &Data), E_FAIL);

	vPos = { 0.f, -99.f, -1500.f, 1.f };
	memcpy(&Data.WorldMatrix.m[3][0], &vPos, sizeof(_float4));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Wormhole"), &Data), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_WallLaserTrap(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WallLaserTrap_Button")), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_GravityPipe(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE Data;
	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = { 62.9901505f, 35.f, 195.674637f, 1.f };
	Data.iPlayerValue = 0;
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Gravity_Pipe"), &Data), E_FAIL);

	WorldMatrix.r[3] = { 62.9901505f, 157.f, 195.674637f,1.f };
	Data.iPlayerValue = 1;
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Gravity_Pipe"), &Data), E_FAIL);
	return S_OK;
}
#pragma region Env_Particles
HRESULT CLevel_Stage::Ready_Layer_Env_Particles_Main(const _tchar * pLayerTag)
{
	_tchar szProtoTypeName[MAX_PATH] = TEXT("GameObject_2D_Env_Particle_Field");
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;

#pragma region Particle
	
	Arg_Desc.iInstanceCount = 2000;
	Arg_Desc.vPosition = { 60.f, 0.f, 60.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 30.f, 200.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 500;
	Arg_Desc.vPosition = { 63.f, 60.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 80.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 63.f, 125.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 60.f, 30.f, 60.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

#pragma endregion

#pragma region Star
	lstrcpy(szProtoTypeName, TEXT("GameObject_2D_Env_Particle_Field_Star"));
	Arg_Desc.vDefaultSize = { 0.9f, 0.9f };
	Arg_Desc.vTextureUV = { 1, 1 };
	Arg_Desc.fSpeedPerSec = 0.5f;

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 60.f, 0.f, 100.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 45.f, 200.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 54;
	Arg_Desc.vPosition = { 63.f, 125.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 30.f, 50.f };
	Arg_Desc.iGrouping_Count = 6;
	Arg_Desc.IsGrouping = true;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);
	Arg_Desc.iGrouping_Count = 1;
	Arg_Desc.IsGrouping = false;

#pragma endregion

#pragma region Dust
	lstrcpy(szProtoTypeName, TEXT("GameObject_2D_Env_Particle_Field_Dust"));
	Arg_Desc.vDefaultSize = { 0.5f, 0.5f };
	Arg_Desc.vTextureUV = { 2, 2 };
	Arg_Desc.fSpeedPerSec = 0.0f;

	Arg_Desc.iInstanceCount = 70;
	Arg_Desc.vPosition = { 60.f, 0.f, 100.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 200.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);


	Arg_Desc.iInstanceCount = 54;
	Arg_Desc.vPosition = { 63.f, 125.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 30.f, 50.f };
	Arg_Desc.iGrouping_Count = 3;
	Arg_Desc.IsGrouping = true;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

#pragma endregion

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Env_Particles_PinBall(const _tchar * pLayerTag)
{
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;

#pragma region Particle

	Arg_Desc.iInstanceCount = 4000;
	Arg_Desc.vPosition = { 60.f, 0.f, 60.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 30.f, 200.f };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);


#pragma endregion

#pragma region Star
	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 60.f, 0.f, 100.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 45.f, 200.f };
	Arg_Desc.vDefaultSize = { 0.8f, 0.8f };
	Arg_Desc.vTextureUV = { 1, 1 };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field_Star"), &Arg_Desc), E_FAIL);
#pragma endregion

#pragma region Dust

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 60.f, 0.f, 100.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 45.f, 200.f };
	Arg_Desc.vDefaultSize = { 0.8f, 0.8f };
	Arg_Desc.vTextureUV = { 1, 1 };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field_Star"), &Arg_Desc), E_FAIL);

#pragma endregion

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Env_Particles_Rail(const _tchar * pLayerTag)
{
	_tchar szProtoTypeName[MAX_PATH] = TEXT("GameObject_2D_Env_Particle_Field");
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;

#pragma region Particle
	Arg_Desc.fSpeedPerSec = 0.5f;
	Arg_Desc.fReSize = 0.05f;

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 765.f, 725.f, 270.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 30.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 50;
	Arg_Desc.vPosition = { 690.f, 725.f, 110.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 30.f, 30.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);
	
	Arg_Desc.iInstanceCount = 200;
	Arg_Desc.vPosition = { 890.f, 717.f, 340.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 1500;
	Arg_Desc.vPosition = { 997.f, 708.f, 184.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 70.f, 70.f, 70.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);


	Arg_Desc.iInstanceCount = 400;
	Arg_Desc.vPosition = { 638.f, 756.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

#pragma endregion

#pragma region Star
	lstrcpy(szProtoTypeName, TEXT("GameObject_2D_Env_Particle_Field_Star"));
	Arg_Desc.vDefaultSize = { 0.9f, 0.9f };
	Arg_Desc.vTextureUV = { 1, 1 };
	Arg_Desc.fSpeedPerSec = 0.75f;

	Arg_Desc.iInstanceCount = 25;
	Arg_Desc.vPosition = { 765.f, 725.f, 270.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 20.f, 45.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 30;
	Arg_Desc.vPosition = { 690.f, 725.f, 110.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 30.f, 30.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);
	
	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 843.5f, 710.f, 115.3f, 1.f };
	Arg_Desc.vRadiusXYZ = { 40.f, 50.f, 40.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 250;
	Arg_Desc.vPosition = { 997.f, 708.f, 184.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 70.f, 70.f, 70.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 638.f, 756.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

#pragma endregion

#pragma region Dust
	lstrcpy(szProtoTypeName, TEXT("GameObject_2D_Env_Particle_Field_Dust"));
	Arg_Desc.vDefaultSize = { 0.75f, 0.75f };
	Arg_Desc.vTextureUV = { 2, 2 };
	Arg_Desc.fSpeedPerSec = 1.f;
	Arg_Desc.iGrouping_Count = 7;
	Arg_Desc.IsGrouping = true;
	
	Arg_Desc.iInstanceCount = 70;
	Arg_Desc.vPosition = { 890.f, 717.f, 340.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 35;
	Arg_Desc.vPosition = { 843.5f, 710.f, 115.3f, 1.f };
	Arg_Desc.vRadiusXYZ = { 20.f, 50.f, 20.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 210;
	Arg_Desc.vPosition = { 997.f, 708.f, 184.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 70.f, 70.f, 70.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 80;
	Arg_Desc.iGrouping_Count = 4;
	Arg_Desc.vPosition = { 638.f, 756.f, 195.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 50.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, szProtoTypeName, &Arg_Desc), E_FAIL);

#pragma endregion
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Env_Particles_Boss(const _tchar * pLayerTag)
{
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;

#pragma region Particle

#pragma endregion

#pragma region Star

#pragma endregion

#pragma region Dust

#pragma endregion

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Env_Particles_Moon(const _tchar * pLayerTag)
{
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;

#pragma region Particle

	Arg_Desc.iInstanceCount = 4000;
	Arg_Desc.vPosition = { 60.f, 0.f, 60.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 30.f, 200.f };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);

	Arg_Desc.iInstanceCount = 100;
	Arg_Desc.vPosition = { 60.f, 0.f, 100.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 45.f, 200.f };
	Arg_Desc.vDefaultSize = { 0.8f, 0.8f };
	Arg_Desc.vTextureUV = { 1, 1 };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field_Star"), &Arg_Desc), E_FAIL);

#pragma endregion

#pragma region Star
	Arg_Desc.iInstanceCount = 4000;
	Arg_Desc.vPosition = { 60.f, 10000.f, 30.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 45.f, 45.f, 200.f };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);

#pragma endregion

#pragma region Dust

	Arg_Desc.iInstanceCount = 5000;
	Arg_Desc.vPosition = { 60.f, 10000.f, 30.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 50.f, 50.f, 200.f };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);

#pragma endregion

	return S_OK;
}
#pragma endregion
#pragma endregion

#pragma region Hye 
HRESULT CLevel_Stage::Ready_Layer_Planet(const _tchar * pLayerTag)
{
	/* 행성밀기 */
	CHangingPlanet::ARG_DESC tPlanetArg;
	_matrix World = XMMatrixIdentity();
	lstrcpy(tPlanetArg.DynamicDesc.szModelTag, TEXT("Component_Model_Hanging_Planet"));
	XMStoreFloat4x4(&tPlanetArg.DynamicDesc.WorldMatrix, World);
	tPlanetArg.DynamicDesc.iMaterialIndex = 0;
	tPlanetArg.DynamicDesc.iOption = 0;

	tPlanetArg.vJointPosition = _float3(1000.f, 740.f, 213.f);
	tPlanetArg.vOffset = _float3(0.f, 33.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Hanging_Planet"), &tPlanetArg), E_FAIL);

	tPlanetArg.vJointPosition = _float3(1000.f, 755.f, 176.5f);
	tPlanetArg.vOffset = _float3(0.f, 40.f, 0.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Hanging_Planet"), &tPlanetArg), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Script(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Script")), E_FAIL);

	return S_OK;
}
#pragma endregion

#pragma region Won
HRESULT CLevel_Stage::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* For.Directional : Ambient / Specular Zero - 0.45 */ 
	Ready_DirectionalLight(TEXT("Sun"), _float3(1.f, -1.f, 1.f), _float4(0.45f, 0.45f, 0.45f, 1.f), _float4(0.35f,0.35f,0.35f,1.f), _float4(0.45f, 0.45f, 0.45f,1.f));

#ifndef __MAPLOADING_OFF
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_2Floor.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_BossRoom.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_BossRoom_Floor1.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_BossRoom_Floor2.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_Bridge.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_ComputeRoom.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_ControlRoom.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_InShip.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_Pinball.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_Planet_Robot.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_Start.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_ToyBoxButton_Back.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_BASICLIGHT, TEXT("../Bin/Resources/Data/LightData/BasicLight_Umbrella.dat"));

	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_EFFECTLIGHT, TEXT("../Bin/Resources/Data/LightData/EffectLight_Rail_Bg.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_EFFECTLIGHT, TEXT("../Bin/Resources/Data/LightData/EffectLight_Rail_Bg2.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_EFFECTLIGHT, TEXT("../Bin/Resources/Data/LightData/EffectLight_Start.dat"));

	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_VOLUMELIGHT, TEXT("../Bin/Resources/Data/LightData/VolumeLight_ComputeRoom.dat"));
	CLightUtility::Load_StaticLightData(CLightUtility::LOAD_VOLUMELIGHT, TEXT("../Bin/Resources/Data/LightData/VolumeLight_Pinball.dat"));

	Ready_VolumeLight_Rail();
#endif

	return S_OK;
}

HRESULT CLevel_Stage::Ready_DirectionalLight(const _tchar* pLightTag, _float3 vDirection, _float4 vDiffuse, _float4 vAmbient, _float4 vSpecular)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return pGameInstance->Add_Light(LightStatus::eDIRECTIONAL, CLight::Create(pLightTag, &LIGHT_DESC(LIGHT_DESC::TYPE_DIRECTIONAL, vDirection, vDiffuse, vAmbient, vSpecular)));
}

HRESULT CLevel_Stage::Ready_VolumeLight_Rail()
{
	CVolumeLight* pLight = nullptr;

	LIGHT_DESC lightDesc;
	CVolumeLight::VOLUMELIGHT_DESC volumeLightDesc;

	lightDesc.eType = LIGHT_DESC::TYPE_POINT;
	lightDesc.vPosition = { 843.28f, 663.f ,108.225f };
	lightDesc.fRange = 60.f;
	lightDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	lightDesc.vSpecular = { 1.f,1.f,1.f,1.f };

	volumeLightDesc.tLightDesc = lightDesc;
	volumeLightDesc.tVolumeDesc.eVolumeType = CVolumeObject::TYPE_CUBE;
	volumeLightDesc.tVolumeDesc.fCullRadius = 1000;
	volumeLightDesc.tVolumeDesc.vInnerColor = { 1.f,1.f,1.f};
	volumeLightDesc.tVolumeDesc.vOuterColor = { 0.f,0.5f,1.f };
	_matrix WorldMatrix = XMMatrixScaling(60.f, 60.f, 60.f);
	WorldMatrix *= XMMatrixTranslation(843.28f, 663.f, 108.225f);
	XMStoreFloat4x4(&volumeLightDesc.tVolumeDesc.WorldMatrix, WorldMatrix);
	
	pLight = CVolumeLight::Create(TEXT("Static_Volume"), &volumeLightDesc);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Light(LightStatus::eSTATIC, pLight), E_FAIL);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	lightDesc.eType = LIGHT_DESC::TYPE_POINT;
	lightDesc.vPosition = { 678.f, 729.5f, 120.86f };
	lightDesc.fRange = 60.f;
	lightDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	lightDesc.vSpecular = { 1.f,1.f,1.f,1.f };

	volumeLightDesc.tLightDesc = lightDesc;
	volumeLightDesc.tVolumeDesc.eVolumeType = CVolumeObject::TYPE_CUBE;
	volumeLightDesc.tVolumeDesc.fCullRadius = 1000;
	volumeLightDesc.tVolumeDesc.vInnerColor = { 1.f,1.f,1.f };
	volumeLightDesc.tVolumeDesc.vOuterColor = { 1.f,0.5f,0.5f };
	WorldMatrix = XMMatrixScaling(42.f, 42.f, 42.f);
	WorldMatrix *= XMMatrixTranslation(678.f, 729.5f, 120.86f);
	XMStoreFloat4x4(&volumeLightDesc.tVolumeDesc.WorldMatrix, WorldMatrix);

	pLight = CVolumeLight::Create(TEXT("Static_Volume"), &volumeLightDesc);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Light(LightStatus::eSTATIC, pLight), E_FAIL);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	lightDesc.eType = LIGHT_DESC::TYPE_POINT;
	lightDesc.vPosition = { 750.4f, 723.4f,  267.f };
	lightDesc.fRange = 60.f;
	lightDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	lightDesc.vSpecular = { 1.f,1.f,1.f,1.f };

	volumeLightDesc.tLightDesc = lightDesc;
	volumeLightDesc.tVolumeDesc.eVolumeType = CVolumeObject::TYPE_CUBE;
	volumeLightDesc.tVolumeDesc.fCullRadius = 1000;
	volumeLightDesc.tVolumeDesc.vInnerColor = { 1.f,1.f,1.f };
	volumeLightDesc.tVolumeDesc.vOuterColor = { 1.f,0.5f,0.f };
	WorldMatrix = XMMatrixScaling(49.f, 49.f, 49.f);
	WorldMatrix *= XMMatrixTranslation(775.4f, 712.11f, 263.57f);
	XMStoreFloat4x4(&volumeLightDesc.tVolumeDesc.WorldMatrix, WorldMatrix);

	pLight = CVolumeLight::Create(TEXT("Static_Volume"), &volumeLightDesc);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Light(LightStatus::eSTATIC, pLight), E_FAIL);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	lightDesc.eType = LIGHT_DESC::TYPE_POINT;
	lightDesc.vPosition = { 915.29f,  735.66f,  315.78f };
	lightDesc.fRange = 20.f;
	lightDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	lightDesc.vSpecular = { 1.f,1.f,1.f,1.f };

	volumeLightDesc.tLightDesc = lightDesc;
	volumeLightDesc.tVolumeDesc.eVolumeType = CVolumeObject::TYPE_CONE;
	volumeLightDesc.tVolumeDesc.fCullRadius = 1000;
	volumeLightDesc.tVolumeDesc.vInnerColor = { 1.f,1.f,1.f };
	volumeLightDesc.tVolumeDesc.vOuterColor = { 1.f,1.f,1.f };
	WorldMatrix = XMMatrixScaling(4.2f, 25.f, 4.2f);
	WorldMatrix *= XMMatrixTranslation(915.29f, 735.66f, 315.78f);
	XMStoreFloat4x4(&volumeLightDesc.tVolumeDesc.WorldMatrix, WorldMatrix);

	pLight = CVolumeLight::Create(TEXT("Static_Volume"), &volumeLightDesc);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Light(LightStatus::eSTATIC, pLight), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Sky(const _tchar * pLayerTag)
{
	_uint iViewportIndex = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), &iViewportIndex), E_FAIL);
	iViewportIndex = 2;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Sky_Space"), &iViewportIndex), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_ToyBoxButton(const _tchar* pLayerTag)
{
	CToyBoxButton::TOYBOXBUTTON_DESC tToyBoxDesc;
	
	// 0
	tToyBoxDesc.iIndex = 0;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f, 0.f , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 724.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);
	
	// 1
	tToyBoxDesc.iIndex = 1;
	tToyBoxDesc.WorldMatrix = {  -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 720.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 2
	tToyBoxDesc.iIndex = 2;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 716.694f , 199.113f, 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 3
	tToyBoxDesc.iIndex = 3;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f, 0 , -1.19207e-07f , 0 , 1033.06f , 712.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 4
	tToyBoxDesc.iIndex = 4;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 708.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 5
	tToyBoxDesc.iIndex = 5;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 704.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 6
	tToyBoxDesc.iIndex = 6;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 700.694f , 199.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 7
	tToyBoxDesc.iIndex = 7;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 724.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 8
	tToyBoxDesc.iIndex = 8;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f ,720.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 9
	tToyBoxDesc.iIndex = 9;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 716.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 10
	tToyBoxDesc.iIndex = 10;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 712.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 11
	tToyBoxDesc.iIndex = 11;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f ,708.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 12
	tToyBoxDesc.iIndex = 12;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f , 704.694f , 191.113f , 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	// 13
	tToyBoxDesc.iIndex = 13;
	tToyBoxDesc.WorldMatrix = { -1.19207e-07f , 0 , 0.999984f , 0 , 0 , 0.999995f , 0 , 0 , -0.999984f , 0 , -1.19207e-07f , 0 , 1033.06f ,700.694f , 191.113f, 1 };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ToyBoxButton"), &tToyBoxDesc), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_MoonBaboonCore(const _tchar* pLayerTag)
{
	CMoonBaboonCore::MOONBABOONCORE_DESC tDesc;
	
	tDesc.iIndex = 0;
	tDesc.WorldMatrix._41 = 78.945f;
	tDesc.WorldMatrix._42 = 244.2567f;
	tDesc.WorldMatrix._43 = 158.95f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboonCore"), &tDesc), E_FAIL);

	tDesc.iIndex = 1;
	tDesc.WorldMatrix._41 = 22.008f;
	tDesc.WorldMatrix._42 = 244.2567f;
	tDesc.WorldMatrix._43 = 212.40f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboonCore"), &tDesc), E_FAIL);

	tDesc.iIndex = 2;
	tDesc.WorldMatrix._41 = 100.05f;
	tDesc.WorldMatrix._42 = 244.2567f;
	tDesc.WorldMatrix._43 = 209.95f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboonCore"), &tDesc), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_MoonBaboon_MainLaser(const _tchar* pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon_MainLaser")), E_FAIL);
	return S_OK;
}
#pragma endregion

#pragma region Yoon
HRESULT CLevel_Stage::Ready_Layer_Cody(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Cody")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_May(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_May")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Rocket(const _tchar * pLayerTag)
{
	ROBOTDESC RocketDesc;
	RocketDesc.vPosition = { 53.872f, 20.882f, 174.64f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Rocket"), &RocketDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_StarBuddy(const _tchar * pLayerTag)
{
	ROBOTDESC StarDesc;
	StarDesc.vPosition = { 63.504f, 126.751f, 226.338f, 1.f };
	StarDesc.iStageNum = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), &StarDesc), E_FAIL);

	StarDesc.vPosition = { 63.15f, 126.751f, 162.764f, 1.f };
	StarDesc.iStageNum = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), &StarDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_SecurityCameraHandle(const _tchar * pLayerTag)
{
	ROBOTDESC SecurityCamHandle;
	SecurityCamHandle.vPosition = { 57.356f, 24.812f, 180.39f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SecurityCameraHandle"), &SecurityCamHandle), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_SecurityCamera(const _tchar * pLayerTag)
{
	ROBOTDESC SecurityCam;
	SecurityCam.vPosition = { 57.356f, 24.812f, 180.39f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SecurityCamera"), &SecurityCam), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_TutorialDoor(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_TutorialDoor")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_BigButton(const _tchar * pLayerTag)
{
	ROBOTDESC BigButton;
	BigButton.vPosition = { 64.005f, 1.65499f, 70.839f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_BigButton"), &BigButton), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_BigButtonFrame(const _tchar * pLayerTag)
{
	ROBOTDESC BigButtonFrame;
	BigButtonFrame.vPosition = { 64.005f, 1.58499f, 70.839f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_BigButtonFrame"), &BigButtonFrame), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_NoBatterySign(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_SpaceValve(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE ForPlayerID;
	ForPlayerID.iPlayerValue = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), &ForPlayerID), E_FAIL);
	ForPlayerID.iPlayerValue = 2;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), &ForPlayerID), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Hook_UFO(const _tchar * pLayerTag)
{
	ROBOTDESC RobotDesc;
	RobotDesc.vPosition = { 915.313f, 740.f, 315.746f, 1.f };
	//RobotDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO"), &RobotDesc), E_FAIL);

	//RobotDesc.vPosition = { 894.939f, 735.f, 353.171f, 1.f };
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO"), &RobotDesc), E_FAIL);


	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_BigPlanet(const _tchar * pLayerTag)
{
	ROBOTDESC PlanetDesc;
	PlanetDesc.vPosition = { 997.418f, 745.831f, 218.054f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HangingPlanet"), &PlanetDesc), E_FAIL);

	PlanetDesc.vPosition = { 992.980f, 754.788f, 176.661f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HangingPlanet"), &PlanetDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_RobotParts(const _tchar * pLayerTag)
{
	ROBOTDESC RobotDesc;
	RobotDesc.iStageNum = ST_GRAVITYPATH;
	RobotDesc.vPosition = { 70.872f, 20.882f, 178.64f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotParts"), &RobotDesc), E_FAIL);

	RobotDesc.iStageNum = ST_RAIL;
	RobotDesc.vPosition = { 1035.099f, 740.8861f, 215.8869f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotParts"), &RobotDesc), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_RotatedRobotParts(const _tchar * pLayerTag)
{
	RTROBOTDESC RotatedRobotDesc;
	RotatedRobotDesc.iStageNum = ST_PINBALL;
	RotatedRobotDesc.vPosition = { -817.391077f, 793.421240f, 228.273270f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RotatedRobotParts"), &RotatedRobotDesc), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_DummyWall(const _tchar * pLayerTag)
{
	ROBOTDESC DummyWallDesc;
	DummyWallDesc.vPosition = { -805.311f, 767.083f, 189.47f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_DummyWall"), &DummyWallDesc), E_FAIL);

	DummyWallDesc.vPosition = { -805.311f, 767.083f, 197.97f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_DummyWall"), &DummyWallDesc), E_FAIL);

	DummyWallDesc.vPosition = { -805.311f, 775.083f, 193.72f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_CameraTrigger"), Level::LEVEL_STAGE, TEXT("GameObject_DummyWallCameraTrigger"), &DummyWallDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_MayJumpWall(const _tchar * pLayerTag)
{
	ROBOTDESC MayJumpWall;
	MayJumpWall.vPosition = { -815.311f - 13.f, 767.083f + 2.f, 189.9f - 0.48f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MayJumpWall"), &MayJumpWall), E_FAIL);

	MayJumpWall.vPosition = { -815.311f - 13.f, 767.083f + 2.f, 198.37f - 2.4f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MayJumpWall"), &MayJumpWall), E_FAIL);

	MayJumpWall.vPosition = { -823.88f, 768.301f, 193.517f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_MayWallCameraTrigger"), Level::LEVEL_STAGE, TEXT("GameObject_MayWallCameraTrigger"), &MayJumpWall), E_FAIL);

	return S_OK;

}
HRESULT CLevel_Stage::Ready_Layer_PipeJumpWall(const _tchar * pLayerTag)
{
	ROBOTDESC PipeJumpWall;
	PipeJumpWall.vPosition = { 44.8339f, 218.45084f, 224.48f, 1.f };
	PipeJumpWall.iStageNum = ST_GRAVITYPATH;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PipeJumpWall"), &PipeJumpWall), E_FAIL);

	PipeJumpWall.vPosition = { 46.1009f, 218.45084f, 224.48f, 1.f };
	PipeJumpWall.iStageNum = ST_PINBALL;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PipeJumpWall"), &PipeJumpWall), E_FAIL);

	PipeJumpWall.vPosition = { 45.4779f, 218.0519f, 224.223f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PipeWallCameraTrigger"), Level::LEVEL_STAGE, TEXT("GameObject_PipeWallCameraTrigger"), &PipeJumpWall), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Rope(const _tchar * pLayerTag)
{
	ROPEDESC RopeDesc;
	RopeDesc.iNumPlayer = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Rope"), &RopeDesc), E_FAIL);

	RopeDesc.iNumPlayer = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Rope"), &RopeDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_UFORadarSet(const _tchar * pLayerTag)
{
	RADARDESC RadarDesc;
	//64.0174942f, 601.063843f, 1012.77844f, 1.f
	RadarDesc.vPosition = { 64.0174942f, 601.063843f, 1012.77844f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UFORadarSet"), &RadarDesc), E_FAIL);

	/* Pixels */
	_vector vPosition = { 64.0174942f, 601.063843f + 0.3f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelChargeBar"), &vPosition), E_FAIL);

	vPosition = { 64.0174942f, 601.063843f + 0.3f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelChargeBarGauge"), &vPosition), E_FAIL);

	vPosition = { 64.0174942f, 601.063843f + 0.56f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelUFO"), &vPosition), E_FAIL);

	vPosition = { 64.0174942f, 601.063843f + 0.56f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelCrossHair"), &vPosition), E_FAIL);

	vPosition = { 64.0174942f, 601.063843f + 0.56f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelBaboon"), &vPosition), E_FAIL);

	vPosition = { 64.0174942f, 601.063843f + 0.56f, 1012.77844f - 0.29f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PixelLaser"), &vPosition), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_TestRocket(const _tchar * pLayerTag)
{
	CBoss_Missile::tagBossMissile_Desc MissileDesc;
	MissileDesc.IsTarget_Cody = true;
	MissileDesc.vPosition = { 75.f, 265.f, 207.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss_Missile", Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &MissileDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_EndingRocket(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_EndingRocket", Level::LEVEL_STAGE, TEXT("GameObject_EndingRocket")), E_FAIL);
	return S_OK;

}
#pragma endregion

#pragma region Jin
HRESULT CLevel_Stage::Ready_Layer_ControlRoomPuzzle(const _tchar * pLayerTag, const _tchar * pGameObjectTag)
{
	_uint iOption = 0;

	for (_uint i = 0; i < 2; ++i)
	{
		iOption = i;
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, pGameObjectTag, &iOption), E_FAIL);
	}

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_ControlRoom_Glass(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Glass")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Umbrella_Joystick(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam_Joystick")), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_MoonBaboon(const _tchar * pLayerTag)
{
	ROBOTDESC MoonBaboonDesc;
	MoonBaboonDesc.vPosition = { 64.f, 248.5f, 195.f, 1.f };
	//MoonBaboonDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon"), &MoonBaboonDesc), E_FAIL);
	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_UFO(const _tchar * pLayerTag)
{
	ROBOTDESC UFODesc;
	UFODesc.vPosition = { 64.f, 249.f, 195.f, 1.f };
	//UFODesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UFO"), &UFODesc), E_FAIL);
	return S_OK;
}
#pragma endregion

#pragma region Jun
HRESULT CLevel_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CCamera::CAMERA_DESC CameraDesc;
	CameraDesc.iViewportIndex = 1;
	CameraDesc.vEye = _float4(0.f, 8.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 2.f, 0.f, 1.f);
	CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovY = XMConvertToRadians(60.f);
	CameraDesc.fFullScreenAspect = (_float)g_iWinCX / (_float)g_iWinCY;
	CameraDesc.fAspect = 1.f;
	CameraDesc.fNear = 0.3f;
	CameraDesc.fFar = 450.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), &CameraDesc), E_FAIL);

	CameraDesc.iViewportIndex = 2;
	CameraDesc.vEye = _float4(0.f, 8.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), &CameraDesc), E_FAIL);

	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Performer(const _tchar * pLayerTag)
{
	CCutScenePlayer* pCutScenePlayer = CCutScenePlayer::GetInstance();
	CPerformer::PERFORMERDESC tDesc;
	tDesc.strModelTag = TEXT("Component_Model_Cody_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_May_CutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ToyBox1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_RemoteContollerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_GravityBootsCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_SizeBeltCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);
	
	tDesc.strModelTag = TEXT("Component_Model_SizeBeltRemoteControllerCutScene1");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ControlRoom_Keyboard_01_Button");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ControlRoom_Keyboard_01_Button");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ControlRoom_Keyboard_01_Button");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ControlRoom_Keyboard_01_Button");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);

	tDesc.strModelTag = TEXT("Component_Model_ControlRoom_Button_Large_01_Button");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Performer"), &tDesc), E_FAIL);


	return S_OK;
}
#pragma endregion

_int CLevel_Stage::Tick_EndingCredit(_double dTimedelta)
{
	m_dEndingCreditAccTime += dTimedelta;

	if (m_iEndingCreditStep == 0)
	{
		++m_iEndingCreditStep;

		_float fSync = 7.f;

		ENDINGCREDIT->Create_3DText(0, -792.5f + fSync);
		ENDINGCREDIT->Create_3DText(1, -1080.f + fSync);
		ENDINGCREDIT->Create_3DText(2, -1363.f + fSync);
		ENDINGCREDIT->Create_3DText(3, -1646.f + fSync);
		ENDINGCREDIT->Create_3DText(4, -1930.f + fSync);
		ENDINGCREDIT->Create_3DText(5, -2214.f + fSync);
		ENDINGCREDIT->Create_3DText(6, -2496.f + fSync);
		ENDINGCREDIT->Create_3DText(7, -2781.f + fSync);
		ENDINGCREDIT->Create_3DText(8, -3064.f + fSync);
		ENDINGCREDIT->Create_3DText(9, -3349.f + fSync);
		ENDINGCREDIT->Create_3DText(10, -3632.f + fSync);
		ENDINGCREDIT->Create_3DText(12, -3916.f + fSync);
		ENDINGCREDIT->Create_3DText(11, -4100.f + fSync);
		ENDINGCREDIT->Create_3DText(13, -4270.f + fSync);
		ENDINGCREDIT->Create_3DText(14, -4380.f + fSync);
		ENDINGCREDIT->Create_3DText(15, -4545.f + fSync);
		ENDINGCREDIT->Create_3DText(16, -4707.f + fSync);
		ENDINGCREDIT->Create_3DText(17, -4992.f + fSync);
		ENDINGCREDIT->Create_3DText(18, -5276.f + fSync);
		ENDINGCREDIT->Create_3DText(19, -5558.f + fSync);
		ENDINGCREDIT->Create_3DText(20, -5846.f + fSync);
		ENDINGCREDIT->Create_3DText(21, -6131.f + fSync);
		ENDINGCREDIT->Create_3DText(22, -6415.f + fSync);
		ENDINGCREDIT->Create_3DText(23, -6696.f + fSync);
	}

	return NO_EVENT;
}

CLevel_Stage * CLevel_Stage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_Stage* pInstance = new CLevel_Stage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Create Instance - CLevel_Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage::Free()
{
	CLevel::Free();
}
