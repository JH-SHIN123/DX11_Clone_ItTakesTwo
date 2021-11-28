#include "stdafx.h"
#include "..\Public\Level_Stage.h"
#include "GameInstance.h"
#include "Environment_Generator.h"
#include "Camera.h"

#include "InGameEffect.h"
#include "Effect_Generator.h"
#include "RobotParts.h"

CLevel_Stage::CLevel_Stage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage::NativeConstruct()
{
	CLevel::NativeConstruct();

	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Terrain(TEXT("Layer_Terrain")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Sky(TEXT("Layer_Sky")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Cody(TEXT("Layer_Cody")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_May(TEXT("Layer_May")), E_FAIL);

	FAILED_CHECK_RETURN(Test_Layer_Object_Effect(TEXT("Layer_Object_Effect")), E_FAIL);

	/* For.Interactive Objects */
	FAILED_CHECK_RETURN(Ready_Layer_Rocket(TEXT("Layer_Rocket")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_StarBuddy(TEXT("Layer_StarBuddy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigButton(TEXT("Layer_BigButton")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RobotParts(TEXT("Layer_RobotParts")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCameraHandle(TEXT("Layer_SecurityCameraHandle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SecurityCamera(TEXT("Layer_SecurityCamera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SpaceValve(TEXT("Layer_SpaceValve")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BigPlanet(TEXT("Layer_BigPlanet")), E_FAIL);


	//FAILED_CHECK_RETURN(Ready_Layer_TutorialDoor(TEXT("Layer_TutorialDoor")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Hook_UFO(TEXT("Layer_HookUFO")), E_FAIL);



	/* For. Environment */
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Space(), E_FAIL);
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Space_Boss(), E_FAIL);
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Interactive_Instancing(), E_FAIL);

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


HRESULT CLevel_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CCamera::CAMERA_DESC CameraDesc;
	CameraDesc.iViewportIndex					= 1;
	CameraDesc.vEye								= _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt								= _float3(0.f, 2.f, 0.f);
	CameraDesc.vAxisY							= _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovY							= XMConvertToRadians(60.f);
	CameraDesc.fFullScreenAspect				= (_float)g_iWinCX / (_float)g_iWinCY;
	CameraDesc.fAspect							= 1.f;
	CameraDesc.fNear							= 0.3f;
	CameraDesc.fFar								= 250.f;
	CameraDesc.TransformDesc.dSpeedPerSec		= 10.f;
	CameraDesc.TransformDesc.dRotationPerSec	= XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), &CameraDesc), E_FAIL);

	CameraDesc.iViewportIndex					= 2;
	CameraDesc.vEye								= _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt								= _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY							= _float3(0.f, 1.f, 0.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), &CameraDesc), E_FAIL);
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

HRESULT CLevel_Stage::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Terrain")), E_FAIL);
	return S_OK;
}

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

	WorldMatrix.r[3] = { 62.9901505f, 35.f, 195.674637f,1.f };
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Gravity_Pipe"), &Data), E_FAIL);	
	
	WorldMatrix.r[3] = { -5.f,2.f,5.f,1.f };
	XMStoreFloat4x4(&Data.WorldMatrix, WorldMatrix);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_3D_Wormhole"), &Data), E_FAIL);

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
	ROBOTDESC RocketDesc;
	RocketDesc.vPosition = { 53.872f, 20.882f, 174.64f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Rocket"), &RocketDesc), E_FAIL);
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_StarBuddy(const _tchar * pLayerTag)
{
	ROBOTDESC StarDesc;
	StarDesc.vPosition = {34.998f, 127.441216f, 170.7718f, 1.f};
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy"), &StarDesc), E_FAIL);

	StarDesc.vPosition = { 102.0644f, 127.4629f, 195.2063f, 1.f };
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
	BigButton.vPosition = { 64.005f, 0.68499f, 70.839f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_BigButton"), &BigButton), E_FAIL);
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
	ROBOTDESC RobotDesc;
	RobotDesc.vPosition = { 70.872f, 20.882f, 178.64f, 1.f };

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_HookUFO")), E_FAIL);
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
	RobotDesc.vPosition = { 125.f, 0.f, 60.f, 1.f };
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_RobotParts"), &RobotDesc), E_FAIL);


	return S_OK;
}
HRESULT CLevel_Stage::Ready_Layer_Test()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Test"), Level::LEVEL_STAGE, TEXT("GameObject_TestObject01")), E_FAIL);
	return S_OK;
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
