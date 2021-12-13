#include "stdafx.h"
#include "..\Public\MoonUFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "Moon.h"

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

	m_pModelCom->Set_Animation(UFO_MH);
	m_pModelCom->Set_NextAnimIndex(UFO_MH);

	DATABASE->Set_MoonUFO(this);

	m_pTransformCom->Set_Speed(0.f, 45.f);

	return S_OK;
}

_int CMoonUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == m_IsMayInUFO)
	{
		KeyInPut(dTimeDelta);
		Calculate_Matrix(dTimeDelta);
	}

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CMoonUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	//if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	//	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CMoonUFO::Render(RENDER_GROUP::Enum eGroup)
{
	if (false == m_IsMayInUFO)
		return S_OK;

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
	_vector vLook	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vUp		= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vRight	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * UFOFORCE, XMVectorGetY(vRight) * UFOFORCE, XMVectorGetZ(vRight) * UFOFORCE));
		m_bRotateRight = true;
	}
	else
		m_bRotateRight = false;

	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * -UFOFORCE, XMVectorGetY(vRight)  * -UFOFORCE, XMVectorGetZ(vRight) * -UFOFORCE));
		m_bRotateLeft = true;
	}
	else
		m_bRotateLeft = false;

	if (m_pGameInstance->Key_Pressing(DIK_UP))
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * UFOFORCE, XMVectorGetY(vLook) * UFOFORCE, XMVectorGetZ(vLook) * UFOFORCE));

	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * -UFOFORCE, XMVectorGetY(vLook) * -UFOFORCE, XMVectorGetZ(vLook) * -UFOFORCE));
}

void CMoonUFO::Calculate_Matrix(_double dTimeDelta)
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

	if (true == m_bRotateRight)
		m_pTransformCom->RotateYaw_Speed(dTimeDelta);
	if (true == m_bRotateLeft)
		m_pTransformCom->RotateYaw_Speed(-dTimeDelta);
}

HRESULT CMoonUFO::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	/* Dynamic */
	PxGeometry* Geom = new PxSphereGeometry(5.f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);

	PxShape* pShape = nullptr;
	m_pDynamicActorCom->Get_Actor()->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	m_pDynamicActorCom->Get_Actor()->setLinearDamping(0.2f);
	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	/* Joint */
	PxJointLimitCone LimitCone = PxJointLimitCone(PxPi, PxPi, 0.05f);
	m_pJoint = CPhysX::GetInstance()->Create_Joint(m_pDynamicActorCom->Get_Actor(), PxTransform(PxVec3(0.f, 90.f, 0.f)), nullptr, PxTransform(MH_PxVec3(((CMoon*)(DATABASE->Get_Mooon()))->Get_Position())), LimitCone, false);
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
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}