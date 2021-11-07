#pragma once

#include "Base.h"

BEGIN(Engine)

class CMesh final : public CBase
{
private:
	explicit CMesh() = default;
	virtual ~CMesh() = default;

public: /* Getter */
	const _uint	Get_StartVertexIndex() const { return m_iStratVertexIndex; }
	const _uint	Get_StratFaceIndex() const { return m_iStratFaceIndex; }
	const _uint	Get_StratFaceCount() const { return m_iStratFaceCount; }
	const _uint	Get_MaterialIndex() const { return m_iMaterialIndex; }
	const _uint	Get_BoneCount() const { return m_iBoneCount; }

public:
	HRESULT	NativeConstruct(const char* pMeshName, _uint iStartVertexIndex, _uint iStartFaceIndex, _uint iFaceCount, _uint iMaterialIndex);
	HRESULT	Bring_BoneContainer(vector<BONE_DESC*>& Bones);
	void	Calc_BoneMatrices(_matrix* pBoneMatrices, const vector<_float4x4>& CombinedTransformations);

private: /* Typedef */
	typedef vector<BONE_DESC*> BONES;
private:
	BONES	m_Bones;
	char	m_szMeshName[MAX_PATH] = "";
	_uint	m_iStratVertexIndex = 0;
	_uint	m_iStratFaceIndex = 0;
	_uint	m_iStratFaceCount = 0;
	_uint	m_iMaterialIndex = 0;
	_uint	m_iBoneCount = 0;

public:
	static CMesh* Create(const char* pMeshName, _uint iStartVertexIndex, _uint iStartFaceIndex, _uint iFaceCount, _uint iMaterialIndex);
	virtual void Free() override;
};

END