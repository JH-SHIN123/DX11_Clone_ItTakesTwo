#include "stdafx.h"
#include "..\Public\WarpGate.h"
#include "WarpGate_Star.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "DataStorage.h"
#include "Effect_Generator.h"
#include "Effect_Env_Particle.h"
#include "Effect_RespawnTunnel.h"
#include "Effect_RespawnTunnel_Portal.h"
#include "Script.h"

CWarpGate::CWarpGate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWarpGate::CWarpGate(const CWarpGate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWarpGate::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWarpGate::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WarpGate_Desc, pArg, sizeof(WARPGATE_DESC));

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Check_WarpGate_Star(), E_FAIL);

	if(nullptr != m_pRespawnTunnel)
		m_pRespawnTunnel->Set_Stage_Viewer(m_WarpGate_Desc.eStageValue);

	_matrix SmokeWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	SmokeWorldMatrix.r[3] += XMVectorSet(0.f, 1.8f, 0.f, 0.f);
	SmokeWorldMatrix.r[3] -= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 2.5f;
	SmokeWorldMatrix.r[3] -= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 2.f;
	EFFECT->Add_Effect(Effect_Value::Gate_Smoke, SmokeWorldMatrix);

	m_pGameInstance->Play_Sound(TEXT("Gate.wav"), CHANNEL_GATE, 0.f);
	m_pGameInstance->Play_Sound(TEXT("GateStar.wav"), CHANNEL_GATE_STAR, 0.f);

	return S_OK;
}

_int CWarpGate::Tick(_double TimeDelta)
{
	Check_StageClear();
	Check_ClearEffect(TimeDelta);
	Check_Sound();

	return _int();
}

_int CWarpGate::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CWarpGate::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CWarpGate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		if (false == static_cast<CMay*>(DATABASE->GetMay())->Get_IsWarpDone() &&
			false == static_cast<CMay*>(DATABASE->GetMay())->Get_IsWarpNextStage())
		{
			m_pGameInstance->Play_Sound(TEXT("Gate_Pass.wav"), CHANNEL_GATE_PASS);
			((CMay*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eWARPGATE, true, this);
		}
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		if (false == static_cast<CMay*>(DATABASE->GetMay())->Get_IsWarpDone() &&
			false == static_cast<CMay*>(DATABASE->GetMay())->Get_IsWarpNextStage())
			((CMay*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eWARPGATE, false, this);
	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		if (false == static_cast<CCody*>(DATABASE->GetCody())->Get_IsWarpDone() &&
			false == static_cast<CCody*>(DATABASE->GetCody())->Get_IsWarpNextStage())
		{
			m_pGameInstance->Play_Sound(TEXT("Gate_Pass.wav"), CHANNEL_GATE_PASS);
			((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eWARPGATE, true, this);
		}
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		if (false == static_cast<CCody*>(DATABASE->GetCody())->Get_IsWarpDone() &&
			false == static_cast<CCody*>(DATABASE->GetCody())->Get_IsWarpNextStage())
			((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eWARPGATE, false, this);
	}
}

HRESULT CWarpGate::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CWarpGate::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	Check_Stage_Value();

	m_UserData = USERDATA(GameID::eWARPGATE, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxBoxGeometry(5.f, 8.f, 0.1f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	CStaticActor::ARG_DESC Static_ArgDesc;
	Static_ArgDesc.pModel = m_pModelCom;
	Static_ArgDesc.pTransform = m_pTransformCom;
	Static_ArgDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &Static_ArgDesc), E_FAIL);

	EFFECT_DESC_CLONE Effect_Desc;
	XMStoreFloat4x4(&Effect_Desc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	if(MAIN_TENNIS != m_WarpGate_Desc.eStageValue)
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RespawnPortal"), Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel"), &Effect_Desc, (CGameObject**)&m_pRespawnTunnel), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RespawnPortal"), Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel_Portal"), &Effect_Desc, (CGameObject**)&m_pRespawnTunnel_Portal), E_FAIL);

	return S_OK;
}

