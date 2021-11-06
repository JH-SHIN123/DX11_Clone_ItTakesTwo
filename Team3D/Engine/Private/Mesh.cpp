#include "..\public\Mesh.h"
#include "HierarchyNode.h"

CMesh::CMesh()
{
}

const _uint CMesh::Get_StartVertexIndex() const
{
	return m_iStratVertexIndex;
}

const _uint CMesh::Get_StratFaceIndex() const
{
	return m_iStratFaceIndex;
}

const _uint CMesh::Get_StratFaceCount() const
{
	return m_iStratFaceCount;
}

const _uint CMesh::Get_MaterialIndex() const
{
	return m_iMaterialIndex;
}

const _uint CMesh::Get_BoneCount() const
{
	return m_iBoneCount;
}

_fvector CMesh::Get_MeshColor() const
{
	return XMLoadFloat4(&m_vColor);
}

HRESULT CMesh::NativeConstruct(const char * pMeshName, const _uint & iStartVertexIndex, const _uint & iStartFaceIndex, const _uint & iFaceCount, const _uint & iMaterialIndex, _fvector vColor)
{
	strcpy_s(m_szMeshName, pMeshName);

	m_iStratVertexIndex = iStartVertexIndex;
	m_iStratFaceIndex	= iStartFaceIndex;
	m_iStratFaceCount	= iFaceCount;
	m_iMaterialIndex	= iMaterialIndex;

	XMStoreFloat4(&m_vColor, vColor);

	return S_OK;
}

HRESULT CMesh::Bring_BoneContainer(vector<BONE_DESC*>& Bones)
{
	NULL_CHECK_RETURN(m_Bones.empty(), E_FAIL);

	m_iBoneCount = (_uint)Bones.size();

	m_Bones.swap(Bones);

	return S_OK;
}

void CMesh::Calc_BoneMatrices(_matrix * pBoneMatrices, const vector<_float4x4> & CombinedTransformations)
{
	_uint	iIndex = 0;

	for (auto& pBoneDesc : m_Bones)
		pBoneMatrices[iIndex++] = XMMatrixTranspose(XMLoadFloat4x4(&pBoneDesc->OffsetMatrix) * XMLoadFloat4x4(&CombinedTransformations[pBoneDesc->pHierarchyNode->Get_NodeIndex()]));
}

CMesh * CMesh::Create(const char * pMeshName, const _uint & iStartVertexIndex, const _uint & iStartFaceIndex, const _uint & iFaceCount, const _uint & iMaterialIndex, _fvector vColor)
{
	CMesh*	pInstance	= new CMesh;

	if (FAILED(pInstance->NativeConstruct(pMeshName, iStartVertexIndex, iStartFaceIndex, iFaceCount, iMaterialIndex, vColor)))
	{
		MSG_BOX("Failed to Creating Instance(CMesh).");
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
