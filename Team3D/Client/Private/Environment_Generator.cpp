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
#include "RotationCylinder.h"
#include "RotationFan_Base.h"
#include "RotationFan.h"
#include "Press.h"
#include "Pedal.h"
#include "RotationBox.h"
#include "ElectricBox.h"
#include "ElectricWall.h"

IMPLEMENT_SINGLETON(CEnvironment_Generator)
CEnvironment_Generator::CEnvironment_Generator()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Instancing_TXT()
{
#ifndef __MAPLOADING_OFF

	/* 인스턴싱 모델 프로토타입 생성 */
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_tchar		szNumMaterial[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;
	_uint		iLevelIndex = 0;;

	wifstream fin;
	fin.open("../Bin/Resources/Data/MapData/PrototypeData/TXT/Model_Instancing.txt");

	if (!fin.fail())
	{
		while (true)
		{
			fin.getline(szLevelIndex, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szFolderName, MAX_PATH, L'|');
			fin.getline(szNumMaterial, MAX_PATH);

			iLevelIndex = _ttoi(szLevelIndex);
			iNumMaterial = _ttoi(szNumMaterial);

			_matrix PivotMatrix = Set_Model_PivotMatrix(szPrototypeTag);
			//_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
			FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel_Instance::Create(m_pDevice, m_pDeviceContext, 1000, TEXT("../Bin/Resources/Model/Environment/Instancing/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);

			if (fin.eof())
				break;
		}
	}
	fin.close();

#endif //__MAPLOADING_OFF
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Others_TXT(_tchar * pFilePath)
{
#ifndef __MAPLOADING_OFF

	/* 모델 프로토타입 생성 */
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_tchar		szNumMaterial[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;
	_uint		iLevelIndex = 0;;

	char pFileName[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, pFilePath, MAX_PATH, pFileName, MAX_PATH, 0, 0);

	wifstream fin;
	fin.open(pFileName);

	if (!fin.fail())
	{
		while (true)
		{
			fin.getline(szLevelIndex, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szFolderName, MAX_PATH, L'|'); 
			fin.getline(szNumMaterial, MAX_PATH);

			iLevelIndex = _ttoi(szLevelIndex);
			iNumMaterial = _ttoi(szNumMaterial);

			_matrix PivotMatrix = Set_Model_PivotMatrix(szPrototypeTag);
			//_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
			FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), szFolderName, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);

			if (fin.eof())
				break;
		}
	}
	fin.close();

#endif //__MAPLOADING_OFF
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_GameObject_TXT()
{
#ifndef __MAPLOADING_OFF

	/* 객체 프로토타입 생성 */
	FAILED_CHECK_RETURN(Load_Default_Prototype_GameObject(), E_FAIL);

	_uint		iLevelIndex = 1;
	_tchar		szLevelIndex[MAX_PATH] = L"";
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szModelTag[MAX_PATH] = L"";

	wifstream fin;
	fin.open("../Bin/Resources/Data/MapData/PrototypeData/TXT/GameObject.txt");

	if (!fin.fail())
	{
		while (true)
		{
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

#endif //__MAPLOADING_OFF

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Prototype_Model_Instancing()
{
	/* 인스턴싱 모델 프로토타입 생성 */
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFilePath[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;

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

HRESULT CEnvironment_Generator::Load_Prototype_Model_Others(_tchar * pFilePath)
{
	/* 모델 프로토타입 생성 */
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;

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

HRESULT CEnvironment_Generator::Load_Prototype_GameObject()
{
	/* 객체 프로토타입 생성 */
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

HRESULT CEnvironment_Generator::Load_Prototype_Model_Others_Space(_uint iIndex)
{
#ifdef __MAPLOADING_OFF
	return S_OK;
#endif

	FAILED_CHECK_RETURN(Load_Prototype_Model_ByIndex_Space(iIndex), E_FAIL);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Stage_Space()
{
#ifndef __MAPLOADING_OFF

	FAILED_CHECK_RETURN(Load_Environment_Space(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Space_Boss(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Space_SpaceShip(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Bridge(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_Trigger(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Environment_SpaceRail(), E_FAIL);

#endif //__MAPLOADING_OFF
	return S_OK;
}

HRESULT CEnvironment_Generator::NativeConstruct_Environment_Generator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f))));
	XMStoreFloat4x4(&m_PivotMatrix_SpaceShip, XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f))));

	return S_OK;
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

CGameObject * CEnvironment_Generator::Create_Class(_tchar * pPrototypeTag, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
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
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Blocked01")) || 
			 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_Blocked02")) || 
			 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_PinBall_BlockedHalf")))
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
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_RotationCylinder")))
	{
		pInstance = CRotationCylinder::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - RotationCylinder");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_RotationFan_Base")) ||
			 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_RotationFan_Base2")))
	{
		pInstance = CRotationFan_Base::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - RotationFan_Base");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_RotationFan")))
	{
		pInstance = CRotationFan::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - CRotationFan");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_Press")) ||
			 0 == lstrcmp(pPrototypeTag, TEXT("GameObject_Press2")))
	{
		pInstance = CPress::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - Press");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_Pedal")))
	{
		pInstance = CPedal::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - Pedal");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_RotationBox")))
	{
		pInstance = CRotationBox::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - RotationBox");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_ElectricBox")))
	{
		pInstance = CElectricBox::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - ElectricBox");
	}
	else if (0 == lstrcmp(pPrototypeTag, TEXT("GameObject_ElectricWall")))
	{
		pInstance = CElectricWall::Create(pDevice, pDeviceContext);
		if (nullptr == pInstance)
			MSG_BOX("Failed to Create Instance - ElectricWall");
	}
	return pInstance;
}

