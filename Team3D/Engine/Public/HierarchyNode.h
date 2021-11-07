#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	explicit CHierarchyNode() = default;
	virtual ~CHierarchyNode() = default;

public: /* Getter */
	const char*	Get_Name() const { return m_szNodeName; }
	const _uint	Get_Depth() const { return m_iDepth; }
	const _int	Get_NodeIndex() const { return m_iNodeIndex; }
	const _int	Get_ParentNodeIndex() const;

public: /* Setter */
	void Set_NodeIndex(_int iIndex) { m_iNodeIndex = iIndex; }

public:
	HRESULT	NativeConstruct(char* pName, CHierarchyNode* pParent, _uint iDepth);

private:
	char				m_szNodeName[MAX_PATH] = "";
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;
	_int				m_iNodeIndex = 0;

public:
	static CHierarchyNode* Create(char* pName, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;
};

END