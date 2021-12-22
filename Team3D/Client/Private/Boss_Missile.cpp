#include "stdafx.h"
#include "..\Public\Boss_Missile.h"
#include "DataStorage.h"
#include "UI_Generator.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(1.5f, XMConvertToRadians(120.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Boss_Missile"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	_vector vPos = XMLoadFloat4(&Data.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_IsTargetCody = Data.IsTarget_Cody;

	return S_OK;
}

_int CBoss_Missile::Tick(_double dTimeDelta)
{

	if (m_bPlayerExplosion == true)
	{

		return EVENT_DEAD;
	}
	else if (m_bBossExplosion == true)
	{
		if (m_bCodyControl == true && m_bMayControl == false)
		{
			((CCody*)DATABASE->GetCody())->Set_Escape_From_Rocket(true);
		}
		else if (m_bMayControl == true && m_bCodyControl == false)
		{
			((CMay*)DATABASE->GetMay())->Set_Escape_From_Rocket(true);
		}

		return EVENT_DEAD;
	}


	if (m_IsCrashed == false)
		m_fAttackTime += (_float)dTimeDelta;

	if (m_fAttackTime > 10.f)
	{
		m_IsFalling = true;
		m_fAttackTime = 0.f;
	}

	if (m_IsCrashed == false)
		Combat_Move(dTimeDelta);
	else if (m_IsCrashed == true && m_IsCollide && (m_pGameInstance->Key_Down(DIK_O) || m_pGameInstance->Pad_Key_Down(DIP_Y)))
	{
		m_bMayControl = true;
		UI_Delete(Cody, InputButton_InterActive);
		UI_Delete(May, InputButton_InterActive);
	}
	else if (m_IsCrashed == true && m_IsCollide && m_pGameInstance->Key_Down(DIK_E))
	{
		m_bCodyControl = true;
		UI_Delete(Cody, InputButton_InterActive);
		UI_Delete(May, InputButton_InterActive);
	}

	if (m_bMayControl)
		MayControl_Move(dTimeDelta);
	else if (m_bCodyControl)
		CodyControl_Move(dTimeDelta);

	if (m_IsFalling)
		Falling(dTimeDelta);
	if (m_bStartAdjustAngle)
		Adjust_Angle(dTimeDelta);


	return _int();
}

_int CBoss_Missile::Late_Tick(_double dTimeDelta)
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
	// Cody
	if (m_bMayControl || m_bCodyControl)
		return;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSMISSILE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollide = false;
		UI_Delete(Cody, InputButton_InterActive);
	}

	//May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSMISSILE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
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

HRESULT CBoss_Missile::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

void CBoss_Missile::Combat_Move(_double dTimeDelta)
{
	if (m_IsTargetCody == true)
	{
		_vector vTargetPos = ((CCody*)DATABASE->GetCody())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vTargetPos = XMVectorSetY(vTargetPos, vTargetPos.m128_f32[1] + 1.5f);
		_vector vRocketPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vRocketToTarget = vTargetPos - vRocketPos;
		_vector vNRocketToTarget = XMVector3Normalize(vRocketToTarget);
		vNRocketToTarget = XMVectorSetY(vNRocketToTarget, 0.f);

		_float fDist = XMVectorGetX(XMVector3Length(vRocketToTarget));
		if (fDist < 3.f)
		{
			m_bPlayerExplosion = true;
		}
		else
		{
			m_pTransformCom->Rotate_ToTarget(vTargetPos);
			m_pTransformCom->Move_ToTargetNoRotation(vTargetPos, dTimeDelta * fDist / 2.f);
		}
	}
	else
	{
		_vector vTargetPos = ((CMay*)DATABASE->GetMay())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vTargetPos = XMVectorSetY(vTargetPos, vTargetPos.m128_f32[1] + 1.5f);
		_vector vRocketPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vRocketToTarget = vTargetPos - vRocketPos;
		_vector vNRocketToTarget = XMVector3Normalize(vRocketToTarget);
		vNRocketToTarget = XMVectorSetY(vNRocketToTarget, 0.f);

		_float fDist = XMVectorGetX(XMVector3Length(vRocketToTarget));
		if (fDist < 3.f)
		{
			m_bPlayerExplosion = true;
		}
		else
		{
			m_pTransformCom->Rotate_ToTarget(vTargetPos);
			m_pTransformCom->Move_ToTargetNoRotation(vTargetPos, dTimeDelta * fDist / 2.f);
		}
	}

}

