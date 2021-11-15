#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	explicit CHierarchyNode() = default;
	virtual ~CHierarchyNode() = default;

public: /* Struct */
	typedef struct tagNodeDesc
	{
		_uint	iDepth;
		_int	iNodeIndex;
		_int	iParentNodeIndex;
		char	szNodeName[MAX_PATH];
	}NODE_DESC;

public: /* Getter */
	const char*	Get_Name() const { return m_NodeDesc.szNodeName; }
	const _int	Get_NodeIndex() const { return m_NodeDesc.iNodeIndex; }
	const _int	Get_ParentNodeIndex() const { return m_NodeDesc.iParentNodeIndex; }

public:
	HRESULT	NativeConstruct(NODE_DESC NodeDesc);

private:
	NODE_DESC m_NodeDesc;

public:
	static CHierarchyNode* Create(NODE_DESC NodeDesc);
	virtual void Free() override;
};

END