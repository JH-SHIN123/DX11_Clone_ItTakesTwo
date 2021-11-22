#include "..\Public\TriggerActor.h"
#include "PhysX.h"
#include "Transform.h"
#include "GameObject.h"

CTriggerActor::CTriggerActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CTriggerActor::CTriggerActor(const CTriggerActor & rhs)
	: CActor(rhs)
{
}

HRESULT CTriggerActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTriggerActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	m_pActor = m_pPhysX->Create_TriggerActor(MH_PxTransform(vRotQuat, vPosition), *ArgDesc.pGeometry);
	m_pActor->userData = ArgDesc.pUserData;

	return S_OK;
}

void CTriggerActor::Update_TriggerActor()
{
	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	m_pActor->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
}

CTriggerActor * CTriggerActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTriggerActor* pInstance = new CTriggerActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTriggerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTriggerActor::Clone_Component(void * pArg)
{
	CTriggerActor* pInstance = new CTriggerActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTriggerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerActor::Free()
{
	if (true == m_isClone)
		m_pPhysX->Remove_Actor(&m_pActor);

	Safe_Release(m_pTransform);

	CActor::Free();
}
