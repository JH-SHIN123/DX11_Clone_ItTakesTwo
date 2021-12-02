#include "..\Public\StaticActor.h"
#include "PhysX.h"
#include "Transform.h"
#include "Model.h"
#include "GameObject.h"

CStaticActor::CStaticActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CStaticActor::CStaticActor(const CStaticActor & rhs)
	: CActor(rhs)
{
}

void CStaticActor::Set_Simulation(_bool bCheck)
{
	for (auto& pActor : m_pActors)
		pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, bCheck);
}

HRESULT CStaticActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStaticActor::NativeConstruct(void * pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	CActor::NativeConstruct(pArg);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	const vector<CModel::PX_TRIMESH>& PxTriMeshes = ArgDesc.pModel->Get_PXTriMeshes();
	NULL_CHECK_RETURN(PxTriMeshes.size(), E_FAIL);

	for (auto& TriMesh : PxTriMeshes)
	{
		PxRigidStatic* pActor = m_pPhysX->Create_StaticActor(MH_PxTransform(vRotQuat, vPosition), PxTriangleMeshGeometry(TriMesh.pTriMesh, PxMeshScale(MH_PxVec3(vScale))));
		pActor->userData = ArgDesc.pUserData;
		m_pActors.emplace_back(pActor);
	}

	return S_OK;
}

void CStaticActor::Update_StaticActor()
{
	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransform->Get_WorldMatrix());

	for (auto& pActor : m_pActors)
		pActor->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
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
	{
		for (auto& pActor : m_pActors)
			m_pPhysX->Remove_Actor(&pActor);
		m_pActors.clear();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}
