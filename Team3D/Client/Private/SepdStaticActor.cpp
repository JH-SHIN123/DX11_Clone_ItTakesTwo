#include "stdafx.h"
#include "..\Public\SepdStaticActor.h"

CSepdStaticActor::CSepdStaticActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CSepdStaticActor::CSepdStaticActor(const CSepdStaticActor & rhs)
	: CActor(rhs)
{
}

HRESULT CSepdStaticActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSepdStaticActor::NativeConstruct(void * pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	CActor::NativeConstruct(pArg);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	const vector<CModel::PX_TRIMESH>& PxTriMeshes = ArgDesc.pModel->Get_PXTriMeshes();
	NULL_CHECK_RETURN(false == PxTriMeshes.empty(), E_FAIL);

	_uint iMeshCount = ArgDesc.pModel->Get_MeshCount();

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	for (_uint iMeshIndex = 0; iMeshIndex < iMeshCount; ++iMeshIndex)
	{
		PxRigidStatic* pActor = m_pPhysX->Create_StaticActor(MH_PxTransform(vRotQuat, vPosition), PxTriangleMeshGeometry(PxTriMeshes[iMeshIndex].pTriMesh, PxMeshScale(MH_PxVec3(vScale))));
		pActor->userData = &ArgDesc.arrUserData[iMeshIndex];
		m_pActors.emplace_back(pActor);
	}

	return S_OK;
}

void CSepdStaticActor::Update_StaticActor()
{
	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	for (auto& pActor : m_pActors)
		pActor->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
}

CSepdStaticActor * CSepdStaticActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSepdStaticActor* pInstance = new CSepdStaticActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSepdStaticActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CSepdStaticActor::Clone_Component(void * pArg)
{
	CSepdStaticActor* pInstance = new CSepdStaticActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSepdStaticActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSepdStaticActor::Free()
{
	if (true == m_isClone)
	{
		for (auto& pActor : m_pActors)
			m_pPhysX->Remove_Actor(&pActor);
		m_pActors.clear();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}
