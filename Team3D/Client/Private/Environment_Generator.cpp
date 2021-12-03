#include "stdafx.h"
#include <fstream>
#include "..\Public\Environment_Generator.h"
/* Load */
#include "SavePoint.h"
#include "DeadLine.h"
#include "Bridge.h"
#include "Planet.h"
#include "PlanetRing.h"
#include "SpaceRail.h"
#include "PinBall.h"
#include "PinBall_BallDoor.h"
#include "PinBall_Spring.h"
#include "PinBall_Handle.h"
#include "PinBall_HandleBase.h"
#include "PinBall_Blocked.h"
#include "AlienScreen.h"
#include "PinBall_BallGate.h"
#include "SlideDoor.h"
#include "PinBall_Door.h"
#include "HookahTube.h"
#include "HangingPlanet.h"

IMPLEMENT_SINGLETON(CEnvironment_Generator)
CEnvironment_Generator::CEnvironment_Generator()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEnvironment_Generator::NativeConstruct_Environment_Generator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Others(_tchar * pFilePath)
{
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;

	/* Others */
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFolderName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &iNumMaterial, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Instancing()
{
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFilePath[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;

	/* Instancing Model */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/PrototypeData/Model_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFolderName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &iNumMaterial, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel_Instance::Create(m_pDevice, m_pDeviceContext, 1000, TEXT("../Bin/Resources/Model/Environment/Instancing/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_GameObject()
{
	FAILED_CHECK_RETURN(Load_Default_Prototype_GameObject(), E_FAIL);

	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szModelTag[MAX_PATH] = L"";

	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/PrototypeData/Object.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, szPrototypeTag, Create_Class(szPrototypeTag, m_pDevice, m_pDeviceContext)), E_FAIL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Stage_Space()
{
	FAILED_CHECK_RETURN(Load_Environment_Space(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Space_Boss(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Interactive_Instancing(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Trigger(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_SpaceRail(), E_FAIL);
	
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Others_TXT(_tchar * pFilePath)
{
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_tchar		szNumMaterial[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;
	_uint		iLevelIndex = 0;;

	/* Others */
	char pFileName[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, pFilePath, MAX_PATH, pFileName, MAX_PATH, 0, 0);

	wifstream fin;
	fin.open(pFileName);

	if (!fin.fail())
	{
		while (true)
		{
			// Text 파일 로드
			fin.getline(szLevelIndex, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szFolderName, MAX_PATH, L'|');
			fin.getline(szNumMaterial, MAX_PATH);

			iLevelIndex = _ttoi(szLevelIndex);
			iNumMaterial = _ttoi(szNumMaterial);

			_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
			FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);

			if (fin.eof())
				break;
		}
	}
	fin.close();

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Instancing_TXT()
{
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_tchar		szNumMaterial[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;
	_uint		iLevelIndex = 0;;

	/* Instancing Model */
	wifstream fin;
	fin.open("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Instancing.txt");

	if (!fin.fail())
	{
		while (true)
		{
			// 인스턴싱 Text 파일 로드
			fin.getline(szLevelIndex, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szFolderName, MAX_PATH, L'|');
			fin.getline(szNumMaterial, MAX_PATH);

			iLevelIndex = _ttoi(szLevelIndex);
			iNumMaterial = _ttoi(szNumMaterial);

			_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
			FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel_Instance::Create(m_pDevice, m_pDeviceContext, 1000, TEXT("../Bin/Resources/Model/Environment/Instancing/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);

			if (fin.eof())
				break;
		}
	}
	fin.close();

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_GameObject_TXT()
{
	FAILED_CHECK_RETURN(Load_Default_Prototype_GameObject(), E_FAIL);

	_uint		iLevelIndex = 1;
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szModelTag[MAX_PATH] = L"";

	/* Instancing Model */
	wifstream fin;
	fin.open("../Bin/Resources/Data/MapData/PrototypeData/TXT/GameObject.txt");

	if (!fin.fail())
	{
		while (true)
		{
			// 인스턴싱 Text 파일 로드
			fin.getline(szLevelIndex, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szModelTag, MAX_PATH);

			iLevelIndex = _ttoi(szLevelIndex);

			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, szPrototypeTag, Create_Class(szPrototypeTag, m_pDevice, m_pDeviceContext)), E_FAIL);

			if (fin.eof())
				break;
		}
	}
	fin.close();
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Default_Prototype_GameObject()
{
	/* 기본 프로토타입 생성 */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), CInstancing_Env::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), CStatic_Env::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SavePoint"), CSavePoint::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_DeadLine"), CDeadLine::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Bridge"), CBridge::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_SpaceRail"), CSpaceRail::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Hanging_Planet"), CHangingPlanet::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Space()
{
	DWORD		dwByte;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_uint		iLevelIndex = 0;

	CInstancing_Env::ARG_DESC	tIns_Env_Desc;
	CStatic_Env::ARG_DESC		tStatic_Env_Desc;
	CDynamic_Env::ARG_DESC		tDynamic_Env_Desc;

	/* Instancing */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/Space_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tIns_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);
		tIns_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[tIns_Env_Desc.Instancing_Arg.iInstanceCount];
		ReadFile(hFile, tIns_Env_Desc.Instancing_Arg.pWorldMatrices, sizeof(_float4x4) * tIns_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

		Set_Info_Model(tIns_Env_Desc);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &tIns_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Static */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/Space_Static.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tStatic_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		tStatic_Env_Desc.eGameID = GameID::Enum::eENVIRONMENT;
		Set_Info_Model(tStatic_Env_Desc);
		Adjustment_Model_Position(tStatic_Env_Desc.szModelTag, tStatic_Env_Desc.WorldMatrix);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &tStatic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Dynamic */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/Space_Dynamic.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iMatrialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iOption, sizeof(_uint), &dwByte, nullptr);

		Adjustment_Model_Position(tDynamic_Env_Desc.szModelTag, tDynamic_Env_Desc.WorldMatrix);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, szPrototypeTag, &tDynamic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Space_Boss()
{
	DWORD		dwByte;
	_uint		iNumClone = 0;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szActorName[MAX_PATH] = L"";
	_float4x4	World;
	_uint		iLevelIndex = 0;

	CInstancing_Env::ARG_DESC tIns_Env_Desc;
	CStatic_Env::ARG_DESC	  tStatic_Env_Desc;

	/* Instancing */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceBoss_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tIns_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);
		tIns_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[tIns_Env_Desc.Instancing_Arg.iInstanceCount];
		ReadFile(hFile, tIns_Env_Desc.Instancing_Arg.pWorldMatrices, sizeof(_float4x4) * tIns_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

		tIns_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Boss"), Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &tIns_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Static */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceBoss_Static.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tStatic_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tStatic_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		tStatic_Env_Desc.eGameID = GameID::Enum::eENVIRONMENT;
		Set_Info_Model(tStatic_Env_Desc);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Boss"), Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &tStatic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	///* Dynamic */
	//hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/Space_Dynamic.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	//if (INVALID_HANDLE_VALUE == hFile)
	//	return E_FAIL;

	//for (_uint i = 0; i < iNumClone; ++i)
	//{
	//	ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

	//	if (0 == dwByte)
	//		break;

	//	ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//	ReadFile(hFile, &World, sizeof(_float4x4), &dwByte, nullptr);

	//	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, szPrototypeTag, &World), E_FAIL);
	//}
	//CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Interactive_Instancing()
{
	DWORD		dwByte;
	_uint		iNumClone = 0;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szActorName[MAX_PATH] = L"";
	_float4x4	World;
	_uint		iLevelIndex = 0;

	CInstancing_Env::ARG_DESC	tIns_Env_Desc;
	CStatic_Env::ARG_DESC		tStatic_Env_Desc;

	/* Bridge */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceBridge_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tIns_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tIns_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);
		tIns_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[tIns_Env_Desc.Instancing_Arg.iInstanceCount];
		ReadFile(hFile, tIns_Env_Desc.Instancing_Arg.pWorldMatrices, sizeof(_float4x4) * tIns_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

		tIns_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Bridge"), &tIns_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_SpaceRail()
{
	DWORD		dwByte;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_uint		iLevelIndex = 0;

	CDynamic_Env::ARG_DESC		tDynamic_Env_Desc;

	// 보내준 파일 경로만 잡아주면됨
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceRail.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iMatrialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iOption, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, szPrototypeTag, &tDynamic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Trigger()
{
	DWORD		dwByte;

	CSavePoint::ARG_DESC tSavePointDesc;
	CDeadLine::ARG_DESC tDeadLineDesc;

	/* SavePoint */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/TriggerData/SavePoint.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &tSavePointDesc.eShape, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tSavePointDesc.vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &tSavePointDesc.vRotation, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &tSavePointDesc.vScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &tSavePointDesc.vSavePosition, sizeof(_float3), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment_Trigger"), Level::LEVEL_STAGE, TEXT("GameObject_SavePoint"), &tSavePointDesc), E_FAIL);
	}
	CloseHandle(hFile);

	/* DeadLine */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/TriggerData/DeadLine.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_uint iShape = 0;

	while (true)
	{
		ReadFile(hFile, &iShape, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDeadLineDesc.vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &tDeadLineDesc.vRotation, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &tDeadLineDesc.vScale, sizeof(_float3), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment_Trigger"), Level::LEVEL_STAGE, TEXT("GameObject_DeadLine"), &tDeadLineDesc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

CGameObject* CEnvironment_Generator::Create_Class(_tchar * pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGameObject* pInstance = nullptr;

	if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_Planet")) || 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_Saturn")))
	{
		pInstance = CPlanet::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - Planet");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PlanetRing")))
	{
		pInstance = CPlanetRing::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PlanetRing");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_SpaceRail")))
	{
		pInstance = CSpaceRail::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - CSpaceRail");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall")))
	{
		pInstance = CPinBall::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_BallDoor")))
	{
		pInstance = CPinBall_BallDoor::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_BallDoor");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Spring")))
	{
		pInstance = CPinBall_Spring::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_Spring");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Handle")))
	{
		pInstance = CPinBall_Handle::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_Handle");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_HandleBase")))
	{
		pInstance = CPinBall_HandleBase::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_HandleBase");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Blocked01")) || 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Blocked02")) || 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_BlockedHalf")))
	{
		pInstance = CPInBall_Blocked::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_Blocked");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_AlienScreen")))
	{
		pInstance = CAlienScreen::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - AlienScreen");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_BallGate")))
	{
		pInstance = CPinBall_BallGate::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_BallGate");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_SlideDoor")))
	{
		pInstance = CSlideDoor::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - SlideDoor");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Door")))
	{
		pInstance = CPinBall_Door::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - PinBall_Door");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_HookahTube")))
	{
		pInstance = CHookahTube::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - HookahTube");
	}
	return pInstance;
}
void CEnvironment_Generator::Set_Info_Model(CInstancing_Env::ARG_DESC & tInfo)
{
	tInfo.Instancing_Arg.fCullingRadius = 10.f;

	if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_ToyBox_Platform"))
		tInfo.Instancing_Arg.fCullingRadius = 50.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_ToyBox08_Chunk"))
		tInfo.Instancing_Arg.fCullingRadius = 50.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_ToyBox06_Wall02"))
		tInfo.Instancing_Arg.fCullingRadius = 50.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_ToyBox06_Wall01"))
		tInfo.Instancing_Arg.fCullingRadius = 50.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_ToyBox06_Wall03"))
		tInfo.Instancing_Arg.fCullingRadius = 50.f;
}

void CEnvironment_Generator::Set_Info_Model(CStatic_Env::ARG_DESC & tInfo)
{
	tInfo.fCullRadius = 10.f;

	if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail01"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail02"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail03"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail04"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail05"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_GrindRail06"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_Moon_01_Plushie"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_PlanetWall"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_PlanetFloor"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_PlanetFloorRing"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_Planet_Lamp03"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh01"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh02"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh03"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh04"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh05"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh06"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh07"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh08"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh12"))
		tInfo.fCullRadius = 500.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_SplineMesh13"))
		tInfo.fCullRadius = 500.f;
}

void CEnvironment_Generator::Adjustment_Model_Position(_tchar* pModelTag, _float4x4& rWorld)
{
	if ((0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Attachment")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Ball"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_BallDoor")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_BallSpring"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_BallSpring_Handle")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_BallSpring_HandleBase"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Base")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_BaseStar"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Blockade_Half")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Blockade01"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Blockade02")) || (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Frame"))
		|| (0 == lstrcmp(pModelTag, L"Component_Model_Space_Pinball_Frame_Open")))
		rWorld._42 -= 0.1f;
}

void CEnvironment_Generator::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