void CWarpGate::Check_Stage_Value()
{
	_vector vPos = XMVectorZero();
	_float	fDegree = 0.f;

	switch (m_WarpGate_Desc.eStageValue)
	{
	case CWarpGate::MAIN_UMBRELLA:
		m_vWarpPos = {};
		vPos = XMVectorSet(31.f, 125.25f, 195.8f, 1.f);
		fDegree = 90.f;
		break;
	case CWarpGate::STAGE_UMBRELLA:
		m_vWarpPos = {};
		vPos = XMVectorSet(-617.f, 755.f, 196.f, 1.f);
		fDegree = -90.f;
		break;
	case CWarpGate::MAIN_PLANET:
		m_vWarpPos = {};
		vPos = XMVectorSet(97.8f, 125.25f, 195.8f, 1.f);
		fDegree = -90.f;
		break;
	case CWarpGate::STAGE_PLANET:
		m_vWarpPos = {};
		vPos = XMVectorSet(617.f, 755.f, 196.2f, 1.f);
		fDegree = 90.f;
		break;
	case CWarpGate::MAIN_TENNIS:
		vPos = XMVectorSet(63.8f, 104.5f, 162.f, 1.f);
		break;
	case CWarpGate::STAGE_TENNIS:
		vPos = XMVectorSet(64.f, 730.2f, 956.3f, 1.f);
		break;
	default:
		break;
	}

	DATABASE->Set_May_Stage(ST_GRAVITYPATH);
	DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fDegree));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CWarpGate::Check_WarpGate_Star()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_WarpGate_Star"), Level::LEVEL_STAGE, TEXT("GameObject_WarpGate_Star"), nullptr, (CGameObject**)&m_pWarpGate_Star_1), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_WarpGate_Star"), Level::LEVEL_STAGE, TEXT("GameObject_WarpGate_Star"), nullptr, (CGameObject**)&m_pWarpGate_Star_2), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_WarpGate_Star"), Level::LEVEL_STAGE, TEXT("GameObject_WarpGate_Star"), nullptr, (CGameObject**)&m_pWarpGate_Star_3), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_WarpGate_Star"), Level::LEVEL_STAGE, TEXT("GameObject_WarpGate_Star"), nullptr, (CGameObject**)&m_pWarpGate_Star_4), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_WarpGate_Star"), Level::LEVEL_STAGE, TEXT("GameObject_WarpGate_Star"), nullptr, (CGameObject**)&m_pWarpGate_Star_5), E_FAIL);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pWarpGate_Star_1->Set_WorldMatrix(WorldMatrix, XMVectorSet(5.9f, 7.8f, 1.f, 1.f));
	m_pWarpGate_Star_2->Set_WorldMatrix(WorldMatrix, XMVectorSet(5.f, 10.f, 1.f, 1.f));
	m_pWarpGate_Star_3->Set_WorldMatrix(WorldMatrix, XMVectorSet(3.3f, 11.5f, 1.f, 1.f));
	m_pWarpGate_Star_4->Set_WorldMatrix(WorldMatrix, XMVectorSet(-0.8f, 12.8f, 1.f, 1.f));
	m_pWarpGate_Star_5->Set_WorldMatrix(WorldMatrix, XMVectorSet(-3.2f, 13.2f, 1.f, 1.f));

	m_pWarpGate_Star_1->Set_Scale(XMVectorSet(0.8f, 0.8f, 0.8f, 0.f));
	m_pWarpGate_Star_2->Set_Scale(XMVectorSet(1.1f, 1.1f, 1.1f, 0.f));
	m_pWarpGate_Star_3->Set_Scale(XMVectorSet(0.8f, 0.8f, 0.8f, 0.f));
	m_pWarpGate_Star_4->Set_Scale(XMVectorSet(0.8f, 0.8f, 0.8f, 0.f));
	m_pWarpGate_Star_5->Set_Scale(XMVectorSet(1.17f, 1.17f, 1.17f, 0.f));

	return S_OK;
}

