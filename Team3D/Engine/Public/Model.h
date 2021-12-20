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
	/**
	* Get_Vertices
	* 현재 모델의 Vertex 정보들을 가져온다.
	* 애니메이션이 없는 모델만 해당.
	*/
	VTXMESH*	Get_Vertices() { return m_pVertices; }
	const _uint	Get_VertexCount() { return m_iVertexCount; }
	_fmatrix	Get_BoneMatrix(const char* pBoneName) const;
	const _uint	Get_CurAnimIndex() const { return m_iCurAnimIndex; }
	const _uint	Get_MeshCount() const { return m_iMeshCount; }
	_fmatrix	Get_AnimTransformation(_uint iIndex) const { return XMLoadFloat4x4(&m_AnimTransformations[iIndex]); }

	/**
	* Get_ProgressAnim
	* 현재 애니메이션 진행률
	* 0.f ~ 1.f
	*/
	const _float Get_ProgressAnim() const { return m_fProgressAnim; }
	/**
	* Is_AnimFinished
	* 특정 애니메이션이 종료되었는지 확인.
	*/
	const _bool Is_AnimFinished(_uint iAnimIndex) const { NULL_CHECK_RETURN(iAnimIndex < m_iAnimCount, false); return m_IsAnimFinished[iAnimIndex]; }
	/**
	* Get_PXTriMeshes
	* 모델의 메쉬별 TriMesh 정보를 담은 컨테이너를 가져온다.
	*/
	const vector<PX_TRIMESH>& Get_PXTriMeshes() { return m_PxTriMeshes; }

public: /* Setter */
	/**
	* Set_Animation
	* 애니메이션 교체
	* dAnimTime, dAnimTime이 지난 시점부터 재생
	*/
	HRESULT	Set_Animation(_uint iAnimIndex, _double dAnimTime = 0.0);
	/**
	* Set_NextAnimIndex
	* 다음 애니메이션 예약
	*/
	HRESULT	Set_NextAnimIndex(_uint iAnimIndex);
	/**
	* Set_MeshRenderGroup
	* 특정 메쉬의 렌더 그룹 지정.
	* 하나라도 세팅하면 다른 메쉬들도 그룹을 지정해주어야 한다.
	* Renderer에 추가할 때는 지정한 그룹에 모두 추가해주어야 한다.
	*/
	HRESULT Set_MeshRenderGroup(_uint iMeshIndex, RENDER_GROUP::Enum eGroup);
	/**
	* Set_Variable
	* 셰이더 상수 세팅.
	*/
	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	/**
	* Set_ShaderResourceView
	* 셰이더 텍스쳐 세팅.
	*/
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pShaderResourceView);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType, _uint iTextureIndex = 0);
	/**
	* Set_DefaultVariables_Perspective
	* 셰이더 디폴트 상수버퍼 세팅.
	*/
	HRESULT	Set_DefaultVariables_Perspective(_fmatrix WorldMatrix);
	/**
	* Set_DefaultVariables_Shadow
	* 셰이더 디폴트 그림자버퍼 세팅.
	*/
	HRESULT	Set_DefaultVariables_Shadow();
	/**
	* Set_DefaultVariables_ShadowDepth
	* 셰이더 디폴트 그림자깊이버퍼 세팅.
	*/
	HRESULT	Set_DefaultVariables_ShadowDepth(_fmatrix WorldMatrix);
	/**
	* Set_PivotTransformation
	* 노드 피벗 행렬 세팅
	*/
	HRESULT Set_PivotTransformation(_uint iIndex, _fmatrix PivotMatrix);
	/**
	* Set_PivotTransformation
	* 노드 피벗 행렬 초기화
	*/
	HRESULT Initialize_PivotTransformation();

public:
	virtual HRESULT	NativeConstruct_Prototype(const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char* pCenterBoneName);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	HRESULT			Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<MATERIAL*>& Materials, vector<class CHierarchyNode*>& Nodes, vector<_float4x4>& Transformations, vector<class CAnim*>& Anims);
	/* For.Client */
	/**
	* Add_LerpInfo
	* 특정 애니메이션 선형보간 정보 저장
	*/
	HRESULT Add_LerpInfo(_uint iCurAnimIndex, _uint iNextAnimIndex, _bool bGoingToLerp, _float fLerpSpeed = 5.f);
	/**
	* Update_Animation
	*/
	HRESULT	Update_Animation(_double dTimeDelta);
	/**
	* Culling
	* Render 하려면 반드시 먼저 호출.
	* return, 렌더할 ViewportInfo.
	*/
	_uint Culling(_fvector vPosition, _float fCullingRadius);
	/**
	* Render_Model
	* iMaterialSetNum, 세팅할 텍스쳐세트 인덱스
	* ShadowWrite == true, 텍스쳐 세팅안함.
	*/
	HRESULT	Render_Model(_uint iPassIndex, _uint iMaterialSetNum = 0, _bool bShadowWrite = false, RENDER_GROUP::Enum eGroup = RENDER_GROUP::RENDER_END);
	/**
	* Separated_Bind_Buffer
	* Sepd_Render_Model을 이용하는 경우 호출.
	*/
	HRESULT Sepd_Bind_Buffer();
	/**
	* Separated_Render_Model
	* 텍스쳐 외부에서 따로 연결해줘야함.
	* iMaterialIndex, 렌더할 머티리얼 그룹
	*/
	HRESULT	Sepd_Render_Model(_uint iMaterialIndex, _uint iPassIndex, _bool bShadowWrite = false, RENDER_GROUP::Enum eGroup = RENDER_GROUP::RENDER_END);

	/*이펙트 전용 렌더 Topology가 Point*/
	HRESULT	Render_Model_VERTEX(_uint iPassIndex, _uint iMaterialSetNum = 0, _bool bShadowWrite = false, RENDER_GROUP::Enum eGroup = RENDER_GROUP::RENDER_END);

	/* Model 컴포넌트 클론했을 때 피벗 바꿔야 할 경우 호출 */
	HRESULT	Change_PivotMatrix(_fmatrix PivotMatrix);

private: /* Typedef */
	typedef vector<class CMesh*>			MESHES;
	typedef vector<MATERIAL*>				MATERIALS;
	typedef	vector<class CHierarchyNode*>	NODES;
	typedef vector<class CAnim*>			ANIMS;
	typedef vector<_float4x4>				TRANSFORMATIONS;
	typedef vector<KEY_FRAME>				KEYFRAMES;
	typedef map<LERP_PAIR, LERP_INFO>		LERP_MAP;
private:
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
	TRANSFORMATIONS				m_PivotTransformations;					// 
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
	/* For.Check Bind Materials */
	_uint						m_IsBindMaterials[AI_TEXTURE_TYPE_MAX];
	/* For.MultiRenderGroup */
	_bool						m_bMultiRenderGroup			= false;
	/* For.Draw */
	_uint						m_iViewportDrawInfo			= 0;
private:
	HRESULT	Sort_MeshesByMaterial();
	HRESULT	Apply_PivotMatrix(_fmatrix PivotMatrix);
	HRESULT	Set_CenterBone(const char* pCenterBoneName = "");
	HRESULT	Store_TriMeshes();
	void	Update_AnimTransformations(_double dTimeDelta);
	void	Update_CombinedTransformations();
	HRESULT Is_BindMaterials(_uint iMaterialIndex);
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