#include "..\public\HierarchyNode.h"
#include "AnimChannel.h"

CHierarchyNode::CHierarchyNode()
{
}

const char * CHierarchyNode::Get_Name() const
{
	return m_szNodeName;
}

const _uint CHierarchyNode::Get_Depth() const
{
	return m_iDepth;
}

const _int CHierarchyNode::Get_AnimChannelIndex(const _uint& iAnimIndex) const
{
	return m_AnimChannelIndex[iAnimIndex];
}

const _int CHierarchyNode::Get_NodeIndex() const
{
	return m_iNodeIndex;
}

const _int CHierarchyNode::Get_ParentNodeIndex() const
{
	if (nullptr != m_pParent)
		return m_pParent->m_iNodeIndex;

	return -1;
}

_fmatrix CHierarchyNode::Get_TransformationMatrix() const
{
	return XMLoadFloat4x4(&m_TransformationMatrix);
}

_fmatrix CHierarchyNode::Get_CombinedMatrix() const
{
	if (nullptr == m_pParent)
		return XMLoadFloat4x4(&m_TransformationMatrix);

	return XMLoadFloat4x4(&m_TransformationMatrix) * m_pParent->Get_CombinedMatrix();
}

HRESULT CHierarchyNode::Set_ConnectedAnimChannel(const _uint & iAnimIndex, const _uint & iChannelIndex)
{
	m_AnimChannelIndex[iAnimIndex] = iChannelIndex;

	return S_OK;
}

void CHierarchyNode::Set_NodeIndex(_int iIndex)
{
	m_iNodeIndex = iIndex;
}

HRESULT CHierarchyNode::NativeConstruct(char * pName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, const _uint & iDepth, const _uint & iAnimCount)
{
	strcpy_s(m_szNodeName, pName);

	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);

	m_pParent		= pParent;
	Safe_AddRef(m_pParent);

	m_iDepth		= iDepth;
	m_iNodeIndex	= -1;

	m_AnimChannelIndex.resize(iAnimCount, -1);

	return S_OK;
}

CHierarchyNode * CHierarchyNode::Create(char * pName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, const _uint & iDepth, const _uint & iAnimCount)
{
	CHierarchyNode*	pInstance = new CHierarchyNode;

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, pParent, iDepth, iAnimCount)))
	{
		MSG_BOX("Failed to Creating Instance(CHierarchyNode).");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	m_AnimChannelIndex.clear();

	Safe_Release(m_pParent);
}