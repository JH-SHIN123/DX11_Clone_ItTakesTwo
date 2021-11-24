#include "..\Public\ControllableActor.h"
#include "PhysX.h"
#include "Transform.h"
#include "PxControllerCallback.h"
#include "PxControllerFilterCallback.h"
#include "GameObject.h"

CControllableActor::CControllableActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CControllableActor::CControllableActor(const CControllableActor & rhs)
	: CActor(rhs)
{
}

void CControllableActor::Set_Scale(_float fRadius, _float fHeight)
{
	m_pController->resize(fHeight);
	static_cast<PxCapsuleController*>(m_pController)->setRadius(fRadius);
}

void CControllableActor::Set_Position(_fvector vPosition)
{
	m_pController->setPosition(MH_PxExtendedVec3(vPosition));
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

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	m_pCallback = new CPxControllerCallback;
	m_pFilterCallback = new CPxControllerFilterCallback;
	m_pFilters = new PxControllerFilters(0, 0, m_pFilterCallback);
	ArgDesc.CapsuleControllerDesc.behaviorCallback = m_pCallback;

	m_pController = m_pPhysX->Create_CapsuleController(ArgDesc.CapsuleControllerDesc);
	m_pCallback->Set_Controller(m_pController);
	m_pActor = m_pController->getActor();
	m_pActor->userData = ArgDesc.pUserData;

	//m_fJumpGravity = ArgDesc.fJumpGravity;

	return S_OK;
}

void CControllableActor::Move(_fvector vMove, _double dTimeDelta)
{
	m_pController->move(MH_PxVec3(vMove), 0.001f, (_float)dTimeDelta, *m_pFilters);
}

void CControllableActor::Update(_double dTimeDelta)
{
	m_fHeightDelta = Get_Height(dTimeDelta);
	_float fY;

	if (m_fHeightDelta != 0.f)
		fY = m_fHeightDelta * 0.5f;
	else
		fY = m_fGravity * (_float)dTimeDelta;

	PxVec3 vDist = PxVec3(0, fY, 0);
	PxU32 iFlags = m_pController->move(vDist, 0.f, (_float)dTimeDelta, *m_pFilters);

	if (PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags)
		Jump_Stop();

	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
}

void CControllableActor::Update_Cam(_double dTimeDelta)
{
	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getPosition(), 1.f));
}

void CControllableActor::Jump_Start(_float fJumpForce)
{
	/*if (m_bJump)
		return;*/

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
	return (m_fGravity / 2.f * m_fJumpTime * m_fJumpTime + m_fJumpForce * m_fJumpTime)/* * (_float)dTimeDelta*/;
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
	{
		Safe_Delete(m_pFilters);
		Safe_Delete(m_pFilterCallback);
		Safe_Delete(m_pCallback);
		m_pPhysX->Remove_Actor(&m_pTrigger);
		m_pController->release();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}
