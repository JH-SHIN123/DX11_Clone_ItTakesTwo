#include "stdafx.h"
#include "..\public\RotatedRobotHead.h"
#include "Cody.h"
#include "May.h"

CRotatedRobotHead::CRotatedRobotHead(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRotatedRobotParts(pDevice, pDeviceContext)
{
}

CRotatedRobotHead::CRotatedRobotHead(const CRotatedRobotParts & rhs)
	: CRotatedRobotParts(rhs)
{
}

HRESULT CRotatedRobotHead::NativeConstruct_Prototype()
{
	CRotatedRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotatedRobotHead::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotHead"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	if (nullptr != pArg)
		memcpy(&m_tRtRobotPartsDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));

	_vector vPosition = m_tRtRobotPartsDesc.vPosition;
	vPosition = XMVectorSetX(vPosition, XMVectorGetX(vPosition) + 4.f);
	// 로봇 x += 4.f
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation(3);
	m_pModelCom->Set_NextAnimIndex(3);

	m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
	m_pGameInstance->Play_Sound(TEXT("Robot_Move.wav"), CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
	m_pGameInstance->Stop_Sound(CHANNEL_ROBOT_MOVE);

	return S_OK;
}

_int CRotatedRobotHead::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_bBatteryCharged == false && m_bLeverActive == false)
	{
		if (m_bHitLeverInActive == true)
		{
			Hit_Lever_InActive(dTimeDelta);
			m_bHitLeverInActive = false;
		}
		else
		{
			if(m_pModelCom->Get_CurAnimIndex() != R_InActive_Lever)
				m_pModelCom->Set_Animation(R_InActive_Idle);
		}
	}
	else if (m_bBatteryCharged == true && m_bLeverActive == false)
	{
		if (m_pModelCom->Is_AnimFinished(R_InActive_Idle))
		{
			m_pModelCom->Set_Animation(R_InActive_Battery_Idle);
			m_pModelCom->Set_NextAnimIndex(R_InActive_Battery_Idle);
		}
		if (m_pModelCom->Is_AnimFinished(R_InActive_Battery_Idle))
		{
			m_pModelCom->Set_Animation(R_InActive_Battery_Idle);
		}
	}
	else if (m_bBatteryCharged == true && m_bLeverActive == true)
	{
		_vector vRobotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vCodyPos = ((CCody*)DATABASE->GetCody())->Get_Position();
		_vector vMayPos = ((CMay*)DATABASE->GetMay())->Get_Position();

		_float fCodyDistance = XMVectorGetX(XMVector3Length(vCodyPos - vRobotPos));
		_float fMayDistance = XMVectorGetX(XMVector3Length(vMayPos - vRobotPos));


		// Sound Volume 기준을 0.f ~ 1.f로 잡았다면.. 
		_float fFinalDist = min(fCodyDistance, fMayDistance);

		if (fFinalDist > 30.f)
			m_fHeadBanging_Volume = 0.f;
		else
			m_fHeadBanging_Volume = 2.f - fFinalDist / 30.f;

		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_ROBOT_MOVE) == false && fFinalDist < 30.f)
		{
			m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
			m_pGameInstance->Play_Sound(TEXT("Robot_Move.wav"), CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
		}

		if (m_pModelCom->Is_AnimFinished(R_InActive_Battery_Idle))
		{
			m_pModelCom->Set_Animation(R_Active_Start);
			m_pModelCom->Set_NextAnimIndex(R_Active_Idle);

		}
		if (m_pModelCom->Is_AnimFinished(R_Active_Idle))
		{
			m_pModelCom->Set_Animation(R_Active_Idle);
			m_pModelCom->Set_NextAnimIndex(R_Active_Idle);
		}
	}

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CRotatedRobotHead::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotatedRobotHead::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CRotatedRobotHead::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK; 
}

void CRotatedRobotHead::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CRotatedRobotHead::Hit_Lever_InActive(_double dTimeDelta)
{
	m_tRtRobotPartsDesc.iStageNum;

	m_pModelCom->Set_Animation(R_InActive_Lever);
	m_pModelCom->Set_NextAnimIndex(R_InActive_Idle);
}

CRotatedRobotHead * CRotatedRobotHead::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedRobotHead* pInstance = new CRotatedRobotHead(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRotatedRobotHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedRobotHead::Clone_GameObject(void * pArg)
{
	CRotatedRobotHead* pInstance = new CRotatedRobotHead(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedRobotHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedRobotHead::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CRotatedRobotParts::Free();
}