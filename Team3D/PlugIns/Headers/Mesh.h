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
		_uint	iFaceCount;
		_uint	iMaterialIndex;	
		_uint	iBoneCount;
		char	szMeshName[MAX_PATH];
	}MESH_DESC;

public: /* Getter */
	const _uint					Get_StartVertexIndex() const { return m_MeshDesc.iStratVertexIndex; }
	const _uint					Get_StratFaceIndex() const { return m_MeshDesc.iStratFaceIndex; }
	const _uint					Get_FaceCount() const { return m_MeshDesc.iFaceCount; }
	const _uint					Get_MaterialIndex() const { return m_MeshDesc.iMaterialIndex; }
	const RENDER_GROUP::Enum	Get_RenderGroup() const { return m_eRenderGroup; }

public: /* Setter */
	void Set_RenderGroup(RENDER_GROUP::Enum eGroup) { m_eRenderGroup = eGroup; }

public:
	HRESULT	NativeConstruct(MESH_DESC MeshDesc);
	HRESULT	Bring_BoneContainer(vector<BONE_DESC>& Bones);
	void	Calc_BoneMatrices(_matrix* pBoneMatrices, const vector<_float4x4>& CombinedTransformations);

private: /* Typedef */
	typedef vector<BONE_DESC> BONES;
private:
	BONES				m_Bones;
	MESH_DESC			m_MeshDesc;
	RENDER_GROUP::Enum	m_eRenderGroup = RENDER_GROUP::RENDER_END;

public:
	static CMesh* Create(MESH_DESC MeshDesc);
	virtual void Free() override;
};

END