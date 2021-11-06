#ifndef Mesh_h__
#define Mesh_h__

#include "Base.h"

BEGIN(Engine)

class CMesh final : public CBase
{
private:
	explicit CMesh();
	virtual ~CMesh() = default;

public: /* Getter */
	const _uint	Get_StartVertexIndex	() const;
	const _uint	Get_StratFaceIndex		() const;
	const _uint	Get_StratFaceCount		() const;
	const _uint	Get_MaterialIndex		() const;
	const _uint	Get_BoneCount			() const;
	_fvector	Get_MeshColor			() const;

public:
	HRESULT	NativeConstruct		(const char* pMeshName, const _uint& iStartVertexIndex, const _uint& iStartFaceIndex, const _uint& iFaceCount, const _uint& iMaterialIndex, _fvector vColor);
	HRESULT	Bring_BoneContainer	(vector<BONE_DESC*>& Bones);
	void	Calc_BoneMatrices	(_matrix* pBoneMatrices, const vector<_float4x4>& CombinedTransformations);

private:
	typedef vector<BONE_DESC*>	BONES;
private:
	BONES		m_Bones;
	char		m_szMeshName[MAX_PATH]	= "";
	_uint		m_iStratVertexIndex		= 0;
	_uint		m_iStratFaceIndex		= 0;
	_uint		m_iStratFaceCount		= 0;
	_uint		m_iMaterialIndex		= 0;
	_uint		m_iBoneCount			= 0;
	_float4		m_vColor				= {1.f, 1.f, 1.f ,1.f};

public:
	static CMesh*	Create	(const char* pMeshName, const _uint& iStartVertexIndex, const _uint& iStartFaceIndex, const _uint& iFaceCount, const _uint& iMaterialIndex, _fvector vColor);
	virtual void	Free	() override;
};

END

#endif // Mesh_h__
