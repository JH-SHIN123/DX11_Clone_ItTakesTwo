#include "stdafx.h"
#include "..\Public\Effect_Generator.h"
#include "GameInstance.h"
#include "Effect_Generator.h"

#pragma  region Effect Include
#include "TestEffect.h"
#include "Effect_RespawnTunnel.h"
#include "Effect_FireDoor.h"
#include "Effect_Walking_Smoke.h"
#include "Effect_Landing_Smoke.h"
#include "Effect_Dash.h"
#include "Effect_Player_Dead.h"
#include "Effect_Player_Dead_Particle.h"
#include "Effect_Player_Revive.h"
#include "Effect_Cody_Size.h"
#include "Effect_RespawnTunnel_Portal.h"
#include "Effect_May_Boots.h"
#include "Effect_May_Boots_Walking_Particle.h"
#include "Effect_GravityPipe.h"
#include "Effect_Wormhole.h"
#include "Effect_Env_Particle.h"
#include "Effect_Dead_Particle_Fire.h"
#include "Effect_Robot_Battery_Spark.h"
#pragma endregion

IMPLEMENT_SINGLETON(CEffect_Generator)


CEffect_Generator::CEffect_Generator()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Generator::Add_Effect(Effect_Value eEffect, _fmatrix WorldMatrix, void* pArg)
{
	if (Effect_Value::Effect_Value_End <= eEffect)
		return E_FAIL;
	
	EFFECT_DESC_CLONE Clone_Data;

	_tchar szLayer[MAX_PATH]		= L"Layer_Effect";
	_tchar szPrototype[MAX_PATH]	= L"";

	XMStoreFloat4x4(&Clone_Data.WorldMatrix, WorldMatrix);
	Clone_Data.pArg = pArg;

	switch (eEffect)
	{
	case Effect_Value::Walking_Smoke:
		Clone_Data.vDir = {0.f, 1.f, 0.f};
		Clone_Data.UVTime = 0.01;
		Clone_Data.fSizePower = 0.7f;
		Clone_Data.vSize_Max = { 2.f, 2.f, 0.f };
		Clone_Data.vRandDirPower = { 5.f, 100.f, 5.f };
		Clone_Data.IsRandDirDown[1] = false;
		lstrcpy(szPrototype, L"GameObject_2D_Walking_Smoke");
		break;
	case Effect_Value::Landing_Smoke:
		Clone_Data.UVTime = 0.001;
		lstrcpy(szPrototype, L"GameObject_2D_Landing_Smoke");
		break;
	case Effect_Value::Dash:
		lstrcpy(szPrototype, L"GameObject_2D_Dash");
		break;
	case Effect_Value::Cody_Dead:
		Clone_Data.iPlayerValue = Check_Cody_Size(WorldMatrix);
		lstrcpy(szPrototype, L"GameObject_2D_Player_Dead_Particle");
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, szLayer, Level::LEVEL_STAGE, L"GameObject_2D_Player_Dead", &Clone_Data);
		break;
	case Effect_Value::Cody_Dead_Fire:
		Clone_Data.iPlayerValue = Check_Cody_Size(WorldMatrix);
		lstrcpy(szPrototype, L"GameObject_2D_Player_Dead_Particle_Fire");
		break;
	case Effect_Value::Cody_Revive:
		Clone_Data.iPlayerValue = Check_Cody_Size(WorldMatrix);
		lstrcpy(szPrototype, L"GameObject_2D_Player_Revive");
		break;
	case Effect_Value::May_Dead:
		Clone_Data.iPlayerValue = EFFECT_DESC_CLONE::PV_MAY;
		lstrcpy(szPrototype, L"GameObject_2D_Player_Dead_Particle");
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, szLayer, Level::LEVEL_STAGE, L"GameObject_2D_Player_Dead", &Clone_Data);
		break;
	case Effect_Value::May_Dead_Fire:
		Clone_Data.iPlayerValue = EFFECT_DESC_CLONE::PV_MAY;
		lstrcpy(szPrototype, L"GameObject_2D_Player_Dead_Particle_Fire");
		break;
	case Effect_Value::May_Revive:
		Clone_Data.iPlayerValue = EFFECT_DESC_CLONE::PV_MAY;
		lstrcpy(szPrototype, L"GameObject_2D_Player_Revive");
		break;
	case Effect_Value::May_Boots_Walking:
		lstrcpy(szPrototype, L"GameObject_2D_May_Boots_Walking_Particle");
		break;
	case Effect_Value::RobotBattery_Spark:
		lstrcpy(szPrototype, L"GameObject_2D_Robot_Battery_Spark");
		break;
	default:
		break;
	}

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, szLayer, Level::LEVEL_STAGE, szPrototype, &Clone_Data);

	return S_OK;
}

