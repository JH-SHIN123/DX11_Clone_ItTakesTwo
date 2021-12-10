#include "stdafx.h"
#include "..\Public\Effect_Generator.h"
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
#include "Effect_Umbrella_Pipe.h"
#include "Effect_Pinball_Move.h"
#include "Effect_Boss_Laser_Smoke.h"
#include "Effect_PointLight.h"
#include "Effect_Boss_Laser_Particle.h"
#include "Effect_Boss_Core.h"
#include "Effect_Boss_Core_Hit.h"
#include "Effect_Boss_Core_Smoke.h"
#include "Effect_Boss_Core_Explosion.h"
#include "Effect_Env_Particle_Field.h"
#include "Effect_Boss_Core_Lightning.h"
#include "Effect_Boss_Core_Lightning_Big.h"
#include "Effect_Boss_Gravitational_Bomb.h"
#include "Effect_Boss_Gravitational_Bomb_Pillar.h"
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
	case Effect_Value::BossCore_Hit:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Core_Hit");
		break;
	case Effect_Value::BossCore_Smoke:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Core_Smoke");
		break;
	case Effect_Value::BossCore_Explosion:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Core_Explosion");
		break;
	case Effect_Value::BossCore_Lightning:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Core_Lightning");
		break;
	case Effect_Value::BossCore_Lightning_Big:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Core_Lightning_Big");
		break;
	case Effect_Value::BossBomb:
		lstrcpy(szPrototype, L"GameObject_3D_Boss_Gravitational_Bomb");
		break;
	case Effect_Value::BossBomb_Pillar:
		lstrcpy(szPrototype, L"GameObject_2D_Boss_Gravitational_Bomb_Pillar");
		break;
	default:
		break;
	}

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, szLayer, Level::LEVEL_STAGE, szPrototype, &Clone_Data);
	
	return S_OK;
}

HRESULT CEffect_Generator::Add_PointLight(Effect_PointLight_Desc* pLightArg)
{
	if (nullptr == pLightArg)
		return E_FAIL;

	return 	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect_PointLight"), Level::LEVEL_STAGE, TEXT("GameObject_2D_PointLight"), pLightArg);	
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

// 	_float3 vScale = { 0.002f, 0.002f, 0.002f };
// 	_float3 vRotate = { -90.f, 0.f, 0.f };
// 
// 	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE,TEXT("Component_Model_Boss_Gravitational_Bomb")
// 		, CModel::Create(pDevice, pDeviceContext, TEXT("../Bin/Resources/Effect/3D/")
// 			, TEXT("Boss_Gravitational_Bomb"), TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), "DefaultTechnique", 1
// 			, Compute_Pivot(XMLoadFloat3(&vScale), XMLoadFloat3(&vRotate)))), E_FAIL);
// 
// 	EFFECT_DESC_PROTO Data;
// 	lstrcpy(Data.ModelName ,		 TEXT("Component_Model_Boss_Gravitational_Bomb"));
// 	lstrcpy(Data.TextureName,		 TEXT("Component_Texture_Tilling_Cloud"));
// 	lstrcpy(Data.TextureName_Second, TEXT(""));
// 
// 	m_pGameInstance->Add_GameObject_Prototype(Level::LEVEL_STAGE, L"GameObject_3D_Boss_Gravitational_Bomb", CEffect_Boss_Gravitational_Bomb::Create(pDevice, pDeviceContext, &Data));

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Generator::Create_Prototype(_uint iLevelIndex, const _tchar * pPrototypeName, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EFFECT_DESC_PROTO* pData)
{
	// 2D Effect
	if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_FireDoor"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_FireDoor", CEffect_FireDoor::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Walking_Smoke"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Walking_Smoke", CEffect_Walking_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Landing_Smoke"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Landing_Smoke", CEffect_Landing_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Dash"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Dash", CEffect_Dash::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead", CEffect_Player_Dead::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead_Particle"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead_Particle", CEffect_Player_Dead_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Dead_Particle_Fire"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Dead_Particle_Fire", CEffect_Player_Dead_Particle_Fire::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Player_Revive"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Player_Revive", CEffect_Player_Revive::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Cody_Size"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Cody_Size", CEffect_Cody_Size::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_May_Boots"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_May_Boots", CEffect_May_Boots::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_May_Boots_Walking_Particle"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_May_Boots_Walking_Particle", CEffect_May_Boots_Walking_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Env_Particle"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Env_Particle", CEffect_Env_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Robot_Battery_Spark"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Robot_Battery_Spark", CEffect_Robot_Battery_Spark::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Pinball_Move"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Pinball_Move", CEffect_Pinball_Move::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_PointLight"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_PointLight", CEffect_PointLight::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Laser_Smoke"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Laser_Smoke", CEffect_Boss_Laser_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Laser_Particle"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Laser_Particle", CEffect_Boss_Laser_Particle::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core", CEffect_Boss_Core::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core_Hit"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core_Hit", CEffect_Boss_Core_Hit::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core_Smoke"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core_Smoke", CEffect_Boss_Core_Smoke::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Env_Particle_Field"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Env_Particle_Field", CEffect_Env_Particle_Field::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core_Explosion"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core_Explosion", CEffect_Boss_Core_Explosion::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core_Lightning"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core_Lightning", CEffect_Boss_Core_Lightning::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Core_Lightning_Big"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Core_Lightning_Big", CEffect_Boss_Core_Lightning_Big::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_2D_Boss_Gravitational_Bomb_Pillar"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_2D_Boss_Gravitational_Bomb_Pillar", CEffect_Boss_Gravitational_Bomb_Pillar::Create(pDevice, pDeviceContext, pData));


	// 3D Effect
	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel", CEffect_RespawnTunnel::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_RespawnTunnel_Portal"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_RespawnTunnel_Portal", CEffect_RespawnTunnel_Portal::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Wormhole"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Wormhole", CEffect_Wormhole::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Gravity_Pipe"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Gravity_Pipe", CEffect_GravityPipe::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Umbrella_Pipe"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Umbrella_Pipe", CEffect_Umbrella_Pipe::Create(pDevice, pDeviceContext, pData));

	else if (0 == lstrcmp(pPrototypeName, L"GameObject_3D_Boss_Gravitational_Bomb"))
		m_pGameInstance->Add_GameObject_Prototype(iLevelIndex, L"GameObject_3D_Boss_Gravitational_Bomb", CEffect_Boss_Gravitational_Bomb::Create(pDevice, pDeviceContext, pData));

	else
	{
#ifdef __TEST_JUNG
		_tchar szWarning[MAX_PATH] = L"";
		lstrcat(szWarning, pPrototypeName);
		MessageBox(g_hWnd, szWarning, L"Press Enter", MB_OK);
		Safe_Delete(pData); // ÅÍÁö°Ô ¸¸µê
#endif // __TEST_JUNG
		return S_OK;
	}


	return S_OK;
}

