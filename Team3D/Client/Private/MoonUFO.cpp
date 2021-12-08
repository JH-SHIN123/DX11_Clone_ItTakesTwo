#include "stdafx.h"
#include "..\Public\MoonUFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"

CMoonUFO::CMoonUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonUFO::CMoonUFO(const CMoonUFO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonUFO::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonUFO::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eMOONUFO;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_eNextState = UFO_MH;
	m_pModelCom->Set_Animation(ANI_UFO_MH);
	m_pModelCom->Set_NextAnimIndex(ANI_UFO_MH);

	CDataStorage::GetInstance()->Set_UFOPtr(this);

	return S_OK;
}

_int CMoonUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	KeyInPut(dTimeDelta);
	Check_State(dTimeDelta);
	Change_State(dTimeDelta);
	During_Animation_Behavior(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);
	//m_pDynamicActorCom->Update_DynamicActor();
	Test(dTimeDelta);

	return NO_EVENT;
}

_int CMoonUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	//m_pDynamicActorCom->Update_DynamicActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CMoonUFO::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CMoonUFO::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}

void CMoonUFO::KeyInPut(_double dTimeDelta)
{
	_vector vLook, vUp, vRight;
	vLook	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	vUp		= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	vRight	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * 10000.f, XMVectorGetY(vRight) * 10000.f, XMVectorGetZ(vRight) * 10000.f));
		m_bRotateRight = true;
	}
	else
		m_bRotateRight = false;
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * -10000.f, XMVectorGetY(vRight)  * -10000.f, XMVectorGetZ(vRight) * -10000.f));
		m_bRotateLeft = true;
	}
	else
		m_bRotateLeft = false;
	if (m_pGameInstance->Key_Pressing(DIK_UP))
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * 10000.f, XMVectorGetY(vLook) * 10000.f, XMVectorGetZ(vLook) * 10000.f));
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * -10000.f, XMVectorGetY(vLook) * -10000.f, XMVectorGetZ(vLook) * -10000.f));
}

CMoonUFO::UFO_STATE CMoonUFO::Check_State(_double dTimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		switch (m_eNextState)
		{
		case Client::CMoonUFO::UFO_ENTIRE_ANIMATION:
			m_pModelCom->Set_Animation(0);
			break;
		case Client::CMoonUFO::UFO_REF:
			m_pModelCom->Set_Animation(ANI_UFO_REF);
			break;
		case Client::CMoonUFO::UFO_BACK:
			m_pModelCom->Set_Animation(ANI_UFO_BACK);
			break;
		case Client::CMoonUFO::UFO_CODYHOLDING:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING);
			break;
		case Client::CMoonUFO::UFO_CODYHOLDING_ENTER:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING_ENTER);
			break;
		case Client::CMoonUFO::UFO_CODYHOLDING_LOW:
			m_pModelCom->Set_Animation(ANI_UFO_CODYHOLDING_LOW);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_ADDITIVE:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_ADDITIVE_BOOST:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE_BOOST);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_ADDITIVE_FLYING:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_ADDITIVE_FLYING);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_BCK:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_BCK);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_FWD);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_LEFT);
			break;
		case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_CONTROLLABLE_POSE_RIGHT);
			break;
		case Client::CMoonUFO::UFO_FIREROCKET_ADDITIVE_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_FIREROCKET_ADDITIVE_LEFT);
			break;
		case Client::CMoonUFO::UFO_FIREROCKET_ADDITIVE_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_FIREROCKET_ADDITIVE_RIGHT);
			break;
		case Client::CMoonUFO::UFO_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_FWD);
			break;
		case Client::CMoonUFO::UFO_GROUNDPOUND:
			m_pModelCom->Set_Animation(ANI_UFO_GROUNDPOUND);
			break;
		case Client::CMoonUFO::UFO_HITREACTION_BCK:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_BCK);
			break;
		case Client::CMoonUFO::UFO_HITREACTION_FWD:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_FWD);
			break;
		case Client::CMoonUFO::UFO_HITREACTION_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_LEFT);
			break;
		case Client::CMoonUFO::UFO_HITREACTION_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_HITREACTION_RIGHT);
			break;
		case Client::CMoonUFO::UFO_KNOCKDOWNMH:
			m_pModelCom->Set_Animation(ANI_UFO_KNOCKDOWNMH);
			break;
		case Client::CMoonUFO::UFO_LASER_HITPOD:
			m_pModelCom->Set_Animation(ANI_UFO_LASER_HITPOD);
			break;
		case Client::CMoonUFO::UFO_LASER_MH:
			m_pModelCom->Set_Animation(ANI_UFO_LASER_MH);
			break;
		case Client::CMoonUFO::UFO_LASERRIPPEDOFF:
			m_pModelCom->Set_Animation(ANI_UFO_LASERRIPPEDOFF);
			break;
		case Client::CMoonUFO::UFO_LEFT:
			m_pModelCom->Set_Animation(ANI_UFO_LEFT);
			break;
		case Client::CMoonUFO::UFO_MH:
			m_pModelCom->Set_Animation(ANI_UFO_MH);
			break;
		case Client::CMoonUFO::UFO_RIGHT:
			m_pModelCom->Set_Animation(ANI_UFO_RIGHT);
			break;
		case Client::CMoonUFO::UFO_ROCKETKNOCKDOWN_MH:
			m_pModelCom->Set_Animation(ANI_UFO_ROCKETKNOCKDOWN_MH);
			break;
		case Client::CMoonUFO::CUTSCENE_EJECT_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_EJECT_FLYINGSAUCER);
			break;
		case Client::CMoonUFO::CUTSCENE_ENTERUFO_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_ENTERUFO_FLYINGSAUCER);
			break;
		case Client::CMoonUFO::CUTSCENE_UFO_BOSS_INTRO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_BOSS_INTRO);
			break;
		case Client::CMoonUFO::CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER);
			break;
		case Client::CMoonUFO::CUTSCENE_UFO_OUTRO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_UFO_OUTRO);
			break;
		case Client::CMoonUFO::CUTSCENE_POWERCORESDESTROYED_UFO:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_POWERCORESDESTROYED_UFO);
			break;
		case Client::CMoonUFO::CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER:
			m_pModelCom->Set_Animation(ANI_CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER);
			break;
		default:
			break;
		}
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CMoonUFO::Change_State(_double dTimeDelta)
{
	if (m_eCurState == UFO_LASER_MH)
	{
		m_eNextState = UFO_LASER_HITPOD;
	}
}

