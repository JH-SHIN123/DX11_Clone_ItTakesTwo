#include "stdafx.h"
#include "..\Public\RunningMoonBaboon.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "Moon.h"
#include "PixelCrossHair.h"
#include "PixelUFO.h"

CRunningMoonBaboon::CRunningMoonBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRunningMoonBaboon::CRunningMoonBaboon(const CRunningMoonBaboon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRunningMoonBaboon::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRunningMoonBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eRUNNINGMOONBABOON;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	Add_LerpInfo();
	m_pModelCom->Set_Animation(Moon_Run);
	m_pModelCom->Set_NextAnimIndex(Moon_Run);
	
	DATABASE->Set_RunningMoonBaboon(this);

	m_pTransformCom->Set_Speed(0.f, 45.f);
	return S_OK;
}

_int CRunningMoonBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_IsLaserHit == false)
	{
		_vector vRayPosition = (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
		_vector vRayDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 10.f + (-m_pTransformCom->Get_State(CTransform::STATE_UP)));
		m_pGameInstance->Raycast(MH_PxVec3(vRayPosition), MH_PxVec3(vRayDir), 40.f, m_RaycastBuffer, PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION | PxHitFlag::eNORMAL);
		if (m_RaycastBuffer.getNbAnyHits() > 0)
		{
			_float4 HitPoint = MH_XMFloat4(m_RaycastBuffer.getAnyHit(0).position, 1.f);
			_vector vPosition = XMLoadFloat4(&HitPoint);

			_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
			_vector vDir = XMVector3Normalize(vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_vector vHitNormal = XMVector3Normalize(XMLoadFloat4(&MH_XMFloat4(m_RaycastBuffer.getAnyHit(0).normal, 0.f)));
			PxVec3 a = m_RaycastBuffer.getAnyHit(0).normal;

			_float fCCW = XMVectorGetX(XMVector3Dot(vUp, XMVector3Cross(vDir, -vHitNormal)));

			if (fCCW > 0)
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), -dTimeDelta / 10.f);
			else
				m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), dTimeDelta / 10.f);

			m_pDynamicActorCom->Get_Actor()->clearForce();
			Movement(dTimeDelta);
		}
		else
			Movement(dTimeDelta);

	}
	else
	{
		if (m_bHitOnceAnim == false)
		{
			m_pModelCom->Set_Animation(Moon_JetPack_MH);
			m_pModelCom->Set_NextAnimIndex(Moon_JetPack_Flying_Start);
			m_bHitOnceAnim = true;
		}
		LaserHit_Movement(dTimeDelta);
	}

	Calculate_Matrix(dTimeDelta);

	
	m_pModelCom->Update_Animation(dTimeDelta);
	return NO_EVENT;
}

_int CRunningMoonBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1500.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRunningMoonBaboon::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CRunningMoonBaboon::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}

void CRunningMoonBaboon::Movement(_double dTimeDelta)
{
	_vector vLook	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vUp		= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vRight	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * MOONBABOONFORCE , XMVectorGetY(vLook) * MOONBABOONFORCE, XMVectorGetZ(vLook) * MOONBABOONFORCE));
}

void CRunningMoonBaboon::Calculate_Matrix(_double dTimeDelta)
{
	PxMat44 pxMat = PxMat44(m_pDynamicActorCom->Get_Actor()->getGlobalPose());
	_vector vPosition = XMVectorSet(pxMat.column3.x, pxMat.column3.y, pxMat.column3.z, 1.f);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	vUp = XMVector3Normalize(vPosition - ((CMoon*)(DATABASE->Get_Mooon()))->Get_Position());
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
	
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);	
}

void CRunningMoonBaboon::Add_LerpInfo()
{
	m_pModelCom->Add_LerpInfo(Moon_Run, Moon_Run, false);
}

void CRunningMoonBaboon::LaserHit_Movement(_double dTimeDelta)
{
	if (m_IsLaserHit == true)
	{
		_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

		m_fJetPackTime += (_float)dTimeDelta;

		if (m_fShieldTime < 14.f && m_fJetPackTime < 5.f)
		{
			m_fShieldTime += (_float)dTimeDelta * 14.f;
			PxTransform NextTransform = PxTransform(PxVec3(0.f, 59.f + m_fShieldTime, 0.f));
			m_pJoint->setLocalPose(PxJointActorIndex::eACTOR0, NextTransform);
		}

		if (m_fJetPackTime > 1.f && m_fJetPackTime < 7.f)
		{	
			if(m_fJetAcceleration < 5.5f)
				m_fJetAcceleration += (_float)dTimeDelta * 4.f;

			if (m_bJetPackAnimOnce == false)
			{
				m_pModelCom->Set_Animation(Moon_JetPack_Flying_Start);
				m_pModelCom->Set_NextAnimIndex(Moon_JetPack_Flying);
				m_bJetPackAnimOnce = true;
			}
			m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * MOONBABOONFORCE * m_fJetAcceleration, XMVectorGetY(vLook) * MOONBABOONFORCE * m_fJetAcceleration, XMVectorGetZ(vLook) * MOONBABOONFORCE * m_fJetAcceleration));
		}

		if (m_fJetPackTime >= 7.f)
		{
			if (m_fShieldTime > 0.f)
			{
				m_fShieldTime -= (_float)dTimeDelta * 14.f;
				PxTransform NextTransform = PxTransform(PxVec3(0.f, 59.f + m_fShieldTime, 0.f));
				m_pJoint->setLocalPose(PxJointActorIndex::eACTOR0, NextTransform);
			}
			else
			{
				m_pModelCom->Set_Animation(Moon_JetPackToLanding);
				m_pModelCom->Set_NextAnimIndex(Moon_Run);
				PxTransform NextTransform = PxTransform(PxVec3(0.f, 59.f, 0.f));
				m_pJoint->setLocalPose(PxJointActorIndex::eACTOR0, NextTransform);
				m_IsLaserHit = false;
				m_bHitOnceAnim = false;
				m_bJetPackAnimOnce = false;
				m_fShieldTime = 0.f;
				m_fJetPackTime = 0.f;
			}

		
		}
	}
}

HRESULT CRunningMoonBaboon::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	m_pModelCom = (CModel*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"));
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	m_pModelCom->Change_PivotMatrix(PivotMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(800.f, 1000.f, 2000.f, 1.f));

	/* Dynamic */
	PxGeometry* Geom = new PxSphereGeometry(0.5f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);
	m_pDynamicActorCom->Get_Actor()->setLinearDamping(1.5f);
	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//m_pDynamicActorCom->Get_Actor()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, true);

	/*PxShape* pShape = nullptr; 
	m_pDynamicActorCom->Get_Actor()->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);*/

	/* Joint */
	PxJointLimitCone LimitCone = PxJointLimitCone(PxPi, PxPi, 0.01f);
	m_pJoint = CPhysX::GetInstance()->Create_Joint(m_pDynamicActorCom->Get_Actor(), PxTransform(PxVec3(0.f, 59.f, 0.f)), nullptr, PxTransform(MH_PxVec3(((CMoon*)(DATABASE->Get_Mooon()))->Get_Position())), LimitCone, false);

	return S_OK;
}

CRunningMoonBaboon * CRunningMoonBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRunningMoonBaboon* pInstance = new CRunningMoonBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRunningMoonBaboon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRunningMoonBaboon::Clone_GameObject(void * pArg)
{
	CRunningMoonBaboon* pInstance = new CRunningMoonBaboon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRunningMoonBaboon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRunningMoonBaboon::Free()
{
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}