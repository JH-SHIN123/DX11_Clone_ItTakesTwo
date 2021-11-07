#include "..\public\HierarchyNode.h"

const _int CHierarchyNode::Get_ParentNodeIndex() const
{
	if (nullptr != m_pParent)
		return m_pParent->m_iNodeIndex;

	return -1;
}

HRESULT CHierarchyNode::NativeConstruct(char * pName, CHierarchyNode * pParent, _uint iDepth)
{
	strcpy_s(m_szNodeName, pName);

	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	m_iDepth		= iDepth;
	m_iNodeIndex	= -1;

	return S_OK;
}

CHierarchyNode * CHierarchyNode::Create(char * pName, CHierarchyNode * pParent, _uint iDepth)
{
	CHierarchyNode* pInstance = new CHierarchyNode;

	if (FAILED(pInstance->NativeConstruct(pName, pParent, iDepth)))
	{
		MSG_BOX("Failed to Create Instance - CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}