_matrix CEnvironment_Generator::Set_Model_PivotMatrix(_tchar* pPrototypeTag)
{
	_matrix PivotMatrix;

	if (0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_ArrowSign_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_BatteryHolder_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Cord_01_Medium") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Cord_Big_01_Medium") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Detail_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_EjectLever_Base_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_EjectSign_01") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_EjectSign_02") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_FakeRoof_01") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_FanBase_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Grate_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_Chair_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_Panel_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_Pedal_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_PedalBase_01") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_PedalSmasher_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Interior_PedalSmasher_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorPlatform_Medium_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorPlatform_Small_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorPlatform_SmallOpen_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorPlatform_Support_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorPlatform_Thin_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorWall_Large_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorWall_Large_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorWall_MediumBars_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_InteriorWall_SmallBars_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Lamp_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Lamp_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Lamp_03") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Machinepart_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainFloor_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainFloor_Bottom_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainFloor_Grate_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainFloor_Top_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainRoof_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_MainRoof_Window_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Overheating_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_PlasticLid_01") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RoofDecoration_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RoofDecoration_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RotatingFan_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RotatingPlatform_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RotatingPlatform_02") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_Screw_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_SpinningTunnel_01") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_StickBase_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_TubeDecoration_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_SpinningTunnel_01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_SplineMesh01") || 
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_SplineMesh02") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_SplineMesh03") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_SplineMesh08") ||
		0 == lstrcmp(pPrototypeTag, L"Component_Model_Saucer_RotationBox"))
		PivotMatrix =  XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	else
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	return PivotMatrix;
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
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_Saucer_SplineMesh01"))
		tInfo.fCullRadius = 2000.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_Saucer_SplineMesh02"))
		tInfo.fCullRadius = 2000.f;
	else if (0 == lstrcmp(tInfo.szModelTag, L"Component_Model_Saucer_SplineMesh03"))
		tInfo.fCullRadius = 2000.f;
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
		ReadFile(hFile, &tDynamic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
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
	CDynamic_Env::ARG_DESC	  tDynamic_Env_Desc;

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

	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceBoss_Dynamic.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
		ReadFile(hFile, &tDynamic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iOption, sizeof(_uint), &dwByte, nullptr);

		Adjustment_Model_Position(tDynamic_Env_Desc.szModelTag, tDynamic_Env_Desc.WorldMatrix);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, szPrototypeTag, &tDynamic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Space_SpaceShip()
{
	DWORD		dwByte;
	_uint		iNumClone = 0;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szActorName[MAX_PATH] = L"";
	_float4x4	World;
	_uint		iLevelIndex = 0;

	CInstancing_Env::ARG_DESC tIns_Env_Desc;
	CStatic_Env::ARG_DESC	  tStatic_Env_Desc;
	CDynamic_Env::ARG_DESC	  tDynamic_Env_Desc;

	/* Instancing */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceShip_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceShip_Static.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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

	hFile = CreateFile(TEXT("../Bin/Resources/Data/MapData/SpaceShip_Dynamic.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
		ReadFile(hFile, &tDynamic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tDynamic_Env_Desc.iOption, sizeof(_uint), &dwByte, nullptr);

		Adjustment_Model_Position(tDynamic_Env_Desc.szModelTag, tDynamic_Env_Desc.WorldMatrix);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Boss"), Level::LEVEL_STAGE, szPrototypeTag, &tDynamic_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Bridge()
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

	/* Space Rail */
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
		ReadFile(hFile, &tDynamic_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
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

HRESULT CEnvironment_Generator::Load_Prototype_Model_ByIndex_Space(_uint iIndex)
{
	if (iIndex < 50)
	{
		if (iIndex == 10) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Alien_Screen_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Alien_Screen_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 11) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Antenna_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Antenna_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 12) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_BigButton"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("BigButton"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 13) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_BabyMonitor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_BabyMonitor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 14) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Base_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Base_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 15) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Button_Large_01_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Button_Large_01_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 16) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Button_Large_01_Button"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Button_Large_01_Button"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 17) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Cable_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Cable_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 18) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Computer_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Computer_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 19) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Door_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 20) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_DoorFrame_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_DoorFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 21) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_GraphMonitor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_GraphMonitor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 22) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Keyboard_01_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Keyboard_01_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 23) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Keyboard_01_Button"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Keyboard_01_Button"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 24) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Monitor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Monitor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 25) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planet"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planet"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 5, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 26) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saturn"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saturn"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 2, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 27) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Monitor_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Monitor_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 28) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_NoteBoard_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_NoteBoard_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 29) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Strip_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_Strip_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 30) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_StripShort_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_StripShort_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 31) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_StripTiny_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoom_StripTiny_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 32) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_Battery_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_Battery_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 33) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PipeCurve_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PipeCurve_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 34) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PipeMedium_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PipeMedium_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 35) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PipeShort_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PipeShort_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 36) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PipeTiny_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PipeTiny_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 37) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PressurePlate_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 38) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PressurePlateFrame_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlateFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 39) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_PressurePlateLock_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_PressurePlateLock_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 40) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoomPuzzle_SupportFrame_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ControlRoomPuzzle_SupportFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 41) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Desklamp_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Desklamp_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 42) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_DeskLamp_01_Arm_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("DeskLamp_01_Arm_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 43) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_DeskLamp_01_Arm_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("DeskLamp_01_Arm_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 44) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_DeskLamp_01_Assembled_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("DeskLamp_01_Assembled_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 45) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Desklamp_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Desklamp_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 46) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Desklamp_03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Desklamp_03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 47) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Desklamp_Arm_01_Medium"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Desklamp_Arm_01_Medium"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 48) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_DoorWay"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("DoorWay"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 49) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_EscapeHatch_Base_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("EscapeHatch_Base_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
	}
	else if (iIndex < 100)
	{
		if (iIndex == 50) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_EscpaeHatch_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("EscpaeHatch_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 51) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_FlashLight_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("FlashLight_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 52) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_FlashLight_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("FlashLight_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 53) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_FlashLight_02_NoGlass"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("FlashLight_02_NoGlass"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 54) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_AntennaBackGround"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_AntennaBackGround"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 55) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Antennas_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_Antennas_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 56) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Cage_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_Cage_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 57) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_CageLifter_0"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_CageLifter_0"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 58) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_DoorBlocker_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_DoorBlocker_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 59) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Star"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Generator_Star"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 60) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GlassWall_End"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GlassWall_End"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 61) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 62) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 63) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 64) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath04"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath04"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 65) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_LavaLamp"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("LavaLamp"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 66) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath05"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath05"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 67) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath06"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GravityPath06"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 68) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 69) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 70) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 71) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail04"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail04"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 72) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail05"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail05"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 73) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_GrindRail06"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("GrindRail06"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 74) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Hook_UFO"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Hook_UFO"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 75) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_HOOK_UFO_LandingGear"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("HOOK_UFO_LandingGear"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 76) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_HOOK_UFO_Toy"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("HOOK_UFO_Toy"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 77) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_HookahTube_01_End"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("HookahTube_01_End"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 78) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_BlastCrater_Cutscene"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_BlastCrater_Cutscene"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 79) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Hill_Large"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Hill_Large"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 80) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Hill_Medium"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Hill_Medium"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 81) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Hill_Small"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Hill_Small"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 82) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Pillow_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Pillow_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 83) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Moon_01_Plushie"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Moon_01_Plushie"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 84) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_Arcade_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_Arcade_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 85) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CorePillar_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 86) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Button_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CorePillar_Button_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 87) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Glass_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CorePillar_Glass_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 88) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Glass_Broken_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CorePillar_Glass_Broken_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 89) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Shield_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CorePillar_Shield_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 90) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CoreShield_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_CoreShield_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 91) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_GlassDome_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_GlassDome_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 92) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_GlassWall_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_GlassWall_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 93) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_GlassWall_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_GlassWall_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 94) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MainLaser_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_MainLaser_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 95) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MovingFloor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_MovingFloor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 96) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MovingFloor_Gate_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_MovingFloor_Gate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 97) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_ShutterDoor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_ShutterDoor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 98) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_TilingWall_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_TilingWall_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 99) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_TilingWall_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBaboon_TilingWall_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
	}
	else if (iIndex < 150)
	{
		if (iIndex == 100) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBase_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBase_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 101) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBlastCraterPaddingSmall_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonBlastCraterPaddingSmall_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 102) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonExcavator_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonExcavator_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 103) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonFlag_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonFlag_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 104) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonLander_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonLander_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 105) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonMan_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonMan_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 106) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonRover_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonRover_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 107) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonSilo_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonSilo_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 108) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_MoonToy"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("MoonToy"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 109) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planelt_Mobile_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planelt_Mobile_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 110) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planelt_Mobile_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planelt_Mobile_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 111) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planelt_Mobile_03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planelt_Mobile_03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 112) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planelt_Mobile_04"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planelt_Mobile_04"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 113) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Planet_Lamp03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Planet_Lamp03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 114) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PlanetFloor"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("PlanetFloor"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 115) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PlanetFloorRing"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("PlanetFloorRing"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 116) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_PlanetWall"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("PlanetWall"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 117) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Robot"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 118) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotBattery"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotBattery"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 119) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RobotLever"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("RobotLever"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 120) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Rocket"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 121) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_ArrowSign_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_ArrowSign_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 122) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_BatteryHolder_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_BatteryHolder_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 123) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Cord_01_Medium"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Cord_01_Medium"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 124) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Cord_Big_01_Medium"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Cord_Big_01_Medium"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 125) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Detail_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Detail_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 126) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_EjectLever_Base_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_EjectLever_Base_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 127) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_EjectSign_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_EjectSign_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 128) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_EjectSign_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_EjectSign_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 129) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_FakeRoof_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_FakeRoof_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 130) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_FanBase_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_FanBase_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 131) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Grate_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Grate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 132) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_Chair_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_Chair_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 133) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_Panel_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_Panel_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 134) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_Pedal_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_Pedal_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 135) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_PedalBase_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_PedalBase_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 136) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_PedalSmasher_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_PedalSmasher_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 137) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorPlatform_Medium_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorPlatform_Medium_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 139) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorPlatform_SmallOpen_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorPlatform_SmallOpen_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 140) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorPlatform_Support_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorPlatform_Support_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 141) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorPlatform_Thin_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorPlatform_Thin_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 142) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorWall_Large_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorWall_Large_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 143) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorWall_Large_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorWall_Large_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 144) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorWall_MediumBars_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorWall_MediumBars_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 145) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_InteriorWall_SmallBars_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_InteriorWall_SmallBars_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 146) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Lamp_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Lamp_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 147) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Lamp_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Lamp_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 148) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Lamp_03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Lamp_03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 149) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Machinepart_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Machinepart_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
	}
	else if (iIndex < 200)
	{
		if (iIndex == 150) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainFloor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainFloor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 151) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainFloor_Bottom_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainFloor_Bottom_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 152) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainFloor_Grate_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainFloor_Grate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 153) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainFloor_Top_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainFloor_Top_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 154) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainRoof_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainRoof_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 155) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_MainRoof_Window_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_MainRoof_Window_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 156) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Overheating_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Overheating_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 157) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_PlasticLid_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_PlasticLid_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 158) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RoofDecoration_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RoofDecoration_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 159) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RoofDecoration_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RoofDecoration_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 160) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RotatingFan_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RotatingFan_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 161) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RotatingPlatform_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RotatingPlatform_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 162) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RotatingPlatform_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RotatingPlatform_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 163) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Screw_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Screw_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 164) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_SpinningTunnel_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_SpinningTunnel_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 165) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_StickBase_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_StickBase_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 166) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_TubeDecoration_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_TubeDecoration_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 167) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SecurityCamera"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SecurityCamera"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 168) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SecurityCameraHandle"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SecurityCameraHandle"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 169) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Sheep_Plushie_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Sheep_Plushie_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 170) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Button_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Button_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 171) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Button_Base_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Button_Base_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 172) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Fan_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Fan_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 173) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Fan_Base_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Fan_Base_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 174) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Lever_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Lever_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 175) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Megaphone"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Megaphone"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 176) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Attachment"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Attachment"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 177) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Ball"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Ball"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 178) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_BallDoor"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_BallDoor"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 179) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_BallSpring"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_BallSpring"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 180) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_BallSpring_Handle"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_BallSpring_Handle"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 181) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_BallSpring_HandleBase"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_BallSpring_HandleBase"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 182) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 183) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_BaseStar"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_PinBall_BaseStar"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 184) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Blockade_Half"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Blockade_Half"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 185) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Blockade01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Blockade01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 186) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Blockade02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Blockade02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 187) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Frame"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Frame"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 188) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_Frame_Open"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_Frame_Open"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 189) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Plaque_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Plaque_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 190) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_SideScroller_Door_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_SideScroller_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 191) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_SideScroller_Platform_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_SideScroller_Platform_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 192) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceBowl_Door_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceBowl_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 193) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ToyBox08_WallJump"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ToyBox08_WallJump"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 194) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceBowl_Door_Wide_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceBowl_Door_Wide_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 195) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Door_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve_Door_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 196) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 197) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 198) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 199) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh04"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh04"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
	}
	else
	{
		if (iIndex == 200) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh05"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh05"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 201) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh06"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh06"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 202) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh07"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh07"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 203) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh08"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh08"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 204) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh12"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh12"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 205) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SplineMesh13"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SplineMesh13"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 206) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("StarBuddy"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 207) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy_Surprised"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("StarBuddy_Surprised"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 208) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_StuffedRabbit_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("StuffedRabbit_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 209) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_ToyBox09_Stars"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("ToyBox09_Stars"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 210) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Umbrella"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Umbrella"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 211) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_01_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 212) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_01_Stand"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Stand"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 213) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_01_Umbrella"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_01_Umbrella"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 214) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_02_Base"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_02_Base"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 215) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Marker_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("UmbrellaBeam_Marker_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 216) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Wall_SmallDoor_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Wall_SmallDoor_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 217) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 218) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_Cable_01_Medium_Collision_Variation"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_Cable_01_Medium_Collision_Variation"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 219) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_Flag_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_Flag_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 220) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_Generator_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_Generator_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 221) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_SideContent_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_SideContent_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 222) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_Star_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WarpGate_Star_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 223) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_WrapGate_Cable_01_Medium"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("WrapGate_Cable_01_Medium"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 224) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_NoBatterySign"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("NoBatterySign"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 225) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("SpaceValve"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 226) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Space_Pinball_AttachBall"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Space_Pinball_AttachBall"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 227) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_LaserButtonLargeFrame_01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("LaserButtonLargeFrame_01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 228) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Hanging_Planet"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Hanging_Planet"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 229) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Tube"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Tube"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix))), E_FAIL); }
		else if (iIndex == 230) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Interior_PedalSmasher_02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_Interior_PedalSmasher_02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 231) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_SplineMesh01"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_SplineMesh01"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 232) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_SplineMesh02"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_SplineMesh02"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 233) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_SplineMesh03"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_SplineMesh03"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
		else if (iIndex == 234) { FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_RotationBox"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Model/Environment/Others/"), TEXT("Saucer_RotationBox"), TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", 1, XMLoadFloat4x4(&m_PivotMatrix_SpaceShip))), E_FAIL); }
	}

	return S_OK;
}

void CEnvironment_Generator::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