EFFECT_DESC_CLONE::PLAYER_VALUE CEffect_Generator::Check_Cody_Size(_fmatrix WorldMatrix)
{
	_float fScale = (XMVector3Length(WorldMatrix.r[0])).m128_f32[0];

	if (0.f <= fScale && fScale < 0.8f)
		return EFFECT_DESC_CLONE::PV_CODY_S;
	else if (0.8f <= fScale && fScale < 2.5f)
		return EFFECT_DESC_CLONE::PV_CODY;
	else if (2.5f <= fScale && fScale <= 5.f)
		return EFFECT_DESC_CLONE::PV_CODY_L;

	return EFFECT_DESC_CLONE::PV_CODY;
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
			// ¼öÁ¤
			wstring	 wstrName = Data->ModelName;
			wstrName.erase(0, 16);
			_tchar szPrototypeName[MAX_PATH] = L"";
			lstrcat(szPrototypeName, wstrName.c_str());

			FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Data->iLevelIndex, Data->ModelName
				, CModel::Create(pDevice, pDeviceContext, TEXT("../Bin/Resources/Effect/3D/")
					, szPrototypeName, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", 1
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
	if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_FireDoor"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_FireDoor",						CEffect_FireDoor::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Walking_Smoke"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Walking_Smoke",				CEffect_Walking_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Landing_Smoke"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Landing_Smoke",				CEffect_Landing_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Dash"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Dash",							CEffect_Dash::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead",					CEffect_Player_Dead::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead_Particle"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead_Particle",			CEffect_Player_Dead_Particle::Create(pDevice, pDeviceContext, pData));
	
	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead_Particle_Fire"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead_Particle_Fire",	CEffect_Player_Dead_Particle_Fire::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Revive"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Revive",				CEffect_Player_Revive::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Cody_Size"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Cody_Size",					CEffect_Cody_Size::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_May_Boots"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_May_Boots",					CEffect_May_Boots::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_May_Boots_Walking_Particle"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_May_Boots_Walking_Particle",	CEffect_May_Boots_Walking_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Env_Particle"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Env_Particle",					CEffect_Env_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Robot_Battery_Spark"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Robot_Battery_Spark",			CEffect_Robot_Battery_Spark::Create(pDevice, pDeviceContext, pData));


	// 3D Effect
	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel", CEffect_RespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel_Portal"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel_Portal", CEffect_RespawnTunnel_Portal::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Wormhole"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Wormhole", CEffect_Wormhole::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Gravity_Pipe"))
		pInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Gravity_Pipe", CEffect_GravityPipe::Create(pDevice, pDeviceContext, pData));


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

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV")
		, CVIBuffer_Rect_TripleUV::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl"), "DefaultTechnique")), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom")
		, CVIBuffer_PointInstance_Custom_ST::Create(pDevice, pDeviceContext, 10000, TEXT("../Bin/ShaderFiles/Shader_PointCustom.hlsl"), "DefaultTechnique")), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT")
		, CVIBuffer_PointInstance_Custom_STT::Create(pDevice, pDeviceContext, 10000, TEXT("../Bin/ShaderFiles/Shader_PointCustom_STT.hlsl"), "DefaultTechnique")), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_RectInstance_Custom")
		, CVIBuffer_RectInstance_Custom::Create(pDevice, pDeviceContext, 100, TEXT("../Bin/ShaderFiles/Shader_RectCustom.hlsl"), "DefaultTechnique")), E_FAIL);

#pragma region Texture
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp/Color_Ramp_%d.png"), 13)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01_xbox"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01_xbox.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Cloud_Texture_1k"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Cloud_Texture_1k.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_ligntning_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/ligntning_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Fire_Loop/Fire_Loop_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Noise/Tilling_Noise_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Cloud/Tilling_Cloud_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Ribbon_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Ribbon_Noise/Ribbon_Noise_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_CartoonTrail"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/CartoonTrail_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Gradient_Noise"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Gradient_Noise/Gradient_Noise_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Circle_Alpha.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_distortion"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/distortion_01_E2.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Dead_Cells"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Custom/Dead_Cells.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_DecalSplat"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Mask_Texture/T_DecalSplat_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_flowmaptest"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/flowmaptest_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Wormhole_Noise"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Wormhole_Noise/Wormhole_Noise_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Level_Preview"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Level_Preview/%d.png"), 7)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Slime_Cloud"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Shockwave_02_E1.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Dot"),				CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Custom/Dot.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Fire_Tiled"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/T_Fire_Tiled.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_SoftCLoud"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/SoftCLoud_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion7x7"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Explosion/Explosion7x7_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion8x8"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Explosion/Explosion8x8_%d.png"), 2)), E_FAIL);
#pragma endregion

	return S_OK;
}

void CEffect_Generator::LoopSpawner(_double TimeDelta)
{
#ifdef _DEBUG
	m_dSpawnTerm -= TimeDelta;

	if (0.0 >= m_dSpawnTerm)
	{
		m_dSpawnTerm = 5.0;

		_matrix WorldMatrix = XMMatrixIdentity();
		
		WorldMatrix.r[3] = { 5.f, 0.f, 5.f, 1.f };
		//Add_Effect(Effect_Value::Cody_DeadEffect, WorldMatrix);
	}
#endif // _DEBUG
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
