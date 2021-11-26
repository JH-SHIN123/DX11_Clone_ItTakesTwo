#include "stdafx.h"
#include "..\Public\CameraActor.h"

#include "CameraBehaviorCallback.h"
#include "CameraFilterCallback.h"

CCameraActor::CCameraActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CCameraActor::CCameraActor(const CCameraActor & rhs)
	: CActor(rhs)
{
}

void CCameraActor::Set_Position(_fvector vPosition)
{
	m_pController->setPosition(MH_PxExtendedVec3(vPosition));
}

HRESULT CCameraActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCameraActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	m_pBehaviorCallback = new CCameraBehaviorCallback;
	m_pFilterCallback = new CCameraFilterCallback;
	m_pFilters = new PxControllerFilters(0, 0, m_pFilterCallback);

	PxCapsuleControllerDesc CapsuleControllerDesc = PxCapsuleControllerDesc();
	CapsuleControllerDesc.height = 1.f;
	CapsuleControllerDesc.radius = 1.f;
	CapsuleControllerDesc.behaviorCallback = m_pBehaviorCallback;
	CapsuleControllerDesc.material = m_pGameInstance->Get_BasePxMaterial();
	CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransform->Get_State(CTransform::STATE_POSITION));
	CapsuleControllerDesc.contactOffset = 0.01f;
	CapsuleControllerDesc.stepOffset = 0.707f;
	CapsuleControllerDesc.slopeLimit = 0.f;

	m_pBehaviorCallback->Set_CameraActorPtr(this);
	m_pFilterCallback->Set_Controller(m_pController);
	m_pController = m_pPhysX->Create_CapsuleController(CapsuleControllerDesc);
	m_pActor = m_pController->getActor();
	m_pActor->userData = ArgDesc.pUserData;
	m_pController->setUserData(ArgDesc.pUserData);

	m_eID = ArgDesc.pUserData->eID;

	return S_OK;
}

void CCameraActor::Move(_fvector vMove, _double dTimeDelta)
{
	m_pController->move(MH_PxVec3(vMove), 0.001f, (_float)dTimeDelta, *m_pFilters);
}

void CCameraActor::Update(_double dTimeDelta)
{


	//m_pTransform->Set_WorldMatrix();
	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getPosition(), 1.f));
}

void CCameraActor::Set_CorrectPosition()
{
	if (m_eID == GameID::eMAINCAMERA)
	{
		if (nullptr != DATABASE->GetCody())
		{
			PxRaycastBuffer RaycastBuffer;

			_vector vPlayerPos = DATABASE->GetCody()->Get_Position();
			_vector vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vDir = vCameraPos - vPlayerPos;
			_float	fDist = XMVectorGetX(XMVector3Length(vDir));
			vPlayerPos += vDir;
			vDir = XMVector3Normalize(vDir);

			if (m_pGameInstance->Raycast(MH_PxVec3(vPlayerPos), MH_PxVec3(vDir), fDist, RaycastBuffer, PxHitFlag::ePOSITION))
				m_pController->setPosition(MH_PxExtendedVec3(RaycastBuffer.block.position));
		}
	}
	else if (m_eID == GameID::eSUBCAMERA)
	{
		if (nullptr != DATABASE->GetMay())
		{
			PxRaycastBuffer RaycastBuffer;

			_vector vPlayerPos = DATABASE->GetMay()->Get_Position();
			_vector vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vDir = vCameraPos - vPlayerPos;
			_float	fDist = XMVectorGetX(XMVector3Length(vDir));
			vDir = XMVector3Normalize(vDir);

			if (m_pGameInstance->Raycast(MH_PxVec3(vPlayerPos), MH_PxVec3(vDir), fDist, RaycastBuffer, PxHitFlag::ePOSITION))
				m_pController->setPosition(MH_PxExtendedVec3(RaycastBuffer.block.position));
		}
	}
}

CCameraActor * CCameraActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCameraActor* pInstance = new CCameraActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CCameraActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCameraActor::Clone_Component(void * pArg)
{
	CCameraActor* pInstance = new CCameraActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CCameraActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraActor::Free()
{
	if (true == m_isClone)
	{
		Safe_Delete(m_pFilters);
		Safe_Delete(m_pFilterCallback);
		Safe_Delete(m_pBehaviorCallback);
		m_pPhysX->Remove_Actor(&m_pTrigger);
		m_pController->release();
	}
	Safe_Release(m_pTransform);

	CActor::Free();
}