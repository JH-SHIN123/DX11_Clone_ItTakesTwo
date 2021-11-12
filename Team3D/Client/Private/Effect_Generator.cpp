#include "stdafx.h"
#include "..\Public\Effect_Generator.h"
#include "fstream"
#include "GameInstance.h"

#include "GameEffect.h"
#include "TestEffect.h"
#include "RespawnTunnel.h"
#include "RespawnTunnel_Smoke.h"
#include "FireDoor.h"
#include "Walking_Smoke.h"

IMPLEMENT_SINGLETON(CEffect_Generator)


CEffect_Generator::CEffect_Generator()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Generator::Add_Effect(Effect_Value eEffect, _fvector vPosition)
{
	if (Effect_Value::Effect_Value_End <= eEffect)
		return E_FAIL;
	
	EFFECT_DESC_CLONE Clone_Data;

	_tchar szLayer[MAX_PATH] = L"";
	_tchar szPrototype[MAX_PATH] = L"";


	switch (eEffect)
	{
	case Client::Effect_Value::Walking_Smoke:
		Clone_Data.vDir = {0.f, 1.f, 0.f};
		Clone_Data.UVTime = 0.01;
		Clone_Data.fSizePower = 0.7f;
		Clone_Data.vSize_Max = { 2.f, 2.f, 0.f };
		Clone_Data.vRandDirPower = { 5.f, 100.f, 5.f };
		Clone_Data.IsRandDirDown[1] = false;
		XMStoreFloat4(&Clone_Data.vPos, vPosition);
		lstrcpy(szLayer, L"Layer_Effect");
		lstrcpy(szPrototype, L"GameObject_2D_Walking_Smoke");
		break;
	default:
		break;
	}

	m_pGameInstance->Add_GameObject_Clone(1, szLayer, 1, szPrototype, &Clone_Data);

	return S_OK;
}

HRESULT CEffect_Generator::Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to LoadData UI");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = 0;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	while (true)
	{
		EFFECT_DESC_PROTO* Data = new EFFECT_DESC_PROTO;

		ReadFile(hFile, Data, sizeof(EFFECT_DESC_PROTO), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(Data);
			break;
		}

		if (1 < lstrlen(Data->ModelName))
		{
			wstring	 wstrName = Data->ModelName;
			wstrName.erase(0, 16);
			char szPrototypeName[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, wstrName.c_str(), MAX_PATH, szPrototypeName, MAX_PATH, NULL, NULL);

			char pPath[MAX_PATH] = "../Bin/Resources/Effect/3D/";
			strcat_s(pPath, MAX_PATH, szPrototypeName);
			strcat_s(pPath, MAX_PATH, "/");

			strcat_s(szPrototypeName, MAX_PATH, ".fbx");

			FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Data->iLevelIndex, Data->ModelName
				, CModel::Create(pDevice, pDeviceContext, pPath, szPrototypeName, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique"
					, Compute_Pivot(XMLoadFloat3(&Data->vPivotScale), XMLoadFloat3(&Data->vPivotRotate_Degree)))), E_FAIL);
		}


		Create_Prototype(Data->iLevelIndex, Data->EffectName, pDevice, pDeviceContext, Data);
		Safe_Delete(Data);
	}


	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Generator::Create_Prototype(_uint iLevelIndex, const _tchar * pPrototypeName, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EFFECT_DESC_PROTO* pData)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	// 2D Effect
	if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_RespawnTunnel_Smoke"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_RespawnTunnel_Smoke", CRespawnTunnel_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_FireDoor"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_FireDoor", CFireDoor::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Walking_Smoke"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Walking_Smoke", CWalking_Smoke::Create(pDevice, pDeviceContext, pData));

	// 3D Effect
	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel", CRespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel_Portal"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel_Portal", CRespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else
	{
		_tchar szWarning[MAX_PATH] = L"";
		lstrcat(szWarning, pPrototypeName);
		MessageBox(g_hWnd, szWarning, L"Load Error", MB_OK);
		Safe_Delete(pData);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Generator::Create_Prototype_Resource_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom")
		, CVIBuffer_PointInstance_Custom::Create(pDevice, pDeviceContext, 5000, TEXT("../Bin/ShaderFiles/Shader_PointCustom.hlsl"), "DefaultTechnique")), E_FAIL);

#pragma region Texture
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01_xbox"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01_xbox.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Cloud_Texture_1k"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Cloud_Texture_1k.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_ligntning_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/ligntning_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Fire_Loop/Fire_Loop_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp/Color_Ramp_%d.png"), 10)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Noise/Tilling_Noise_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Cloud/Tilling_Cloud_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Ribbon_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Ribbon_Noise/Ribbon_Noise_0%d.png"), 2)), E_FAIL);





#pragma endregion


#pragma region Model


#pragma  endregion
	return S_OK;
}

_fmatrix CEffect_Generator::Compute_Pivot(_vector vScale, _vector vRotate)
{
	_matrix vPivot = XMMatrixIdentity();;

	vPivot = XMMatrixScalingFromVector(vScale);

	vPivot *= XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(vRotate.m128_f32[0]),
		XMConvertToRadians(vRotate.m128_f32[1]),
		XMConvertToRadians(vRotate.m128_f32[2]));

	return vPivot;
}

void CEffect_Generator::Free()
{
	Safe_Release(m_pGameInstance);
}