void CWarpGate::Check_ClearEffect(_double TimeDelta)
{
	if (false == m_bClearEffect)
		return;

	_double dTerm = 0.495 * (_double)m_iClearEffect_Count;
	m_dClearEffect_Time += TimeDelta;

	switch (m_iClearEffect_Count)
	{
	case 0:
		if (m_WarpGate_Desc.dClearEffect_Term < m_dClearEffect_Time)
		{
			m_pGameInstance->Play_Sound(TEXT("GateStar.wav"), CHANNEL_GATE_STAR);
			m_pWarpGate_Star_1->Set_Activate(true);
			++m_iClearEffect_Count;
		}
		break;
	case 1:
		if (m_WarpGate_Desc.dClearEffect_Term + dTerm < m_dClearEffect_Time)
		{
			m_pWarpGate_Star_2->Set_Activate(true);
			++m_iClearEffect_Count;
		}
		break;
	case 2:
		if (m_WarpGate_Desc.dClearEffect_Term + dTerm < m_dClearEffect_Time)
		{
			m_pWarpGate_Star_3->Set_Activate(true);
			++m_iClearEffect_Count;
		}
		break;
	case 3:
		if (m_WarpGate_Desc.dClearEffect_Term + dTerm < m_dClearEffect_Time)
		{
			m_pWarpGate_Star_4->Set_Activate(true);
			++m_iClearEffect_Count;
		}
		break;
	case 4:
		if (m_WarpGate_Desc.dClearEffect_Term + dTerm < m_dClearEffect_Time)
		{
			m_pWarpGate_Star_5->Set_Activate(true);
			++m_iClearEffect_Count;
		}
		break;
	default:
		m_dClearEffect_Time = 0.0;
		break;
	}
}

void CWarpGate::Check_StageClear()
{
	if (true == m_bClearEffect)
		return;

	if (MAIN_UMBRELLA == m_WarpGate_Desc.eStageValue || STAGE_UMBRELLA == m_WarpGate_Desc.eStageValue)
		m_bClearEffect = DATABASE->Get_PinBallStageClear();

	else if (MAIN_PLANET == m_WarpGate_Desc.eStageValue || STAGE_PLANET == m_WarpGate_Desc.eStageValue)
		m_bClearEffect = DATABASE->Get_RailStageClear();

	if (m_pGameInstance->Key_Down(DIK_V))
		m_bClearEffect = true;

	Check_Tennis_Found();
}

void CWarpGate::Check_Tennis_Found()
{
	if (MAIN_TENNIS == m_WarpGate_Desc.eStageValue)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (ST_GRAVITYPATH == DATABASE->Get_May_Stage())
		{
			_vector vMayPos = DATABASE->GetMay()->Get_Position();
			_vector vDir = vMayPos - vPos;

			for(_int i = 0; i < 3 ; ++i)
				vDir.m128_f32[i] = fabs(vDir.m128_f32[i]);

			if (0.45f > vDir.m128_f32[0] &&
				0.25f > vDir.m128_f32[1] &&
				15.8f > vDir.m128_f32[2])
				m_bClearEffect = true;
		}

		if (ST_GRAVITYPATH == DATABASE->Get_Cody_Stage())
		{
			_vector vMayPos = DATABASE->GetCody()->Get_Position();
			_vector vDir = vMayPos - vPos;

			for (_int i = 0; i < 3; ++i)
				vDir.m128_f32[i] = fabs(vDir.m128_f32[i]);

			if (0.45f > vDir.m128_f32[0] &&
				0.25f > vDir.m128_f32[1] &&
				15.8f > vDir.m128_f32[2])
				m_bClearEffect = true;
		}
	}

	return;
}

void CWarpGate::Check_Sound()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPos_Cody = DATABASE->GetCody()->Get_Position();
	_vector vPos_May = DATABASE->GetMay()->Get_Position();

	_float	fLength_Cody = XMVector3Length(vPos - vPos_Cody).m128_f32[0];
	_float	fLength_May = XMVector3Length(vPos - vPos_May).m128_f32[0];
	_float	fLength_Size = 7.f;

	_bool	IsSoundOff[2] = { false, false };

	if (fLength_Size < fLength_Cody)
		IsSoundOff[0] = true;

	if (fLength_Size < fLength_May)
		IsSoundOff[1] = true;

	if (true == IsSoundOff[0] && true == IsSoundOff[1])
		return;

	_float fLength_Less = fLength_Cody;
	if (fLength_Cody > fLength_May)
		fLength_Less = fLength_May;

	_float fNormalize = fabs(fLength_Size - fLength_Less) / fLength_Size;

	fNormalize *= 2.f;
	m_pGameInstance->Set_SoundVolume(CHANNEL_GATE, fNormalize);
	if (false == m_pGameInstance->IsPlaying(CHANNEL_GATE))
		m_pGameInstance->Play_Sound(TEXT("Gate.wav"), CHANNEL_GATE, fNormalize);
}

