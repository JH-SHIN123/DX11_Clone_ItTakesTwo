#include "stdafx.h"
#include "..\Public\Effect_Generator.h"
#include "fstream"
#include "GameInstance.h"

#include "GameEffect.h"
#include "TestEffect.h"
#include "RespawnTunnel.h"
#include "RespawnTunnel_Smoke.h"
#include "FireDoor.h"

IMPLEMENT_SINGLETON(CEffect_Generator)

HRESULT CEffect_Generator::Create_Prototype_Resource_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom")
		, CVIBuffer_PointInstance_Custom::Create(pDevice, pDeviceContext, 5000, TEXT("../Bin/ShaderFiles/Shader_PointCustom.hlsl"), "DefaultTechnique")), E_FAIL);

#pragma region Texture
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Fire_Loop_01.png"))), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01_xbox"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01_xbox.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Cloud_Texture_1k"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Cloud_Texture_1k.png"))), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_02.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_03"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_03.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_04"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_04.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_05"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_05.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_06"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_06.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_07"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_07.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp_08"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp_08.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise_02"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Noise_02.png"))), E_FAIL);



#pragma endregion


#pragma region Model
	//_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	//FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Lightning")
	//	, CModel::Create(pDevice, pDeviceContext, "../Bin/Resources/Effect/3D/Lightning/", "Lightning.fbx", TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RespawnTunnel")
	//	, CModel::Create(pDevice, pDeviceContext, "../Bin/Resources/Effect/3D/RespawnTunnel/", "RespawnTunnel.fbx", TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_RespawnTunnel_Portal")
	//	, CModel::Create(pDevice, pDeviceContext, "../Bin/Resources/Effect/3D/RespawnTunnel/", "RespawnTunnel_Portal.fbx", TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", PivotMatrix)), E_FAIL);
	//FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Model_Wormhole")
	//	, CModel::Create(pDevice, pDeviceContext, "../Bin/Resources/Effect/3D/Wormhole/", "Wormhole.fbx", TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", PivotMatrix)), E_FAIL);

#pragma  endregion
	return S_OK;
}

CEffect_Generator::CEffect_Generator()
{
}

HRESULT CEffect_Generator::Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	wifstream fin;

	fin.open(pFilePath);

	if (!fin.fail())
	{
		CGameInstance* pInstance = CGameInstance::GetInstance();
		NULL_CHECK_RETURN(pInstance, E_FAIL);


		while (true)
		{
			TCHAR NumData[MAX_PATH] = L"";
			EFFECT_DESC_PROTO* Data = new EFFECT_DESC_PROTO;

#pragma region Load Effect Data
			fin.getline(Data->EffectName, MAX_PATH, L'|');
			fin.getline(Data->TextureName, MAX_PATH, L'|');
			fin.getline(Data->TextureName_Second, MAX_PATH, L'|');
			fin.getline(Data->ModelName, MAX_PATH, L'|');

			if (fin.eof())
			{
				Safe_Delete(Data);
				break;
			}

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iInstanceCount = _ttoi(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iTextureCount_U = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->iTextureCount_V = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fUVTime_U = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fUVTime_V = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsTexFlow_U = (_ttoi(NumData) == TRUE) ? true : false;
			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsTexFlow_V = (_ttoi(NumData) == TRUE) ? true : false;

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fLifeTime = _ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fSizeTime = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fSizeChangePower = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsResizeContinue = (_ttoi(NumData) == TRUE) ? true : false;
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.z = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fColorChangePower = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.w = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.w = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsGravity = (_ttoi(NumData) == TRUE) ? true : false;

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iShaderPass = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->iLevelIndex = _ttoi(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsRePosAll = (_ttoi(NumData) == TRUE) ? true : false;
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fDirMoveTime = _ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fDirMoveSpeed = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fRenderTerm = _ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fInstancePosUpdateTerm = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.z = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotRotate_Degree.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotRotate_Degree.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH);
			Data->vPivotRotate_Degree.z = (_float)_ttof(NumData);
#pragma endregion
			
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

				FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, Data->ModelName
					, CModel::Create(pDevice, pDeviceContext, pPath, szPrototypeName, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique"
						, Compute_Pivot(XMLoadFloat3(&Data->vPivotScale), XMLoadFloat3(&Data->vPivotRotate_Degree)))), E_FAIL);
			}


			Create_Prototype(Data->iLevelIndex, Data->EffectName, pDevice, pDeviceContext, Data);
		}

		fin.close();
	}





	return S_OK;
}

HRESULT CEffect_Generator::Create_Prototype(_uint iLevelIndex, const _tchar * pPrototypeName, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EFFECT_DESC_PROTO* pData)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, pPrototypeName, CRespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel_Portal"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, pPrototypeName, CRespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_RespawnTunnel_Smoke"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, pPrototypeName, CRespawnTunnel_Smoke::Create(pDevice, pDeviceContext, pData));

	else if(0 == lstrcmp(pPrototypeName, L"GameObject_2D_FireDoor"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, pPrototypeName, CFireDoor::Create(pDevice, pDeviceContext, pData));

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
}
