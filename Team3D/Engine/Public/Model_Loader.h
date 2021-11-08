#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel_Loader final :public CBase 
{
	DECLARE_SINGLETON(CModel_Loader)
private:
	explicit CModel_Loader() = default;
	virtual ~CModel_Loader() = default;

public:
	HRESULT	Load_ModelFromFile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, class CModel* pModel, const char* pModelFilePath, const char* pModelFileName);

private: /* Typedef */
	typedef vector<class CMesh*>					MESHES;
	typedef vector<MATERIAL*>						MATERIALS;
	typedef	vector<class CHierarchyNode*>			NODES;
	typedef vector<BONE_DESC*>						BONES;
	typedef vector<class CAnim*>					ANIMS;
	typedef vector<class CAnimChannel*>				CHANNELS;
	typedef vector<KEY_FRAME*>						KEYFRAMES;
	typedef vector<_float4x4>						TRANSFORMATIONS;
	typedef pair<class CHierarchyNode*, _float4x4>	TEMPNODE;
	typedef vector<TEMPNODE>						TEMPNODES;
private:
	HRESULT					Add_MeshToContainer(const aiScene* pScene, VTXMESH* pVertices, POLYGON_INDICES32* pFaces, MESHES& Meshes);
	HRESULT					Add_MaterialToContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const aiScene* pScene, const char* pModelFilePath, MATERIALS& Materials);
	HRESULT					Add_NodeToContainer(aiNode* pAiNode, MESHES& Meshes, TEMPNODES& TempNodes, class CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT					Add_BoneInfoToContainer(const aiScene* pScene, VTXMESH* pVertices, MESHES& Meshes, NODES& Nodes);
	HRESULT					Add_AnimToContainer(const aiScene* pScene, NODES& Nodes, ANIMS& Anims);
	void					Find_NodeCount(aiNode* pAiNode, _uint& iCount);
	class CHierarchyNode*	Find_Node_ByName(NODES& Nodes, const char* pNodeName);
	void					Set_NodeIndex(TEMPNODES& TempNodes, NODES& Nodes, TRANSFORMATIONS& Transformations);

public:
	virtual void Free() override;
};

END