#include "stdafx.h"
#include "..\Public\Environment_Generator.h"
#include "Instancing_Env.h"
#include "Static_Env.h"

IMPLEMENT_SINGLETON(CEnvironment_Generator)
CEnvironment_Generator::CEnvironment_Generator()
{
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pGameInstance);
}

HRESULT CEnvironment_Generator::Load_Environment_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CEnvironment_Generator::Load_Environment_Clone(const _tchar* pLayerTag)
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
	HANDLE hFile = CreateFile(L"../../Data/Test21_Instancing.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, Ins_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szActorName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.RenderGroup, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.ShaderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Ins_Env_Desc.Instancing_Arg.iInstanceCount, sizeof(_uint), &dwByte, nullptr);

		Ins_Env_Desc.Instancing_Arg.pWorldMatrices = new _float4x4[Ins_Env_Desc.Instancing_Arg.iInstanceCount];
		ReadFile(hFile, Ins_Env_Desc.Instancing_Arg.pWorldMatrices, sizeof(_float4x4) * Ins_Env_Desc.Instancing_Arg.iInstanceCount, &dwByte, nullptr);

		Ins_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;
		Ins_Env_Desc.Instancing_Arg.pActorName = "Actor";

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Instancing_Env"), &Ins_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Static */
	hFile = CreateFile(L"../../Data/Test21_Static.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, Static_Env_Desc.szModelTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szActorName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.RenderGroup, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.ShaderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Static_Env_Desc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		Ins_Env_Desc.Instancing_Arg.fCullingRadius = 10.f;
		Ins_Env_Desc.Instancing_Arg.pActorName = "Actor";
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_Static_Env"), &Static_Env_Desc), E_FAIL);
	}
	CloseHandle(hFile);

	/* Dynamic */
	hFile = CreateFile(L"../../Data/Test21_Dynamic.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	for (_uint i = 0; i < iNumClone; ++i)
	{
		ReadFile(hFile, &iLevelIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &World, sizeof(_float4x4), &dwByte, nullptr);
	}
	CloseHandle(hFile);

	return S_OK;
}

void CEnvironment_Generator::Free()
{
	Safe_Release(m_pGameInstance);
}
