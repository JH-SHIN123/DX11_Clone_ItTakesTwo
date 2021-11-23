#include "stdafx.h"
#include "..\Public\Environment_Generator.h"
/* Load */
#include "TileBox.h"

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

HRESULT CEnvironment_Generator::Load_Model_Others(_tchar * pFilePath)
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

HRESULT CEnvironment_Generator::Load_Model_Instancing()
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
}

HRESULT CEnvironment_Generator::Load_Environment_Model_Prototype()
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

	/* Others */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/PrototypeData/Model_Others.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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

HRESULT CEnvironment_Generator::Load_Environment_GameObject_Prototype()
{
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szModelTag[MAX_PATH] = L"";

	/* Instancing Model */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/PrototypeData/Object.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	/* 기본 프로토타입 생성 */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), CInstancing_Env::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), CStatic_Env::Create(m_pDevice, m_pDeviceContext)), E_FAIL);

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, szPrototypeTag, Create_Class(szPrototypeTag, m_pDevice, m_pDeviceContext)), E_FAIL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Space()
{
	DWORD		dwByte;
	_uint		iNumClone = 0;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szActorName[MAX_PATH] = L"";
	_float4x4	World;
	_uint		iLevelIndex = 0;

	CInstancing_Env::INS_ENV_DESC Ins_Env_Desc;
	CStatic_Env::STATIC_ENV_DESC  Static_Env_Desc;

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
		ReadFile(hFile, Ins_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);
		Ins_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[Ins_Env_Desc.Instancing_Arg.iInstanceCount];
		ReadFile(hFile, Ins_Env_Desc.Instancing_Arg.pWorldMatrices, sizeof(_float4x4) * Ins_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

		Ins_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;
		Ins_Env_Desc.Instancing_Arg.pMaterial = m_pGameInstance->Get_BasePxMaterial();
		Ins_Env_Desc.Instancing_Arg.pActorName = "Environment";

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &Ins_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Static */
	//hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/Space_Static.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	//if (INVALID_HANDLE_VALUE == hFile)
	//	return E_FAIL;

	//while (true)
	//{
	//	ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

	//	if (0 == dwByte)
	//		break;

	//	ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//	ReadFile(hFile, Static_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	//	ReadFile(hFile, &Static_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	//	ReadFile(hFile, &Static_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

	//	Static_Env_Desc.eGameID = GameID::Enum::eENVIRONMENT;
	//	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &Static_Env_Desc), E_FAIL);
	//}
	//CloseHandle(hFile);

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

CGameObject* CEnvironment_Generator::Create_Class(_tchar * pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGameObject* pInstance = nullptr;

	if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_DoorWay")))
	{
		pInstance = CTileBox::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - GameObject_DoorWay");
	}
	//else if ()
	//{

	//}
	return pInstance;
}

void CEnvironment_Generator::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
