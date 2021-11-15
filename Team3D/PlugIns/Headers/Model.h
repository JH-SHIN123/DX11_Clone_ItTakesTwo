#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public: /* Struct */
	typedef struct tagPxTriMesh
	{
		PxVec3*				pVertices;
		POLYGON_INDICES32*	pFaces;
		PxTriangleMesh*		pTriMesh;
	}PX_TRIMESH;
	typedef struct tagLerpPair
	{
		_uint iCurAnimIndex;
		_uint iNextAnimIndex;
		
		bool operator<(const tagLerpPair &var) const
		{
			if (iCurAnimIndex != var.iCurAnimIndex)
				return iCurAnimIndex < var.iCurAnimIndex;
			return iNextAnimIndex < var.iNextAnimIndex;
		}

		tagLerpPair() {}
		tagLerpPair(_uint _iCurAnimIndex, _uint _iNextAnimIndex) : iCurAnimIndex(_iCurAnimIndex), iNextAnimIndex(_iNextAnimIndex) {}
	}LERP_PAIR;
	typedef struct tagLerpInfo
	{
		_bool	bGoingToLerp;
		_float	fLerpSpeed;
	}LERP_INFO;

public: /* Getter */
	VTXMESH*		Get_Vertices() { return m_pVertices; }
	const _uint		Get_VertexCount() { return m_iVertexCount; }
	_fmatrix		Get_BoneMatrix(const char* pBoneName) const;
	const _uint		Get_CurAnimIndex() const { return m_iCurAnimIndex; }
	const _float	Get_ProgressAnim() const { return m_fProgressAnim; }
	const _bool		Is_AnimFinished(_uint iAnimIndex) const { NULL_CHECK_RETURN(iAnimIndex < m_iAnimCount, false); return m_IsAnimFinished[iAnimIndex]; }

public: /* Setter */
	HRESULT	Set_Animation(_uint iAnimIndex);
	HRESULT	Set_NextAnimIndex(_uint iAnimIndex);
	void Set_Pivot(_fmatrix PivotMatrix);
	/* For.Shader */
	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pShaderResourceView);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType, _uint iTextureIndex = 0);
	HRESULT	Set_DefaultVariables_Perspective(_fmatrix WorldMatrix);

public:
	virtual HRESULT	NativeConstruct_Prototype(const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char* pCenterBoneName);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	/* For.ModelLoader */
	HRESULT	Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<MATERIAL*>& Materials, vector<class CHierarchyNode*>& Nodes, vector<_float4x4>& Transformations, vector<class CAnim*>& Anims);
	/* For.Client */
	HRESULT Add_LerpInfo(_uint iCurAnimIndex, _uint iNextAnimIndex, _bool bGoingToLerp, _float fLerpSpeed = 5.f);
	HRESULT	Update_Animation(_double dTimeDelta);
	HRESULT	Render_Model(_uint iPassIndex, _uint iMaterialSetNum = 0);

private: /* Typedef */
	typedef vector<class CMesh*>			MESHES;
	typedef vector<MATERIAL*>				MATERIALS;
	typedef	vector<class CHierarchyNode*>	NODES;
	typedef vector<class CAnim*>			ANIMS;
	typedef vector<_float4x4>				TRANSFORMATIONS;
	typedef vector<KEY_FRAME>				KEYFRAMES;
	typedef map<LERP_PAIR, LERP_INFO>		LERP_MAP;
private:
	class CModel_Loader*		m_pModel_Loader				= nullptr;
	VTXMESH*					m_pVertices					= nullptr;
	POLYGON_INDICES32*			m_pFaces					= nullptr;
	MESHES						m_Meshes;
	MATERIALS					m_Materials;
	NODES						m_Nodes;
	ANIMS						m_Anims;
	_uint						m_iMeshCount				= 0;
	_uint						m_iMaterialCount			= 0;
	_uint						m_iNodeCount				= 0;
	_uint						m_iAnimCount				= 0;
	vector<MESHES>				m_SortedMeshes;
	_float4x4					m_PivotMatrix;
	_float4x4					m_CombinedPivotMatrix;
	/* For.Animating */
	_double						m_dCurrentTime				= 0.0;		// 현재 애니메이션 진행시간
	_uint						m_iCurAnimIndex				= 0;		// 현재 애니메이션 인덱스
	_uint						m_iNextAnimIndex			= 0;		// 다음 애니메이션 인덱스
	_uint						m_iCurAnimFrame				= 0;		// 현재 애니메이션 프레임
	TRANSFORMATIONS				m_BaseTransformations;					// 원래 노드의 TransformationMatrix, 바뀌면 안됨.
	TRANSFORMATIONS				m_AnimTransformations;					// 채널의 Transformation이 채워진 TransformationMatrix
	TRANSFORMATIONS				m_CombinedTransformations;				// 계산된 CombinedTransformationMatrix
	_bool						m_bReserveNextAnim			= false;	// 다음 애니메이션 예약이 되어있으면 true
	_float						m_fProgressAnim				= 0.f;		// 애니메이션 진행도
	/* For.Lerp */
	_float						m_fLerpRatio				= 0.f;
	_float						m_fLerpSpeed				= 0.f;
	KEYFRAMES					m_PreAnimKeyFrames;						// 이전 애니메이션 키프레임들
	LERP_MAP					m_LerpMap;
	/* For.FinishCheck */
	vector<_bool>				m_IsAnimFinished;						// 애니메이션 종료 확인
	/* For.Adjust_Center */
	_bool						m_bNeedCenterBone			= false;	// 중심 뼈 존재 여부
	class CHierarchyNode*		m_pCenterBoneNode			= nullptr;	// 중심 뼈 이름
	_float4						m_vAnimDistFromCenter		= _float4(0.f, 0.f, 0.f, 0.f);
	/* For.PhyX */
	vector<PX_TRIMESH>			m_PxTriMeshes;
	/* For.MaterialSet */
	_uint						m_iMaterialSetCount			= 0;
private:
	HRESULT	Sort_MeshesByMaterial();
	HRESULT	Set_CenterBone(const char* pCenterBoneName = "");
	HRESULT	Apply_PivotMatrix(_fmatrix PivotMatrix);
	HRESULT	Store_TriMeshes();
	void	Update_AnimTransformations(_double dTimeDelta);
	void	Update_CombinedTransformations();

#pragma region For_Buffer
private: /* For.Buffer */
	/* For.Vertices */
	ID3D11Buffer*				m_pVB = nullptr;
	_uint						m_iVertexCount = 0;
	_uint						m_iVertexStride = 0;
	_uint						m_iVertexBufferCount = 0;
	/* For.Indices */
	ID3D11Buffer*				m_pIB = nullptr;
	_uint						m_iFaceCount = 0;
	_uint						m_iFaceStride = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	/* For.Shader */
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts;
private:
	/* For.Buffer */
	HRESULT		Create_Buffer(ID3D11Buffer** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void* pSysMem, _uint iSysMemPitch = 0, _uint iSysMemSlicePitch = 0);
	HRESULT		Create_VIBuffer(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT		SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC* pInputElementDesc, _uint iElementCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
#pragma endregion

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount = 1, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bNeedCenterBone = false, const char* pCenterBoneName = "");
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END