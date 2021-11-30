#include "..\Public\DynamicActor.h"
#include "PhysX.h"
#include "Transform.h"
#include "GameObject.h"

CDynamicActor::CDynamicActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CDynamicActor::CDynamicActor(const CDynamicActor & rhs)
	: CActor(rhs)
{
}

PxRigidDynamic *& CDynamicActor::Get_Actor()
{
	return m_pActor;
}

HRESULT CDynamicActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDynamicActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	m_pActor = m_pPhysX->Create_DynamicActor(MH_PxTransform(vRotQuat, vPosition), *ArgDesc.pGeometry, ArgDesc.fDensity, ArgDesc.vVelocity);
	m_pActor->userData = ArgDesc.pUserData;

	return S_OK;
}

void CDynamicActor::Update_DynamicActor()
{
	m_pTransform->Set_WorldMatrix(MH_XMMatrix(PxMat44(m_pActor->getGlobalPose())));
}

void CDynamicActor::Update_DynamicActor(_float fDis)
{
	_matrix World = MH_XMMatrix(PxMat44(m_pActor->getGlobalPose()));

	_float4x4 Temp;
	XMStoreFloat4x4(&Temp, World);
	Temp._42 -= fDis;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&Temp));
}

CDynamicActor * CDynamicActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDynamicActor* pInstance = new CDynamicActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDynamicActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CDynamicActor::Clone_Component(void * pArg)
{
	CDynamicActor* pInstance = new CDynamicActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDynamicActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicActor::Free()
{
	if (true == m_isClone)
		m_pPhysX->Remove_Actor(&m_pActor);

	Safe_Release(m_pTransform);

	CActor::Free();
}