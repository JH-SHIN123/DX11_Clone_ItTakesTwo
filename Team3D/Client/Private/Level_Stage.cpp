#include "stdafx.h"
#include "..\Public\Level_Stage.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Instancing_Env.h"
#include "Static_Env.h"
#include "Environment_Generator.h"

CLevel_Stage::CLevel_Stage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Stage::NativeConstruct()
{
	CLevel::NativeConstruct();

	FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Terrain(TEXT("Layer_Terrain")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(TEXT("Layer_Environment")), E_FAIL);

	return S_OK;
}

_int CLevel_Stage::Tick(_double dTimedelta)
{
	CLevel::Tick(dTimedelta);

	if (m_pGameInstance->Key_Down(DIK_Q))
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

	if (m_pGameInstance->Key_Down(DIK_E))
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));

	return NO_EVENT;
}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CCamera::CAMERA_DESC CameraDesc;
	CameraDesc.iViewportIndex					= 1;
	CameraDesc.vEye								= _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt								= _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY							= _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovY							= XMConvertToRadians(60.f);
	CameraDesc.fAspect							= 1.f;
	CameraDesc.fNear							= 0.3f;
	CameraDesc.fFar								= 300.f;
	CameraDesc.TransformDesc.dSpeedPerSec		= 10.f;
	CameraDesc.TransformDesc.dRotationPerSec	= XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MainCamera"), &CameraDesc), E_FAIL);

	CameraDesc.iViewportIndex					= 2;
	CameraDesc.vEye								= _float3(1.f, 8.f, -5.f);
	CameraDesc.vAt								= _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY							= _float3(0.f, 1.f, 0.f);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SubCamera"), &CameraDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Terrain")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Player(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Cody")), E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_TileBox")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->Load_Environment_Clone(pLayerTag), E_FAIL);
	return S_OK;

	//HANDLE hFile = CreateFile(L"../../Data/test19.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	//if (INVALID_HANDLE_VALUE == hFile)
	//	return E_FAIL;

	//DWORD		dwByte;
	//_uint		iNumClone = 0;
	//_tchar		szPrototypeTag[MAX_PATH] = L"";
	//_float4x4	World;

	//CInstancing_Env::INS_ENV_DESC Ins_Env_Desc;
	//CStatic_Env::STATIC_ENV_DESC  Static_Env_Desc;

	//while (true)
	//{
	//	/* Instancing Env */
	//	ReadFile(hFile, &iNumClone, sizeof(_uint), &dwByte, nullptr);
	//	for (_uint i = 0; i < iNumClone; ++i)
	//	{
	//		ReadFile(hFile, &szPrototypeTag,							 sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &Ins_Env_Desc.szModelTag,					 sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &Ins_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);

	//		Ins_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[Ins_Env_Desc.Instancing_Arg.iInstanceCount];
	//		ReadFile(hFile, Ins_Env_Desc.Instancing_Arg.pWorldMatrices,	 sizeof(_float4x4) * Ins_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

	//		Ins_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;
	//		Ins_Env_Desc.Instancing_Arg.pActorName = "TileBox";

	//		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &Ins_Env_Desc), E_FAIL);
	//	}

	//	/* Static Env */
	//	ReadFile(hFile, &iNumClone, sizeof(_int), &dwByte, nullptr);
	//	for (_uint i = 0; i < iNumClone; ++i)
	//	{
	//		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &Static_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &Static_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

	//		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &Static_Env_Desc), E_FAIL);
	//	}

	//	ReadFile(hFile, &iNumClone, sizeof(_int), &dwByte, nullptr);

	//	for (_uint i = 0; i < iNumClone; ++i)
	//	{
	//		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &World, sizeof(_float4x4), &dwByte, nullptr);
	//	}

	//	if (0 == dwByte)
	//		break;
	//}

	//CloseHandle(hFile);
	//return S_OK;
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