HRESULT CEffect_Generator::Create_Prototype_Resource_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV")
		, CVIBuffer_Rect_TripleUV::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect_Effect.hlsl"), "DefaultTechnique")), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom")
		, CVIBuffer_PointInstance_Custom_ST::Create(pDevice, pDeviceContext, 10000, TEXT("../Bin/ShaderFiles/Shader_PointCustom.hlsl"), "DefaultTechnique")), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT")
		, CVIBuffer_PointInstance_Custom_STT::Create(pDevice, pDeviceContext, 10000, TEXT("../Bin/ShaderFiles/Shader_PointCustom_STT.hlsl"), "DefaultTechnique")), E_FAIL);

// 	FAILED_CHECK_RETURN(pInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_RectInstance_Custom")
// 		, CVIBuffer_RectInstance_Custom::Create(pDevice, pDeviceContext, 100, TEXT("../Bin/ShaderFiles/Shader_RectCustom.hlsl"), "DefaultTechnique")), E_FAIL);

#pragma region Texture
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Color_Ramp/Color_Ramp_%d.png"), 15)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Clouds_01_xbox"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Clouds_01_xbox.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Cloud_Texture_1k"),	CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Cloud_Texture_1k.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_ligntning_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Lightning/ligntning_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Fire_Loop/Fire_Loop_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Flow_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_01"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_01_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_02"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/Smoke_Puff_02_E%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Noise/Tilling_Noise_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Tilling_Cloud/Tilling_Cloud_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Ribbon_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Ribbon_Noise/Ribbon_Noise_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_CartoonTrail"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/CartoonTrail_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Gradient_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Gradient_Noise/Gradient_Noise_0%d.png"), 3)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Circle_Alpha.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_distortion"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/distortion_01_E2.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Dead_Cells"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Custom/Dead_Cells.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_DecalSplat"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Mask_Texture/T_DecalSplat_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_flowmaptest"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/flowmaptest_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Wormhole_Noise"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Wormhole_Noise/Wormhole_Noise_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Level_Preview"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Level_Preview/%d.png"), 7)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Slime_Cloud"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Shockwave_02_E1.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Dot"),					CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Custom/Dot.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Fire_Tiled"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/T_Fire_Tiled.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_SoftCLoud"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/SoftCLoud_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion7x7"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Explosion/Explosion7x7_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion8x8"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Explosion/Explosion8x8_%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ember_Texture"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Mask_Texture/T_Ember_Texture_256.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_radialgradientsample"),CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/radialgradientsample.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Spark_Mask"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Mask_Texture/Spark_Mask_01.png"))), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"),			CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Smoke/smokeloop_0%d.png"), 2)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Dust_Motes"),		CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Effect/2D/Mask_Texture/T_Dust_Motes_03.png"))), E_FAIL);

	
	
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
		
		WorldMatrix.r[3] = { 30.f, 0.f, 30.f, 1.f };
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
