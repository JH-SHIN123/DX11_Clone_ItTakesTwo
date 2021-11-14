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

HRESULT CEnvironment_Generator::Load_Environment_Model_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szFilePath[MAX_PATH] = L"";
	_tchar		szFolderName[MAX_PATH] = L"";
	_uint		iNumMaterial = 1;

	/* Instancing Model */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/PrototypeData/Model_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFolderName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &iNumMaterial, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel_Instance::Create(pDevice, pDeviceContext, 10000, szFilePath, szFolderName, TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);
	}
	CloseHandle(hFile);

	/* Others */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/PrototypeData/Model_Others.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szFolderName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &iNumMaterial, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, szPrototypeTag, CModel::Create(pDevice, pDeviceContext, szFilePath, szFolderName, TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique", iNumMaterial, PivotMatrix)), E_FAIL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_GameObject_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	DWORD		dwByte;
	_uint		iLevelIndex = 1;
	_tchar		szPrototypeTag[MAX_PATH] = L"";
	_tchar		szModelTag[MAX_PATH] = L"";

	/* Instancing Model */
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/PrototypeData/Object.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	/* 기본 프로토타입 생성 */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), CInstancing_Env::Create(pDevice, pDeviceContext)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), CStatic_Env::Create(pDevice, pDeviceContext)), E_FAIL);

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, szPrototypeTag, Create_Class(szPrototypeTag, pDevice, pDeviceContext)), E_FAIL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CEnvironment_Generator::Load_Environment_Clone()
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
	HANDLE hFile = CreateFile(TEXT("../Bin/Resources/Data/CloneData/Clone_Instancing.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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

		Add_Environment_Desc(Ins_Env_Desc);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &Ins_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Static */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/CloneData/Clone_Static.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, Static_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		Add_Environment_Desc(Static_Env_Desc);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &Static_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Dynamic */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/CloneData/Clone_Dynamic.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	for (_uint i = 0; i < iNumClone; ++i)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &World, sizeof(_float4x4), &dwByte, nullptr);

		//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, szPrototypeTag, &World), E_FAIL);
	}
	CloseHandle(hFile);

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

void CEnvironment_Generator::Add_Environment_Desc(CInstancing_Env::INS_ENV_DESC & EnvDesc)
{
	if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_GlassWall_Beveled"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_ALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "GlassWall_Beveled";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_GlassWall01"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_ALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "GlassWall01";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_GlassWall01_Half"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_ALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "GlassWall01_Half";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_Platform"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_Platform";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_Round01"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_Round01";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_Round02"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_Round02";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_Small"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_Small";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_SmallDouble"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_SmallDouble";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_SmallPilar"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_SmallPilar";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox_SmallQuadruple"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox_SmallQuadruple";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox01"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox01";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox02"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox02";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox03"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox03";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox04"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox04";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox05"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox05";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox06_Wall01"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox06_Wall01";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox06_Wall02"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox06_Wall02";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox06_Wall03"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox06_Wall03";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox07"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox07";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox08"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox08";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox08_Chunk"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox08_Chunk";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox08_Lid"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox08_Lid";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox08_Variation"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox08_Variation";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox08_WallJump"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox08_WallJump";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox09_Stars"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox09_Stars";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox10"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox10";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox10_Bottom"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox10_Bottom";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_ToyBox10_Lid"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.Instancing_Arg.pActorName = "ToyBox10_Lid";
		EnvDesc.Instancing_Arg.fCullingRadius = 10.f;
	}
}

void CEnvironment_Generator::Add_Environment_Desc(CStatic_Env::STATIC_ENV_DESC & EnvDesc)
{
	if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_GlassWall_End"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_ALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.pActorName = "GlassWall_End";
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_SplineMesh01"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.pActorName = "SplineMesh01";
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_SplineMesh02"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.pActorName = "SplineMesh02";
	}
	else if (0 == lstrcmp(EnvDesc.szModelTag, L"Component_Model_SplineMesh03"))
	{
		EnvDesc.RenderGroup = CRenderer::RENDER_NONALPHA;
		EnvDesc.ShaderPass = 0;
		EnvDesc.pActorName = "SplineMesh03";
	}
}

void CEnvironment_Generator::Free()
{
	Safe_Release(m_pGameInstance);
}
