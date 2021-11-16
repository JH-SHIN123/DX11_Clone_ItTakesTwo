#include "..\Public\StaticActor.h"
#include "PhysX.h"

CStaticActor::CStaticActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CStaticActor::CStaticActor(const CStaticActor & rhs)
	: CActor(rhs)
{
}

HRESULT CStaticActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStaticActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, ArgDesc.WorldMatrix);

	m_pActor = m_pPhysX->Create_StaticActor(MH_PxTransform(vRotQuat, vPosition), *ArgDesc.pGeometry, ArgDesc.pMaterial, ArgDesc.pActorName);
	NULL_CHECK_RETURN(m_pActor, E_FAIL);

	PxShape* Shape;
	m_pActor->getShapes(&Shape, 1);
	Shape->setContactOffset(0.02f);
	Shape->setRestOffset(-0.5f);

	m_pPhysX->Add_ActorToScene(m_pActor);

	return S_OK;
}

CStaticActor * CStaticActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStaticActor* pInstance = new CStaticActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CStaticActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CStaticActor::Clone_Component(void * pArg)
{
	CStaticActor* pInstance = new CStaticActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CStaticActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticActor::Free()
{
	if (true == m_isClone)
		m_pPhysX->Remove_Actor(&m_pActor);

	CActor::Free();
}
