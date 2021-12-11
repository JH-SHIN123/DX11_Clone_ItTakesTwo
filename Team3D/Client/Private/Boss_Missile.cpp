#include "stdafx.h"
#include "..\Public\Boss_Missile.h"
#include "DataStorage.h"
#include "UI_Generator.h"
#include "May.h"
#include "Cody.h"

CBoss_Missile::CBoss_Missile(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBoss_Missile::CBoss_Missile(const CBoss_Missile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss_Missile::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Missile::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	tagBossMissile_Desc Data;
	memcpy(&Data, pArg, sizeof(tagBossMissile_Desc));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(3.f, XMConvertToRadians(40.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform_ForActor"), (CComponent**)&m_pTransformCom_Actor, &CTransform::TRANSFORM_DESC(3.f, XMConvertToRadians(40.f))), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&Data.vDir));

	_vector vPos = XMLoadFloat4(&Data.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	vPos += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 1.5f;
	m_pTransformCom_Actor->Set_State(CTransform::STATE_POSITION, vPos);

	CTriggerActor::ARG_DESC ArgDesc;
	m_UserData = USERDATA(GameID::eBOSSMISSILE_COMBAT, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom_Actor;
	ArgDesc.pGeometry = new PxSphereGeometry(0.5f);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger_Combat"), (CComponent**)&m_pTriggerCom_Combat, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);


	m_IsTargetCody = Data.IsTarget_Cody;

	return S_OK;
}

_int CBoss_Missile::Tick(_double TimeDelta)
{
	if (true == m_IsNull_Trigger)
		return EVENT_DEAD;

	if (true == m_IsExplosion)
		return Dead_Explosion();

	if (true == m_IsCombatMode)
		Combat_Move(TimeDelta);
	else
		Playable_Mode(TimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//vPos += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 1.5f;
	m_pTransformCom_Actor->Set_State(CTransform::STATE_POSITION, vPos);

	if (true == m_IsUpadate_Trigger)
	{
		m_fUpdateTrigger_Term += (_float)TimeDelta;
		if (m_fUpdateTrigger_Term_Max <= m_fUpdateTrigger_Term)
		{
			m_fUpdateTrigger_Term = 0.f;
			m_IsUpadate_Trigger = false;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_H))
		m_IsExplosion = true;

	return _int();
}

_int CBoss_Missile::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBoss_Missile::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

void CBoss_Missile::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (true == m_IsCombatMode)
	{
		// Äâ±¤
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			((CCody*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eBOSSMISSILE_PLAYABLE, true, m_pTransformCom->Get_WorldMatrix());
			UI_Create(Cody, InputButton_InterActive);
			UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsExplosion = true;
		}
	}
	else
	{		
		// Á¶Á¾
		if (false == m_IsPlayable)
		{
			if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
			{
				((CCody*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eBOSSMISSILE_PLAYABLE, true, m_pTransformCom->Get_WorldMatrix());
				//UI_Create(Cody, InputButton_InterActive);
				//UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				m_IsPlayable_Ready = true;

				m_eTarget_GameID = eID;
			}
			else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
			{
				_int i = 0;
			}
		}
// 		else if (true == m_IsPlayable)
// 		{
// 			if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
// 			{
// 				((CCody*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eBOSSMISSILE_PLAYABLE, false, m_pTransformCom->Get_WorldMatrix());
// 				UI_Create(Cody, InputButton_InterActive);
// 				UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
// 				m_IsPlayable_Ready = true;
// 
// 				m_eTarget_GameID = eID;
// 			}
// 		}


		else if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eUFO)
		{
			m_IsExplosion	= true;
			m_IsPlayable	= false;
		}

	}
}

HRESULT CBoss_Missile::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

void CBoss_Missile::Combat_Move(_double TimeDelta)
{
	if (false == m_IsCombatMode)
		return;

	m_fCombatTime -= (_float)TimeDelta;

	_float fAccel = 1.f;
	if (1.f >= m_fCombatTime)
		fAccel = m_fCombatTime;

	m_fCurvePower -= (_float)TimeDelta * 3.f;
	if (1.f >= m_fCurvePower)
		m_fCurvePower = 1.f;

	_vector vTargetPos;
	if (true == m_IsTargetCody)
		vTargetPos = static_cast<CCody*>(CDataStorage::GetInstance()->GetCody())->Get_Position();
	else
		vTargetPos = static_cast<CMay*>(CDataStorage::GetInstance()->GetMay())->Get_Position();

	_float4 vConvertPos;
	XMStoreFloat4(&vConvertPos, vTargetPos);
	vConvertPos.y += 1.f;
	vTargetPos = XMLoadFloat4(&vConvertPos);

	_matrix WorldMatrix		= m_pTransformCom->Get_WorldMatrix();
	_vector vRight			= WorldMatrix.r[0];
	_vector vUp				= WorldMatrix.r[1];
	_vector vLook			= WorldMatrix.r[2];
	_vector vPosition		= WorldMatrix.r[3];

	_vector vTargetDir		= XMVector3Normalize(vTargetPos - vPosition);
	_vector vNewUpDir		= XMVector3Normalize(vTargetDir + ((vUp * 10.f) * m_fCurvePower));
	_vector vNewRightDir	= XMVector3Normalize(XMVector3Cross(vNewUpDir, vLook));
	_vector vNewLookDir		= XMVector3Normalize(XMVector3Cross(vNewRightDir, vNewUpDir));

	vPosition += XMVector3Normalize(vNewUpDir + (vUp * 25.f)) * (_float)TimeDelta * (4.f * fAccel) * 1.3f;
	WorldMatrix.r[0] = vNewRightDir;
	WorldMatrix.r[1] = vNewUpDir;
	WorldMatrix.r[2] = vNewLookDir;
	WorldMatrix.r[3] = vPosition;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	_float fDis = XMVector3Length(vTargetPos - vPosition).m128_f32[0];

	if (3.f >= fDis && false == m_IsUpadate_Trigger)
	{
		m_IsUpadate_Trigger = true;
		m_pTriggerCom_Combat->Update_TriggerActor();
	}

	if (0.f >= m_fCombatTime)
	{
		m_fCombatTime	= 0.f;
		m_IsCombatMode	= false;

		CTriggerActor::ARG_DESC ArgDesc;
		m_UserData = USERDATA(GameID::eBOSSMISSILE_PLAYABLE, this);
		ArgDesc.pUserData = &m_UserData;
		ArgDesc.pTransform = m_pTransformCom_Actor;
		ArgDesc.pGeometry = new PxSphereGeometry(0.5f);
		CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger_Playeable2"), (CComponent**)&m_pTriggerCom_Playable, &ArgDesc);
		Safe_Delete(ArgDesc.pGeometry);
		if (nullptr == m_pTriggerCom_Playable)
			m_IsNull_Trigger = true;
	}
}

void CBoss_Missile::Playable_Mode(_double TimeDelta)
{
	if (true == m_IsPlayable_Ready)
	{
		if (m_pGameInstance->Pad_Key_Down(DIP_RB) && GameID::Enum::eMAY == m_eTarget_GameID)
			m_IsPlayable = true;

		if (m_pGameInstance->Key_Down(DIK_F) && GameID::Enum::eCODY == m_eTarget_GameID)
			m_IsPlayable = true;

		if (true == m_IsPlayable)
		{
			if (GameID::Enum::eMAY == m_eTarget_GameID)
			{
				_matrix TargetMatrix = static_cast<CCharacter*>(CDataStorage::GetInstance()->GetMay())->Get_WorldMatrix(); // ÀÌºÎºÐ °íÄ¡±â

				for (_int i = 0; i < 3; ++i)
					TargetMatrix.r[i] = XMVector3Normalize(TargetMatrix.r[i]);
		
				TargetMatrix = XMMatrixRotationAxis(TargetMatrix.r[0], -90.f);

				TargetMatrix.r[0] = XMVector3TransformNormal(TargetMatrix.r[0], TargetMatrix);
				TargetMatrix.r[1] = XMVector3TransformNormal(TargetMatrix.r[1], TargetMatrix);
				TargetMatrix.r[2] = XMVector3TransformNormal(TargetMatrix.r[2], TargetMatrix);

				TargetMatrix.r[3] -= (TargetMatrix.r[1]) + (TargetMatrix.r[2] * 0.8f);
				m_pTransformCom->Set_WorldMatrix(TargetMatrix);
			}

			if (GameID::Enum::eCODY == m_eTarget_GameID)
			{
				_matrix TargetMatrix = static_cast<CCharacter*>(CDataStorage::GetInstance()->GetCody())->Get_WorldMatrix(); // ÀÌºÎºÐ °íÄ¡±â

				//_vector vRight	= XMVector3Normalize(TargetMatrix.r[0]);
				_vector vUp		= TargetMatrix.r[2];
				_vector vLook	= TargetMatrix.r[1];
				TargetMatrix.r[1] = vUp;
				TargetMatrix.r[2] = vLook * -1.f;

				for (_int i = 0; i < 3; ++i)
					TargetMatrix.r[i] = XMVector3Normalize(TargetMatrix.r[i]);

				//TargetMatrix = XMMatrixRotationAxis(TargetMatrix.r[0], -90.f);
				//
				//TargetMatrix.r[0] = XMVector3TransformNormal(vRight, TargetMatrix);
				//TargetMatrix.r[1] = XMVector3TransformNormal(vUp, TargetMatrix);
				//TargetMatrix.r[2] = XMVector3TransformNormal(vLook, TargetMatrix);
				TargetMatrix.r[3] -= (TargetMatrix.r[1]) + (TargetMatrix.r[2] * -0.25f);

				m_pTransformCom->Set_WorldMatrix(TargetMatrix);
			}
		}
	}
}

_int CBoss_Missile::Dead_Explosion()
{
	if (true == m_IsPlayable)
	{
		if (GameID::Enum::eMAY == m_eTarget_GameID)
		{

		}
		else
		{
			static_cast<CCody*>(CDataStorage::GetInstance()->GetCody())->Set_BossMissile_Attack();
		}
	}


	// ÀÌÆåÆ® »§»§
	return EVENT_DEAD;
}

CBoss_Missile * CBoss_Missile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBoss_Missile* pInstance = new CBoss_Missile(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBoss_Missile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_Missile::Clone_GameObject(void * pArg)
{
	CBoss_Missile* pInstance = new CBoss_Missile(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBoss_Missile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Missile::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTriggerCom_Combat);
	Safe_Release(m_pTriggerCom_Playable);
	Safe_Release(m_pTransformCom_Actor);

	__super::Free();
}