void CMoonUFO::During_Animation_Behavior(_double dTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CMoonUFO::UFO_REF:
		break;
	case Client::CMoonUFO::UFO_BACK:
		break;
	case Client::CMoonUFO::UFO_CODYHOLDING:
		break;
	case Client::CMoonUFO::UFO_CODYHOLDING_ENTER:
		break;
	case Client::CMoonUFO::UFO_CODYHOLDING_LOW:
		break;
	case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_BCK:
		break;
	case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_FWD:
		break;
	case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_LEFT:
		break;
	case Client::CMoonUFO::UFO_CONTROLLABLE_POSE_RIGHT:
		break;
	case Client::CMoonUFO::UFO_FWD:
		break;
	case Client::CMoonUFO::UFO_GROUNDPOUND:
		break;
	case Client::CMoonUFO::UFO_HITREACTION_BCK:
		break;
	case Client::CMoonUFO::UFO_HITREACTION_FWD:
		break;
	case Client::CMoonUFO::UFO_HITREACTION_LEFT:
		break;
	case Client::CMoonUFO::UFO_HITREACTION_RIGHT:
		break;
	case Client::CMoonUFO::UFO_KNOCKDOWNMH:
		break;
	case Client::CMoonUFO::UFO_LASER_HITPOD:
		break;
	case Client::CMoonUFO::UFO_LASER_MH:
		Laser_Pattern(dTimeDelta);
		break;
	case Client::CMoonUFO::UFO_LASERRIPPEDOFF:
		break;
	case Client::CMoonUFO::UFO_LEFT:
		break;
	case Client::CMoonUFO::UFO_MH:
		break;
	case Client::CMoonUFO::UFO_RIGHT:
		break;
	case Client::CMoonUFO::UFO_ROCKETKNOCKDOWN_MH:
		break;
	default:
		break;
	}
}

void CMoonUFO::Laser_Pattern(_double dTimeDelta)
{
	_vector vDir = m_pCodyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_vector vDirForLaser = XMVector3Normalize(vDir);

	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f); // 플레이어 쪽으로 천천히 회전.
}

void CMoonUFO::Test(_double dTimeDelta)
{
	_vector vPosition, vRight, vUp, vLook;

	PxMat44 pxMat = PxMat44(m_pDynamicActorCom->Get_Actor()->getGlobalPose());

	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vRight	  = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vUp		  = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vLook	  = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	vUp = XMVector3Normalize(vPosition - XMVectorSet(0.f, 50.f, 0.f, 1.f));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pxMat.column3.x, pxMat.column3.y, pxMat.column3.z, pxMat.column3.w));

	if (true == m_bRotateRight)
		m_pTransformCom->RotateYaw(dTimeDelta);
	if (true == m_bRotateLeft)
		m_pTransformCom->RotateYaw(-dTimeDelta);
}

HRESULT CMoonUFO::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	/* Dynamic */
	PxGeometry* Geom = new PxSphereGeometry(2.f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);
	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	/* Joint */
	PxJointLimitCone LimitCone = PxJointLimitCone(PxPi, PxPi, 0.05f);
	m_pJoint = CPhysX::GetInstance()->Create_Joint(m_pDynamicActorCom->Get_Actor(), PxTransform(PxVec3(0.f, 30.f, 0.f)), nullptr, PxTransform(PxVec3(0.f, 50.f, 0.f)), LimitCone, false);

	/* Player Transform */
	m_pCodyTransform = ((CCody*)CDataStorage::GetInstance()->GetCody())->Get_Transform();
	if (nullptr == m_pCodyTransform)
		return E_FAIL;
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataStorage::GetInstance()->GetMay())->Get_Transform();
	if (nullptr == m_pMayTransform)
		return E_FAIL;
	Safe_AddRef(m_pMayTransform);

	return S_OK;
}

CMoonUFO * CMoonUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonUFO* pInstance = new CMoonUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoonUFO::Clone_GameObject(void * pArg)
{
	CMoonUFO* pInstance = new CMoonUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoonUFO::Free()
{
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}