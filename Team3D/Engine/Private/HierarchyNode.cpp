#include "..\public\HierarchyNode.h"

HRESULT CHierarchyNode::NativeConstruct(NODE_DESC NodeDesc)
{
	memcpy(&m_NodeDesc, &NodeDesc, sizeof(NODE_DESC));

	return S_OK;
}

CHierarchyNode * CHierarchyNode::Create(NODE_DESC NodeDesc)
{
	CHierarchyNode* pInstance = new CHierarchyNode;

	if (FAILED(pInstance->NativeConstruct(NodeDesc)))
	{
		MSG_BOX("Failed to Create Instance - CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
}