#pragma once

#include "Base.h"

BEGIN(Engine)

class CMesh final : public CBase
{
private:
	explicit CMesh() = default;
	virtual ~CMesh() = default;

public: /* Struct */
	typedef struct tagMeshDesc
	{
		_uint	iStratVertexIndex;
		_uint	iStratFaceIndex;
		_uint	iStratFaceCount;
		_uint	iMaterialIndex;	
		_uint	iBoneCount;
		char	szMeshName[MAX_PATH];
	}MESH_DESC;

public: /* Getter */
	const _uint		Get_StartVertexIndex() const { return m_MeshDesc.iStratVertexIndex; }
	const _uint		Get_StratFaceIndex() const { return m_MeshDesc.iStratFaceIndex; }
	const _uint		Get_StratFaceCount() const { return m_MeshDesc.iStratFaceCount; }
	const _uint		Get_MaterialIndex() const { return m_MeshDesc.iMaterialIndex; }

public:
	HRESULT	NativeConstruct(MESH_DESC MeshDesc);
	HRESULT	Bring_BoneContainer(vector<BONE_DESC>& Bones);
	void	Calc_BoneMatrices(_matrix* pBoneMatrices, const vector<_float4x4>& CombinedTransformations);

private: /* Typedef */
	typedef vector<BONE_DESC> BONES;
private:
	BONES		m_Bones;
	MESH_DESC	m_MeshDesc;

public:
	static CMesh* Create(MESH_DESC MeshDesc);
	virtual void Free() override;
};

END