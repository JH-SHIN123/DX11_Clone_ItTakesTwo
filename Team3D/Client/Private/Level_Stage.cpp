#include "stdafx.h"
#include "..\Public\Level_Stage.h"

#pragma region Headers

/* Framework */
#include "Loading.h"
/* Se */
/* Jung */
#include "Effect_Generator.h"
#include "InGameEffect.h"
#include "WarpGate.h"
#include "Boss_Missile.h"
#include "Effect_Env_Particle_Field.h"
/* Hye */
#include "Environment_Generator.h"
#include "Dynamic_Env.h"
#include "HangingPlanet.h"
/* Taek */
#include "MoonBaboonCore.h"
#include "Light_Generator.h"
/* Yoon */
#include "RotatedRobotParts.h"
#include "RobotParts.h"
#include "ToyBoxButton.h"
/* Jin */
/* Jun */
#include "Camera.h"
#include"CutScenePlayer.h"
#include"Performer.h"

#pragma endregion

CLevel_Stage::CLevel_Stage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}
 
HRESULT CLevel_Stage::NativeConstruct()
{
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
	/* Jung */
	FAILED_CHECK_RETURN(Ready_Layer_WarpGate(TEXT("Layer_WarpGate")), E_FAIL);	
	FAILED_CHECK_RETURN(Ready_Layer_Wormhole(TEXT("Layer_Wormhole")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_WallLaserTrap(TEXT("Layer_WallLaserTrap")), E_FAIL);	
	FAILED_CHECK_RETURN(Ready_Layer_TutorialDoor(TEXT("Layer_TutorialDoor")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GravityPipe(TEXT("Layer_GravityPipe")), E_FAIL); 
	FAILED_CHECK_RETURN(Ready_Layer_Env_Particles(TEXT("Layer_Env_Particle")), E_FAIL);
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
	/* Jin */
	FAILED_CHECK_RETURN(Ready_Layer_ControlRoomPuzzle(TEXT("Layer_PressureBigPlate"), TEXT("GameObject_PressureBigPlate")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_ControlRoom_Glass(TEXT("Layer_ControlRoom_Glass")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Umbrella_Joystick(TEXT("Layer_UmbrellaBeam_Joystick")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UFO (TEXT("Layer_UFO")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon(TEXT("Layer_MoonBaboon")), E_FAIL);

	/* Jun */
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon(TEXT("Layer_MoonBaBoon")),E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UFO(TEXT("Layer_MoonBaBoon")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Performer(TEXT("Layer_Performer")), E_FAIL);
	
	/* For. Environment */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Stage_Space(), E_FAIL);
#else
	FAILED_CHECK_RETURN(Ready_Test(), E_FAIL);
#endif

	return S_OK;
}

_int CLevel_Stage::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

#ifdef __TEST_TAEK
	TCHAR lightTag[256] = L"";

	TCHAR szBuff[256] = L"";
	GetPrivateProfileString(L"Section_2", L"Key_1", L"0", szBuff, 256, L"../test.ini");
	lstrcpy(lightTag, szBuff);

	GetPrivateProfileString(L"Section_2", L"Key_2", L"0", szBuff, 256, L"../test.ini");
	_float a = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_2", L"Key_3", L"0", szBuff, 256, L"../test.ini");
	_float b = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_2", L"Key_4", L"0", szBuff, 256, L"../test.ini");
	_float c = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_2", L"Key_4", L"0", szBuff, 256, L"../test.ini");
	_float d = (_float)_wtof(szBuff);

	LIGHT_DESC* lightDesc = m_pGameInstance->Get_LightDescPtr(lightTag);

	if (lightDesc)
	{
		lightDesc->vPosition = { a,b,c };
		lightDesc->fRange = d;
	}
#endif // __TEST_TAEK

#ifdef __TEST_SE
	if (m_pGameInstance->Key_Down(DIK_M))
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Laser", Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA")), E_FAIL);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Laser", Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeB")), E_FAIL);
	}
#endif

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
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Laser", Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Laser", Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeB")), E_FAIL);
#endif 
	/* Jung */
#ifdef __TEST_JUNG
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_BossEffect", Level::LEVEL_STAGE, TEXT("GameObject_2D_Boss_Laser_Smoke")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_BossEffect", Level::LEVEL_STAGE, TEXT("GameObject_2D_Boss_Core")), E_FAIL);

	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;
	Arg_Desc.iInstanceCount = 8000;
	Arg_Desc.vPosition = { 60.f, 0.f, 30.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 10.f, 100.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);

#endif

	/* Hye */
