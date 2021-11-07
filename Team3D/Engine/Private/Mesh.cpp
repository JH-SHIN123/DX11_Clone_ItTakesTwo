#include "..\public\Mesh.h"
#include "HierarchyNode.h"

HRESULT CMesh::NativeConstruct(const char * pMeshName, _uint iStartVertexIndex, _uint iStartFaceIndex, _uint iFaceCount, _uint iMaterialIndex)
{
	strcpy_s(m_szMeshName, pMeshName);

	m_iStratVertexIndex = iStartVertexIndex;
	m_iStratFaceIndex	= iStartFaceIndex;
	m_iStratFaceCount	= iFaceCount;
	m_iMaterialIndex	= iMaterialIndex;

	return S_OK;
}

HRESULT CMesh::Bring_BoneContainer(vector<BONE_DESC*> & Bones)
{
	NULL_CHECK_RETURN(m_Bones.empty(), E_FAIL);

	m_iBoneCount = (_uint)Bones.size();

	m_Bones.swap(Bones);

	return S_OK;
}

void CMesh::Calc_BoneMatrices(_matrix * pBoneMatrices, const vector<_float4x4> & CombinedTransformations)
{
	_uint iIndex = 0;

	for (auto& pBoneDesc : m_Bones)
		pBoneMatrices[iIndex++] = XMMatrixTranspose(XMLoadFloat4x4(&pBoneDesc->OffsetMatrix) * XMLoadFloat4x4(&CombinedTransformations[pBoneDesc->pHierarchyNode->Get_NodeIndex()]));
}

CMesh * CMesh::Create(const char * pMeshName, _uint iStartVertexIndex, _uint iStartFaceIndex, _uint iFaceCount, _uint iMaterialIndex)
{
	CMesh* pInstance = new CMesh;

	if (FAILED(pInstance->NativeConstruct(pMeshName, iStartVertexIndex, iStartFaceIndex, iFaceCount, iMaterialIndex)))
	{
		MSG_BOX("Failed to Create Instance - CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone->pHierarchyNode);
		Safe_Delete(pBone);
	}
	m_Bones.clear();
}