#include "..\Public\ControllableActor.h"
#include "PhysX.h"
#include "Transform.h"

CControllableActor::CControllableActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CControllableActor::CControllableActor(const CControllableActor & rhs)
	: CActor(rhs)
{
}

HRESULT CControllableActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControllableActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);
	NULL_CHECK_RETURN(ArgDesc.pTransform, E_FAIL);
	m_pTransform = ArgDesc.pTransform;
	Safe_AddRef(m_pTransform);


	PxCapsuleControllerDesc CapsuleControllerDesc;

	//CapsuleControllerDesc.density = 10.f;
	//CapsuleControllerDesc.scaleCoeff = 0.8f;
	//CapsuleControllerDesc.invisibleWallHeight = 0.f;
	//CapsuleControllerDesc.maxJumpHeight = 10.f;
	//CapsuleControllerDesc.reportCallback = NULL;
	//CapsuleControllerDesc.behaviorCallback = NULL;
	//CapsuleControllerDesc.volumeGrowth = 1.5f;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.height = 0.5f;
	CapsuleControllerDesc.radius = 0.5f;
	CapsuleControllerDesc.material = m_pPhysX->Create_Material(0.5f, 0.5f, 0.5f);
	CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	CapsuleControllerDesc.contactOffset = 0.01f;
	CapsuleControllerDesc.stepOffset = 0.5f;
	CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	CapsuleControllerDesc.slopeLimit = 0.707f;
	CapsuleControllerDesc.position = PxExtendedVec3(0.0, 0.5, 0.0);

	m_pController = m_pPhysX->Create_CapsuleController(CapsuleControllerDesc);
	NULL_CHECK_RETURN(m_pController, E_FAIL);
	m_pActor = m_pController->getActor();
	//m_pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);

	//if (m_pActor->getNbShapes())
	//{
	//	PxShape* Shape;
	//	m_pActor->getShapes(&Shape, 1);
	//	Shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	//}

	m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);

	m_fJumpGravity = -9.8f;

	return S_OK;
}

void CControllableActor::Move(_fvector vMove, _double dTimeDelta)
{
	m_pController->move(MH_PxVec3(vMove), 0.0001f, (_float)dTimeDelta, PxControllerFilters());
}

void CControllableActor::Update(_double dTimeDelta)
{
	m_fHeightDelta = Get_Height(dTimeDelta);
	_float fY;

	if (m_fHeightDelta != 0.f)
		fY = m_fHeightDelta * 0.5f;
	else
		fY = -GRAVITY * (_float)dTimeDelta;


	PxVec3 vDist = PxVec3(0, fY, 0);
	PxU32 iFlags = m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());

	if (PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags)
		Jump_Stop();

	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
}

void CControllableActor::Jump_Start(_float fJumpForce)
{
	if (m_bJump)
		return;

	m_fJumpTime = 0.f;
	m_fJumpForce = fJumpForce;
	m_fBaseJumpForce = fJumpForce;
	m_bJump = true;
}

void CControllableActor::Jump_Higher(_float fJumpForce)
{
	if (!m_bJump || m_fHeightDelta <= 0.f)
		return;

	if (m_fBaseJumpForce * 1.2f < m_fJumpForce)
		return;

	m_fJumpForce += fJumpForce;
}

void CControllableActor::Jump_Stop()
{
	if (!m_bJump)
		return;

	m_bJump = false;
}

_float CControllableActor::Get_Height(_double dTimeDelta)
{
	if (!m_bJump)
		return 0.f;
	
	m_fJumpTime += (_float)dTimeDelta;
	return (m_fJumpGravity / 2.f * m_fJumpTime * m_fJumpTime + m_fJumpForce * m_fJumpTime)/* * (_float)dTimeDelta*/;
}

CControllableActor * CControllableActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControllableActor* pInstance = new CControllableActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControllableActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CControllableActor::Clone_Component(void * pArg)
{
	CControllableActor* pInstance = new CControllableActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControllableActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControllableActor::Free()
{
	if (true == m_isClone)
		m_pController->release();

	Safe_Release(m_pTransform);

	CActor::Free();
}