void CBoss_Missile::MayControl_Move(_double dTimeDelta)
{
	// 각도 제한 걸어야 함

#ifdef __CONTROL_MAY_KEYBOARD
	_vector vUFOPos = ((CUFO*)DATABASE->Get_BossUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMissilePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vUFOPos - vMissilePos;
	_float  fDist = XMVectorGetX(XMVector3Length(vDir));
	if (fDist < 4.f)
	{
		((CUFO*)DATABASE->Get_BossUFO())->Set_Who_Collide_Last(GameID::eMAY);
		m_bBossExplosion = true;
		return;
	}

	m_vPlayerOffSetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.13f);
	((CMay*)DATABASE->GetMay())->Set_RocketOffSetPos(m_vPlayerOffSetPosition);

	_matrix matRocket = XMMatrixIdentity();
	matRocket = m_pTransformCom->Get_WorldMatrix();
	((CMay*)DATABASE->GetMay())->Set_RocketMatrix(matRocket);


	if (m_bMayControl && ((CMay*)DATABASE->GetMay())->Get_CurState() == ANI_M_Rocket_MH)
	{

		if (m_fMoveAcceleration < 10.f)
			m_fMoveAcceleration += (_float)dTimeDelta * 4.f;

		_vector vUp = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
		_vector vRocketLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vUp, vRocketLook)));

		// m_pGameInstance->Get_Pad_LStickX() > 44000 (Right)
		// m_pGameInstance->Get_Pad_LStickX() < 20000 (Left)
		// m_pGameInstance->Get_Pad_LStickY() < 20000 (Down)
		// m_pGameInstance->Get_Pad_LStickY() > 44000 (Up)

		if (fDegree >= 15.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_DOWN))
			{
				if (m_fRotateAcceleration < 0.6f)
					m_fRotateAcceleration += (_float)dTimeDelta * 0.5f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (fDegree <= 165.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_UP))
			{
				if (m_fRotateAcceleration < 0.6f)
					m_fRotateAcceleration += (_float)dTimeDelta* 0.5f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			if (m_fRotateAcceleration < 0.6f)
				m_fRotateAcceleration += (_float)dTimeDelta* 0.5f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -dTimeDelta * m_fRotateAcceleration);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			if (m_fRotateAcceleration < 0.6f)
				m_fRotateAcceleration += (_float)dTimeDelta * 0.5f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * m_fRotateAcceleration);
		}


		if ((m_pGameInstance->Key_Up(DIK_W) || m_pGameInstance->Key_Up(DIK_S) || m_pGameInstance->Key_Up(DIK_A) || m_pGameInstance->Key_Up(DIK_D)))
			m_fRotateAcceleration = 0.f;

		m_pTransformCom->Go_Straight(dTimeDelta * m_fMoveAcceleration);
	}
#else

	_vector vUFOPos = ((CUFO*)DATABASE->Get_BossUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMissilePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vUFOPos - vMissilePos;
	_float  fDist = XMVectorGetX(XMVector3Length(vDir));
	if (fDist < 4.f)
	{
		((CUFO*)DATABASE->Get_BossUFO())->Set_Who_Collide_Last(GameID::eMAY);
		m_bBossExplosion = true;
		return;
	}

	m_vPlayerOffSetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.13f);
	((CMay*)DATABASE->GetMay())->Set_RocketOffSetPos(m_vPlayerOffSetPosition);

	_matrix matRocket = XMMatrixIdentity();
	matRocket = m_pTransformCom->Get_WorldMatrix();
	((CMay*)DATABASE->GetMay())->Set_RocketMatrix(matRocket);


	if (m_bMayControl && ((CMay*)DATABASE->GetMay())->Get_CurState() == ANI_M_Rocket_MH)
	{

		if (m_fMoveAcceleration < 10.f)
			m_fMoveAcceleration += dTimeDelta * 4.f;

		_vector vUp = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
		_vector vRocketLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vUp, vRocketLook)));

		// m_pGameInstance->Get_Pad_LStickX() > 44000 (Right)
		// m_pGameInstance->Get_Pad_LStickX() < 20000 (Left)
		// m_pGameInstance->Get_Pad_LStickY() < 20000 (Down)
		// m_pGameInstance->Get_Pad_LStickY() > 44000 (Up)

		if (fDegree >= 15.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_DOWN) || m_pGameInstance->Get_Pad_LStickY() > 44000)
			{
				if (m_fRotateAcceleration < 1.f)
					m_fRotateAcceleration += dTimeDelta * 2.f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (fDegree <= 165.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Get_Pad_LStickY() < 20000)
			{
				if (m_fRotateAcceleration < 1.f)
					m_fRotateAcceleration += dTimeDelta* 2.f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFT) || m_pGameInstance->Get_Pad_LStickX() < 20000)
		{
			if (m_fRotateAcceleration < 1.f)
				m_fRotateAcceleration += dTimeDelta* 2.f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -dTimeDelta * m_fRotateAcceleration);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT) || m_pGameInstance->Get_Pad_LStickX() > 44000)
		{
			if (m_fRotateAcceleration < 1.f)
				m_fRotateAcceleration += dTimeDelta * 2.f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * m_fRotateAcceleration);
		}


		if (m_pGameInstance->Get_Pad_LStickX() > 44000 || m_pGameInstance->Get_Pad_LStickX() < 20000 || m_pGameInstance->Get_Pad_LStickY() < 20000 || m_pGameInstance->Get_Pad_LStickY() > 44000)
			m_fRotateAcceleration = 0.f;


		m_pTransformCom->Go_Straight(dTimeDelta * m_fMoveAcceleration);
	}