_fmatrix CWarpGate::Get_NextPortal_Matrix()
{
	_matrix NextPortalMatrix = XMMatrixIdentity();

	_vector vPos = XMVectorZero();
	_float	fDegree = 0.f;

	// 현재 스테이지 > 다음 스테이지
	switch (m_WarpGate_Desc.eStageValue)
	{
	case CWarpGate::MAIN_UMBRELLA:
		vPos = XMVectorSet(-617.f, 755.f, 196.f, 1.f);
		fDegree = -90.f;
		if (DATABASE->Get_RailStageClear() || DATABASE->Get_PinBallStageClear())
		{
			if (DATABASE->Get_May_Stage() == ST_GRAVITYPATH && SCRIPT->Get_Script_Played(8) == true && SCRIPT->Get_Script_Played(9) == false)
			{
				SCRIPT->VoiceFile_No09();
				SCRIPT->Set_Script_Played(9, true);
			}
		}
		DATABASE->Set_May_Stage(ST_PINBALL);
		DATABASE->Set_Cody_Stage(ST_PINBALL);

		break;
	case CWarpGate::STAGE_UMBRELLA:
		vPos = XMVectorSet(31.f, 125.25f, 195.8f, 1.f);
		fDegree = 90.f;
		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
		break;
	case CWarpGate::MAIN_PLANET:
		vPos = XMVectorSet(617.f, 755.f, 196.2f, 1.f);
		fDegree = 90.f;
		if (DATABASE->Get_RailStageClear() || DATABASE->Get_PinBallStageClear())
		{
			if (DATABASE->Get_May_Stage() == ST_GRAVITYPATH && SCRIPT->Get_Script_Played(8) == true && SCRIPT->Get_Script_Played(9) == false)
			{
				SCRIPT->VoiceFile_No09();
				SCRIPT->Set_Script_Played(9, true);
			}
		}
		DATABASE->Set_May_Stage(ST_RAIL);
		DATABASE->Set_Cody_Stage(ST_RAIL);
		break;
	case CWarpGate::STAGE_PLANET:
		vPos = XMVectorSet(97.8f, 125.25f, 195.8f, 1.f);
		fDegree = -90.f;

		DATABASE->Set_May_Stage(ST_GRAVITYPATH);
		DATABASE->Set_Cody_Stage(ST_GRAVITYPATH);
		break;
	case CWarpGate::MAIN_TENNIS:
		vPos = XMVectorSet(64.f, 730.2f, 956.3f, 1.f);
		break;
	case CWarpGate::STAGE_TENNIS:
		vPos = XMVectorSet(63.8f, 104.5f, 163.f, 1.f);
		break;
	default:
		break;
	}

	vPos.m128_f32[1] += 5.f;

	NextPortalMatrix = XMMatrixRotationY(XMConvertToRadians(fDegree));
	NextPortalMatrix.r[3] = vPos;
	_vector vDir = NextPortalMatrix.r[2];
	NextPortalMatrix.r[3] += vDir;

	return NextPortalMatrix;
}

CWarpGate * CWarpGate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWarpGate* pInstance = new CWarpGate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWarpGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWarpGate::Clone_GameObject(void * pArg)
{
	CWarpGate* pInstance = new CWarpGate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWarpGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarpGate::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTriggerCom_Tennis);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom_Trigger);
	Safe_Release(m_pRespawnTunnel);
	Safe_Release(m_pRespawnTunnel_Portal);
	Safe_Release(m_pWarpGate_Star_1);
	Safe_Release(m_pWarpGate_Star_2);
	Safe_Release(m_pWarpGate_Star_3);
	Safe_Release(m_pWarpGate_Star_4);
	Safe_Release(m_pWarpGate_Star_5);

	__super::Free();
}
