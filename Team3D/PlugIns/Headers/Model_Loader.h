#ifndef Model_Loader_h__
#define Model_Loader_h__

#include "Base.h"

BEGIN(Engine)

class CModel_Loader final :public CBase 
{
	DECLARE_SINGLETON(CModel_Loader)
private:
	explicit CModel_Loader();
	virtual ~CModel_Loader() = default;

public:
	HRESULT	Load_ModelFromFile		(class CModel* pModel, const char* pMeshFilePath, const char* pMeshFileName);

private:
	typedef vector<class CMesh*>			MESHES;
	typedef	vector<class CHierarchyNode*>	NODES;
	typedef vector<BONE_DESC*>				BONES;
	typedef vector<class CAnim*>			ANIMS;
	typedef vector<class CAnimChannel*>		CHANNELS;
	typedef vector<KEY_FRAME*>				KEYFRAMES;
private:
	HRESULT					Add_MeshToContainer		(const aiScene* pScene, VTXMESH* pVertices, POLYGON_INDICES32* pFaces, MESHES& Meshes);
	HRESULT					Add_NodeToContainer		(aiNode* pAiNode, MESHES& Meshes, NODES& Nodes, const _uint& iAnimCount, class CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT					Add_BoneInfoToContainer	(const aiScene* pScene, VTXMESH* pVertices, MESHES& Meshes, NODES& Nodes);
	HRESULT					Add_AnimToContainer		(const aiScene* pScene, NODES& Nodes, ANIMS& Anims);
	void					Find_NodeCount			(aiNode* pAiNode, _uint & iCount);
	class CHierarchyNode*	Find_Node_ByName		(NODES& Nodes, const char* pNodeName);
	void					Set_NodeIndex			(NODES& Nodes);

public:
	virtual void	Free() override;
};

END

#endif // Model_Loader_h__