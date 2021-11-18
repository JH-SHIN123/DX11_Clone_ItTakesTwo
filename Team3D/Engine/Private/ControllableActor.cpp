#include "..\Public\ControllableActor.h"
#include "PhysX.h"
#include "Transform.h"
#include "PxControllerCallback.h"

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

	m_pCallback = new CPxControllerCallback;
	ArgDesc.CapsuleControllerDesc.behaviorCallback = m_pCallback;

	m_pController = m_pPhysX->Create_CapsuleController(ArgDesc.CapsuleControllerDesc);
	NULL_CHECK_RETURN(m_pController, E_FAIL);
	m_pActor = m_pController->getActor();


	//m_fJumpGravity = ArgDesc.fJumpGravity;
	Setup_PxFiltering(m_pActor, FilterGroup::ePLAYER, FilterGroup::eSTATIC | FilterGroup::eDYNAMIC);

	return S_OK;
}

void CControllableActor::Move(_fvector vMove, _double dTimeDelta)
{
	m_pController->move(MH_PxVec3(vMove), 0.001f, (_float)dTimeDelta, PxControllerFilters());
}

void CControllableActor::Update(_double dTimeDelta)
{
	m_fHeightDelta = Get_Height(dTimeDelta);
	_float fY;


	if (m_fHeightDelta != 0.f)
	{
		fY = m_fHeightDelta * 0.5f;
	}
	else
	{
		fY = m_fGravity * (_float)dTimeDelta;
		if(m_bJump == true)
			m_IsFalling = true;
	}

	PxVec3 vDist = PxVec3(0, fY, 0);
	PxU32 iFlags = m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());

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
		Safe_Delete(m_pCallback);
		m_pController->release();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}