#ifdef __TEST_HYE
	CDynamic_Env::ARG_DESC tArg;
	_matrix World = XMMatrixIdentity();
	lstrcpy(tArg.szModelTag, TEXT("Component_Model_Saucer_Interior_Pedal_01"));
	XMStoreFloat4x4(&tArg.WorldMatrix, World);
	tArg.iMaterialIndex = 0;
	tArg.iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss", Level::LEVEL_STAGE, TEXT("GameObject_Pedal"), &tArg), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss", Level::LEVEL_STAGE, TEXT("GameObject_Moon"), &tArg), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss", Level::LEVEL_STAGE, TEXT("GameObject_MoonUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss", Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon_SpaceShip")), E_FAIL);
#endif // __TEST_HYE

	/* Teak */
#ifdef __TEST_TAEK
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Terrain", Level::LEVEL_STAGE, TEXT("GameObject_Terrain")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_SpaceRail", Level::LEVEL_STAGE, TEXT("GameObject_SpaceRail")), E_FAIL);
#endif
	/* Yoon */


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
#pragma endregion

#pragma region Jung
HRESULT CLevel_Stage::Ready_Layer_WarpGate(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::MAIN_UMBRELLA)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::STAGE_UMBRELLA)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::MAIN_PLANET)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_WarpGate"), &CWarpGate::WARPGATE_DESC(CWarpGate::STAGE_PLANET)), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Wormhole(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE Data;

	_float4 vPos = { 0.f, -100.f, -1000.f, 1.f };
	memcpy(&Data.WorldMatrix.m[3][0], &vPos, sizeof(_float4));
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Wormhole"), &Data), E_FAIL);

	vPos = { 0.f, -100.f, -1500.f, 1.f };
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
HRESULT CLevel_Stage::Ready_Layer_Env_Particles(const _tchar * pLayerTag)
{
	CEffect_Env_Particle_Field::ARG_DESC Arg_Desc;
	Arg_Desc.iInstanceCount = 8000;
	Arg_Desc.vPosition = { 60.f, 0.f, 30.f, 1.f };
	Arg_Desc.vRadiusXYZ = { 30.f, 10.f, 100.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Particle", Level::LEVEL_STAGE, TEXT("GameObject_2D_Env_Particle_Field"), &Arg_Desc), E_FAIL);

	return S_OK;
}
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
#pragma endregion

#pragma region Won
HRESULT CLevel_Stage::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	pGameInstance->Reserve_Container_Light(6);

	LIGHT_DESC			LightDesc;

	/* For.Directional : Ambient / Specular Zero */
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDirection = XMFLOAT3(0.f, -1.f, 1.f);
	LightDesc.vDirection = XMFLOAT3(1.f, -1.f, 1.f);
	//LightDesc.vDiffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f);

	LightDesc.vDiffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vAmbient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(L"Sun", LightDesc)))
		return E_FAIL;

#pragma region PointLight
		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = XMFLOAT3(20.f, 5.f, 20.f);
		LightDesc.vDiffuse = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
		LightDesc.vAmbient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		LightDesc.vSpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		LightDesc.fRange = 10.f;
	if (FAILED(CLight_Generator::GetInstance()->Add_Light(TEXT("Point1"), LightDesc, (_uint)(EPoint_Color::Blue)))) return E_FAIL;
#pragma endregion

	/* For. Spot  X */
	//LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	//LightDesc.vPosition = XMFLOAT3(20, 3.f, 20.f);
	//LightDesc.vDirection = XMFLOAT3(1.f, 1.f, 0.f);
	//LightDesc.vDiffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.fOuterAngle = XMConvertToRadians(55.f);
	//LightDesc.fInnerAngle = XMConvertToRadians(45.f);
	//LightDesc.fRange = 15.f;

	//if (FAILED(pGameInstance->Add_Light(L"Spot1", LightDesc)))
	//	return E_FAIL;

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
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), &StarDesc), E_FAIL);

	StarDesc.vPosition = { 63.15f, 126.751f, 162.764f, 1.f };
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
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_MayJumpWall(const _tchar * pLayerTag)
{
	ROBOTDESC MayJumpWall;
	MayJumpWall.vPosition = { -815.311f - 13.f, 767.083f + 2.f, 189.9f - 0.48f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MayJumpWall"), &MayJumpWall), E_FAIL);

	MayJumpWall.vPosition = { -815.311f - 13.f, 767.083f + 2.f, 198.37f - 2.4f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MayJumpWall"), &MayJumpWall), E_FAIL);
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
	MoonBaboonDesc.vPosition = { 64.f, 249.5f, 195.f, 1.f };
	//MoonBaboonDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon"), &MoonBaboonDesc), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_UFO(const _tchar * pLayerTag)
{
	ROBOTDESC UFODesc;
	UFODesc.vPosition = { 64.f, 250.f, 195.f, 1.f };
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

	return S_OK;
}
#pragma endregion

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
