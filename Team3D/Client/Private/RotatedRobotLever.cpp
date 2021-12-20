#include "stdafx.h"
#include "..\public\RotatedRobotLever.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "RotatedRobotHead.h"
#include "RotatedNoBatterySign.h"

CRotatedRobotLever::CRotatedRobotLever(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRotatedRobotParts(pDevice, pDeviceContext)
{
}

CRotatedRobotLever::CRotatedRobotLever(const CRotatedRobotParts & rhs)
	: CRotatedRobotParts(rhs)
{
}

HRESULT CRotatedRobotLever::NativeConstruct_Prototype()
{
	CRotatedRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotatedRobotLever::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotLever"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_tRtRobotPartsDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));

	_vector vPosition = m_tRtRobotPartsDesc.vPosition;
	vPosition.m128_f32[0] += 1.15f;
	vPosition.m128_f32[1] -= 1.45f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_RotateAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-30.f));


	m_UserData = USERDATA(GameID::eROBOTLEVER, this);
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);
	
	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

_int CRotatedRobotLever::Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);

	if (m_bUpdate == true)
	{
		if (m_IsCollide && m_pGameInstance->Pad_Key_Down(DIP_Y)
			|| m_IsCollide && m_pGameInstance->Key_Down(DIK_O))
		{
			UI_Delete(Cody, InputButton_InterActive);
			UI_Delete(May, InputButton_InterActive);
			m_IsCollide = false;
			m_bRotate = true;
		}

		if (m_bRotate == true)
		{
			Activate_Lever(dTimeDelta);
		}
	}
	return NO_EVENT;
}

_int CRotatedRobotLever::Late_Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotatedRobotLever::Render(RENDER_GROUP::Enum eGroup)
{
	CRotatedRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CRotatedRobotLever::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (m_bUpdate == true)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTLEVER, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			UI_Create(Cody, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		{
			m_IsCollide = false;
			UI_Delete(Cody, InputButton_InterActive);
		}

		// May

		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		{
			((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eROBOTLEVER, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			UI_Create(May, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		{
			m_IsCollide = false;
			UI_Delete(May, InputButton_InterActive);
		}
	}
}

HRESULT CRotatedRobotLever::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRotatedRobotLever::Activate_Lever(_double dTimeDelta)
{
	// 만약 배터리가 없다면? 오른쪽 갔다가 0.5초 후에 다시 왼쪽으로 돌아와.
	if (m_bBatteryCharged == false)
	{
		m_fStopDelay += (_float)dTimeDelta;
		if (m_fStopDelay > 0.2f && m_fStopDelay <= 0.6f)
		{
			_vector vDir = XMVector3Normalize((XMVectorSet(0.f, 0.f, -1.f, 0.f) + XMVectorSet(0.f, -1.f, 0.f, 0.f))/* / 2.f*/);
			//m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);
			m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -dTimeDelta * 1.2f);

			if (m_bNoBatteryHit == false)
			{
				((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_RobotHead()->Set_Lever_Hit_When_NoBattery(true);
				((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_NoBatterySign()->Set_HitLever(true);
				m_bNoBatteryHit = true;
			}
		}
		else if (m_fStopDelay > 1.f && m_fStopDelay <= 1.4f)
		{
			_vector vDir = XMVector3Normalize((XMVectorSet(0.f, 0.f, 1.f, 0.f) + XMVectorSet(0.f, -1.f, 0.f, 0.f)) /*/ 2.f*/);
			//m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);
			m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * 1.15f);
		}
		else if (m_fStopDelay > 1.4f)
		{
			m_bNoBatteryHit = false;
			m_fStopDelay = 0.f;
			m_bRotate = false;
		}
	}
	else if (m_bBatteryCharged == true)
	{
		DATABASE->Set_PinBallStageClear(true);

		m_fStopDelay += (_float)dTimeDelta;
		if (m_fStopDelay > 0.2f && m_fStopDelay <= 0.6f)
		{
			_vector vDir = XMVector3Normalize((XMVectorSet(0.f, 0.f, -1.f, 0.f) + XMVectorSet(0.f, -1.f, 0.f, 0.f)) /*/ 2.f*/);
			//m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);
			m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -dTimeDelta* 1.2f); 
			((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_RobotHead()->Set_Lever_Active(true);
			((CRotatedRobotParts*)DATABASE->Get_STPinBallRobot())->Get_NoBatterySign()->Set_BatteryCharged(true);
		}
		else if (m_fStopDelay > 0.6f)
		{
			m_bNoBatteryHit = false;
			m_fStopDelay = 0.f;
			m_bRotate = false;
			m_bUpdate = false;
		}
	}
}

CRotatedRobotLever * CRotatedRobotLever::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedRobotLever* pInstance = new CRotatedRobotLever(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRotatedRobotLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedRobotLever::Clone_GameObject(void * pArg)
{
	CRotatedRobotLever* pInstance = new CRotatedRobotLever(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedRobotLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedRobotLever::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}