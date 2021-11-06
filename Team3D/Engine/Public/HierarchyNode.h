#ifndef HierarchyNode_h__
#define HierarchyNode_h__

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	explicit CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public: /* Getter */
	const char*		Get_Name					() const;
	const _uint		Get_Depth					() const;
	const _int		Get_NodeIndex				() const;
	const _int		Get_ParentNodeIndex			() const;
	const _int		Get_AnimChannelIndex		(const _uint& iAnimIndex) const;
	_fmatrix		Get_TransformationMatrix	() const;
	_fmatrix		Get_CombinedMatrix			() const;

public: /* Setter */
	HRESULT	Set_ConnectedAnimChannel	(const _uint& iAnimIndex, const _uint& iChannelIndex);
	void	Set_NodeIndex				(_int iIndex);

public:
	HRESULT	NativeConstruct		(char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, const _uint& iDepth, const _uint& iAnimCount);

private:
	typedef vector<_int>	CHANNELINDEX_BY_ANIMS;
private:
	CHANNELINDEX_BY_ANIMS	m_AnimChannelIndex;
	char					m_szNodeName[MAX_PATH]	= "";
	CHierarchyNode*			m_pParent				= nullptr;
	_uint					m_iDepth				= 0;
	_float4x4				m_TransformationMatrix;
	_int					m_iNodeIndex			= 0;

public:
	static CHierarchyNode*	Create	(char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, const _uint& iDepth, const _uint& iAnimCount);
	virtual void			Free	() override;
};

END

#endif // HierarchyNode_h__
