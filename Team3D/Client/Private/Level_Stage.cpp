#include "stdafx.h"
#include "..\Public\Level_Stage.h"
#include "Environment_Generator.h"
#include "Effect_Generator.h"
#include "InGameEffect.h"
#include "Camera.h"
#include "Loading.h"
#include "ToyBoxButton.h"
#include "MoonBaboonCore.h"

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

	//FAILED_CHECK_RETURN(Test_Layer_Object_Effect(TEXT("Layer_Object_Effect")), E_FAIL);

	/* Test */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Terrain", Level::LEVEL_STAGE, TEXT("GameObject_Terrain")), E_FAIL);

#ifndef __MAPLOADING_OFF
	/* Se */
	FAILED_CHECK_RETURN(Ready_Layer_GravityPath(TEXT("Layer_GravityPath")), E_FAIL);
	/* Jung */
	/* Hye */
	/* Won */
	FAILED_CHECK_RETURN(Ready_Layer_ToyBoxButton(TEXT("Layer_ToyBoxButton")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboonCore(TEXT("Layer_MoonBaboonCore")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon_MainLaser(TEXT("Layer_MoonBaboon_MainLaser")), E_FAIL);
	/* Yoon */
	FAILED_CHECK_RETURN(Ready_Layer_NoBatterySign(TEXT("Layer_NoBatterySign")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Rocket(TEXT("Layer_Rocket")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_StarBuddy(TEXT("Layer_StarBuddy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Robot(TEXT("Layer_Robot")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RobotHead(TEXT("Layer_RobotHead")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RobotLever(TEXT("Layer_RobotLever")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RobotBattery(TEXT("Layer_RobotBattery")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCameraHandle(TEXT("Layer_SecurityCameraHandle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCamera(TEXT("Layer_SecurityCamera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_TutorialDoor(TEXT("Layer_TutorialDoor")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigButton(TEXT("Layer_BigButton")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SpaceValve(TEXT("Layer_SpaceValve")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigPlanet(TEXT("Layer_BigPlanet")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Hook_UFO(TEXT("Layer_HookUFO")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_UFO(TEXT("Layer_UFO")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon(TEXT("Layer_MoonBaboon")), E_FAIL);
	/* Jin */
	/* Jun */

	/* For. Environment */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Stage_Space(), E_FAIL);
#endif

	return S_OK;
}

_int CLevel_Stage::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

	CEffect_Generator::GetInstance()->LoopSpawner(dTimedelta);

	return NO_EVENT;
}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

#pragma region Se
HRESULT CLevel_Stage::Ready_Layer_GravityPath(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_GravityPath")), E_FAIL);
	return S_OK;
}
#pragma endregion

#pragma region Jung
HRESULT CLevel_Stage::Test_Layer_Effect(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE Data;

	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = { 5.f,2.f,5.f,1.f };
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);

	//	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel_Portal"), &Data), E_FAIL);
	//	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel_Portal"), &Data), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Test_Layer_Object_Effect(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE Data;
	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = { 0.f,2.f,5.f,1.f };
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel"), &Data), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel_Portal"), &Data), E_FAIL);

	WorldMatrix.r[3] = { 15.f,0.f,5.f,1.f };

	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Gravity_Pipe"), &Data), E_FAIL);

	WorldMatrix.r[3] = { -5.f,2.f,5.f,1.f };
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Wormhole"), &Data), E_FAIL);

	return S_OK;
}
#pragma endregion

#pragma region Hye
#pragma endregion

#pragma region Won
HRESULT CLevel_Stage::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	pGameInstance->Reserve_Container_Light(1);

	LIGHT_DESC			LightDesc;

	/* For.Directional : Ambient / Specular Zero */
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDirection = XMFLOAT3(0.f, -1.f, 1.f);
	LightDesc.vDirection = XMFLOAT3(1.f, -1.f, 1.f);
	LightDesc.vDiffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(L"Sun", LightDesc)))
		return E_FAIL;

	/* For. Point */
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = XMFLOAT3(5.f, 5.f, 10.f);
	LightDesc.vDiffuse = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	LightDesc.fRange = 15.f;

	if (FAILED(pGameInstance->Add_Light(L"Point1", LightDesc)))
		return E_FAIL;

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

HRESULT CLevel_Stage::Ready_Layer_UFO(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UFO")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_MoonBaboon(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Rocket(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Rocket")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_StarBuddy(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Robot(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Robot")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_RobotHead(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotHead")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_RobotLever(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotLever")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_RobotBattery(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotBattery")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_SecurityCameraHandle(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SecurityCameraHandle")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_SecurityCamera(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SecurityCamera")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_TutorialDoor(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_TutorialDoor")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_BigButton(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_BigButton")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_NoBatterySign(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_NoBatterySign")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_SpaceValve(const _tchar * pLayerTag)
{
	EFFECT_DESC_CLONE a;
	a.iPlayerValue = 1;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), &a), E_FAIL);
	a.iPlayerValue = 2;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SpaceValve"), &a), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Hook_UFO(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_BigPlanet(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HangingPlanet")), E_FAIL);
	return S_OK;
}

#pragma endregion

#pragma region Jin
#pragma endregion

#pragma region Jun
HRESULT CLevel_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CCamera::CAMERA_DESC CameraDesc;
	CameraDesc.iViewportIndex = 1;
	CameraDesc.vEye = _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 2.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovY = XMConvertToRadians(60.f);
	CameraDesc.fFullScreenAspect = (_float)g_iWinCX / (_float)g_iWinCY;
	CameraDesc.fAspect = 1.f;
	CameraDesc.fNear = 0.3f;
	CameraDesc.fFar = 250.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), &CameraDesc), E_FAIL);

	CameraDesc.iViewportIndex = 2;
	CameraDesc.vEye = _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), &CameraDesc), E_FAIL);
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