#endif
}

void CBoss_Missile::CodyControl_Move(_double dTimeDelta)
{
	// 각도 제한 걸어야 함
	_vector vUFOPos = ((CUFO*)DATABASE->Get_BossUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMissilePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vUFOPos - vMissilePos;
	_float  fDist = XMVectorGetX(XMVector3Length(vDir));
	if (fDist < 4.f)
	{
		((CUFO*)DATABASE->Get_BossUFO())->Set_Who_Collide_Last(GameID::eCODY);
		m_bBossExplosion = true;
		return;
	}

	m_vPlayerOffSetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.13f);
	((CCody*)DATABASE->GetCody())->Set_RocketOffSetPos(m_vPlayerOffSetPosition);

	_matrix matRocket = XMMatrixIdentity();
	matRocket = m_pTransformCom->Get_WorldMatrix();
	((CCody*)DATABASE->GetCody())->Set_RocketMatrix(matRocket);


	if (m_bCodyControl && ((CCody*)DATABASE->GetCody())->Get_CurState() == ANI_C_Rocket_MH)
	{

		if (m_fMoveAcceleration < 10.f)
			m_fMoveAcceleration += (_float)dTimeDelta * 4.f;

		_vector vUp = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
		_vector vRocketLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vUp, vRocketLook)));


		if (fDegree >= 15.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_S))
			{
				if (m_fRotateAcceleration < 1.f)
					m_fRotateAcceleration += (_float)dTimeDelta * 2.f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (fDegree <= 165.f)
		{
			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				if (m_fRotateAcceleration < 1.f)
					m_fRotateAcceleration += (_float)dTimeDelta* 2.f;

				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * m_fRotateAcceleration);
			}
		}
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			if (m_fRotateAcceleration < 1.f)
				m_fRotateAcceleration += (_float)dTimeDelta* 2.f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -dTimeDelta * m_fRotateAcceleration);
		}
		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			if (m_fRotateAcceleration < 1.f)
				m_fRotateAcceleration += (_float)dTimeDelta * 2.f;

			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * m_fRotateAcceleration);
		}


		if (m_pGameInstance->Key_Up(DIK_W) || m_pGameInstance->Key_Up(DIK_S) || m_pGameInstance->Key_Up(DIK_A) || m_pGameInstance->Key_Up(DIK_D))
			m_fRotateAcceleration = 0.f;

		m_pTransformCom->Go_Straight(dTimeDelta * m_fMoveAcceleration);
	}

}

void CBoss_Missile::Falling(_double dTimeDelta)
{
	if (m_IsFalling == true)
	{
		_vector vRayStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		m_pGameInstance->Raycast(MH_PxVec3(vRayStartPos), MH_PxVec3(vRayDir), 50.f, m_RaycastBuffer, PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);

		if (m_RaycastBuffer.getNbAnyHits() > 0)
		{
			_float fDistance = m_RaycastBuffer.getAnyHit(0).distance;
			if (fDistance > 0.5f)
			{
				m_fSpeed += (_float)dTimeDelta * (GRAVITY / 2.f);
				m_pTransformCom->Go_Down(dTimeDelta * m_fSpeed);
			}
			else if (fDistance <= 0.5f)
			{
				m_fSpeed = 0.f;
				m_IsCrashed = true;
				m_IsFalling = false;
				m_bStartAdjustAngle = true;
			}
		}
	}
}

void CBoss_Missile::Adjust_Angle(_double dTimeDelta)
{
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fAngle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vUp, vLook)));

	if (fAngle < 90.f)
	{
		m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * 2.f);
	}
	else
	{
		m_bStartAdjustAngle = false;
		CTriggerActor::ARG_DESC tTriggerDesc;
		m_UserData = USERDATA(GameID::eBOSSMISSILE, this);
		tTriggerDesc.pUserData = &m_UserData;
		tTriggerDesc.pTransform = m_pTransformCom;
		tTriggerDesc.pGeometry = new PxSphereGeometry(6.f);
		CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &tTriggerDesc);

		Safe_Delete(tTriggerDesc.pGeometry);
		m_pTriggerActorCom->Update_TriggerActor();

		m_vPlayerOffSetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.13f);
		((CCody*)DATABASE->GetCody())->Set_RocketOffSetPos(m_vPlayerOffSetPosition);
		((CMay*)DATABASE->GetMay())->Set_RocketOffSetPos(m_vPlayerOffSetPosition);
	}
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
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
