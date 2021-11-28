#include "stdafx.h"
#include "..\Public\Level_Stage.h"
#include "Environment_Generator.h"
#include "Effect_Generator.h"
#include "InGameEffect.h"
#include "Camera.h"

#include "WarpGate.h"
#include "Boss_Missile.h"

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

	/* Se */
	FAILED_CHECK_RETURN(Ready_Layer_GravityPath(TEXT("Layer_GravityPath")), E_FAIL);
	/* Jung */
	FAILED_CHECK_RETURN(Ready_Layer_WarpGate(TEXT("Layer_WarpGate")), E_FAIL);	
	FAILED_CHECK_RETURN(Ready_Layer_Wormhole(TEXT("Layer_Wormhole")), E_FAIL);
	/* Hye */
	/* Won */
	/* Yoon */
	//FAILED_CHECK_RETURN(Ready_Layer_NoBatterySign(TEXT("Layer_NoBatterySign")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Rocket(TEXT("Layer_Rocket")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_StarBuddy(TEXT("Layer_StarBuddy")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Robot(TEXT("Layer_Robot")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_RobotHead(TEXT("Layer_RobotHead")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_RobotLever(TEXT("Layer_RobotLever")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_RobotBattery(TEXT("Layer_RobotBattery")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_SecurityCameraHandle(TEXT("Layer_SecurityCameraHandle")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_SecurityCamera(TEXT("Layer_SecurityCamera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_TutorialDoor(TEXT("Layer_TutorialDoor")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_BigButton(TEXT("Layer_BigButton")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_SpaceValve(TEXT("Layer_SpaceValve")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_BigPlanet(TEXT("Layer_BigPlanet")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Hook_UFO(TEXT("Layer_HookUFO")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_UFO(TEXT("Layer_UFO")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_MoonBaboon(TEXT("Layer_MoonBaboon")), E_FAIL);
	/* Jin */
	/* Jun */


	/* For. Environment */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Space(), E_FAIL);
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Space_Boss(), E_FAIL);
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Interactive_Instancing(), E_FAIL);

	return S_OK;
}

_int CLevel_Stage::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

	if (m_pGameInstance->Key_Down(DIK_C))
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Boss_Missile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &CBoss_Missile::tagBossMissile_Desc(true));

	return NO_EVENT;
}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

#pragma region Se
HRESULT CLevel_Stage::Ready_Layer_GravityPath(const _tchar * pLayerTag)
{
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_GravityPath")), E_FAIL);
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
	LightDesc.vDiffuse = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
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
