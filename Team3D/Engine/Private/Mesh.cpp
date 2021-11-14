#include "..\public\Mesh.h"

HRESULT CMesh::NativeConstruct(MESH_DESC MeshDesc)
{
	memcpy(&m_MeshDesc, &MeshDesc, sizeof(MESH_DESC));

	return S_OK;
}

HRESULT CMesh::Bring_BoneContainer(vector<BONE_DESC> & Bones)
{
	NULL_CHECK_RETURN(m_Bones.empty(), E_FAIL);

	m_MeshDesc.iBoneCount = (_uint)Bones.size();

	m_Bones.swap(Bones);

	return S_OK;
}

void CMesh::Calc_BoneMatrices(_matrix * pBoneMatrices, const vector<_float4x4> & CombinedTransformations)
{
	_uint iIndex = 0;

	for (auto& BoneDesc : m_Bones)
		pBoneMatrices[iIndex++] = XMMatrixTranspose(XMLoadFloat4x4(&BoneDesc.OffsetMatrix) * XMLoadFloat4x4(&CombinedTransformations[BoneDesc.iNodeIndex]));
}

CMesh * CMesh::Create(MESH_DESC MeshDesc)
{
	CMesh* pInstance = new CMesh;

	if (FAILED(pInstance->NativeConstruct(MeshDesc)))
	{
		MSG_BOX("Failed to Create Instance - CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	m_Bones.clear();
}