#include "stdafx.h"
#include "..\public\RobotHead.h"
#include "Cody.h"
#include "May.h"

CRobotHead::CRobotHead(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRobotParts(pDevice, pDeviceContext)
{
}

CRobotHead::CRobotHead(const CRobotParts & rhs)
	: CRobotParts(rhs)
{
}

HRESULT CRobotHead::NativeConstruct_Prototype()
{
	CRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobotHead::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RobotHead"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	if (nullptr != pArg)
		memcpy(&m_tRobotPartsDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	_vector vPosition = m_tRobotPartsDesc.vPosition;
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 4.f);
	// 로봇 y += 4.f
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation(3);
	m_pModelCom->Set_NextAnimIndex(3);

	m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
	m_pGameInstance->Play_Sound(TEXT("Robot_Move.wav"), CHANNEL_ROBOT_MOVE, m_fHeadBanging_Volume);
	m_pGameInstance->Stop_Sound(CHANNEL_ROBOT_MOVE);

	return S_OK;
}

_int CRobotHead::Tick(_double dTimeDelta)
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


		// Sound Volume 기준을 0.f ~ 1.f로 잡았다면.. 나누기 10..
		_float fFinalDist = max(fCodyDistance, fMayDistance);

		if (fFinalDist > 10.f)
			m_fHeadBanging_Volume = 0.f;
		else
			m_fHeadBanging_Volume = 2.f - fFinalDist / 10.f;

		if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_ROBOT_MOVE) == false)
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

_int CRobotHead::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRobotHead::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CRobotHead::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK; 
}

void CRobotHead::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CRobotHead::Hit_Lever_InActive(_double dTimeDelta)
{
	m_pModelCom->Set_Animation(R_InActive_Lever);
	m_pModelCom->Set_NextAnimIndex(R_InActive_Idle);
}

CRobotHead * CRobotHead::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobotHead* pInstance = new CRobotHead(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRobotHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobotHead::Clone_GameObject(void * pArg)
{
	CRobotHead* pInstance = new CRobotHead(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobotHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobotHead::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CRobotParts::Free();
}