#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance final : public CComponent
{
private:
	explicit CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		_float4x4*		pWorldMatrices;
		_uint			iInstanceCount;
		_float			fCullingRadius;

		tagArgumentDesc() {}
		tagArgumentDesc(_float4x4* _pWorldMatrices, _uint _iInstanceCount, _float _fCullingRadius) : pWorldMatrices(_pWorldMatrices), iInstanceCount(_iInstanceCount), fCullingRadius(_fCullingRadius) {}
	}ARG_DESC;
	typedef struct tagPxTriMesh
	{
		PxVec3*				pVertices;
		POLYGON_INDICES32*	pFaces;
		PxTriangleMesh*		pTriMesh;
	}PX_TRIMESH;

public: /* Setter */
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
	HRESULT	Set_DefaultVariables_Perspective();
	/**
	* Set_DefaultVariables_Shadow
	* 셰이더 디폴트 그림자버퍼 세팅.
	*/
	HRESULT	Set_DefaultVariables_Shadow();
	/**
	* Set_DefaultVariables_ShadowDepth
	* 셰이더 디폴트 그림자깊이버퍼 세팅.
	*/
	HRESULT	Set_DefaultVariables_ShadowDepth();

public:
	virtual HRESULT	NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char* pCenterBoneName, _bool bCreateActor);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	HRESULT			Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<MATERIAL*>& Materials);
	/* Client */
	/**
	* Culling
	* Render 하려면 반드시 먼저 호출.
	* return, Culling되지 않은 인스턴스 수
	*/
	_uint Culling();
	/**
	* Update_Model
	* 모델 전체 이동
	* TransformMatrix, 부모 월드 행렬
	*/
	HRESULT Update_Model(_fmatrix TransformMatrix);
	/**
	* Render_Model
	* iMaterialSetNum, 세팅할 텍스쳐세트 인덱스
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
	/**
	* Copy_WorldMatrix
	* 갱신할 월드 행렬 동적 배열과 인스턴싱 개수.
	*/
	void Copy_WorldMatrix(_float4x4* pWorldMatrix_Buffer, _uint iCopyInstanceIndex);
	/**
	* Set_RealTimeMatrices
	* GPU에 바인딩할 월드행렬 세팅
	* 컬링된 수만큼만 복사함 - 컬링 이후에 호출되어야함
	*/
	HRESULT Set_RealTimeMatrices(_float4x4* pWorldMatrices);

private: /* Typedef */
	typedef vector<class CMesh*>	MESHES;
	typedef vector<MATERIAL*>		MATERIALS;
private:
	class CModel_Loader*	m_pModel_Loader = nullptr;
	VTXMESH*				m_pVertices = nullptr;
	POLYGON_INDICES32*		m_pFaces = nullptr;
	MESHES					m_Meshes;
	MATERIALS				m_Materials;
	_uint					m_iMeshCount = 0;
	_uint					m_iMaterialCount = 0;
	vector<MESHES>			m_SortedMeshes;
	/* For.Instance */
	_uint					m_iInstanceCount = 0;
	_float4x4*				m_arrWorldMatrices = nullptr;
	_uint*					m_arrViewportDrawInfo = nullptr;
	_uint					m_iRealDrawCount = 0;
	VTXMATRIX2*				m_arrRealTimeMatrices = nullptr;
	_float					m_fCullingRadius = 0.f;
	/* For.PhyX */
	PxRigidStatic**			m_ppActors = nullptr;
	vector<PX_TRIMESH>		m_PxTriMeshes;
	/* For.MaterialSet */
	_uint					m_iMaterialSetCount = 0;
	/* For.Check Bind Materials */
	_uint					m_IsBindMaterials[AI_TEXTURE_TYPE_MAX];
	/* For.MultiRenderGroup */
	_bool					m_bMultiRenderGroup = false;
	/* For.Check_CreateActor */
	_bool					m_bCreateActor = false;
private:
	HRESULT	Sort_MeshesByMaterial();
	HRESULT	Apply_PivotMatrix(_fmatrix PivotMatrix);
	HRESULT	Store_TriMeshes();
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
	/* For.Instance */
	ID3D11Buffer*				m_pVBInstance = nullptr;
	VTXMATRIX2*					m_pInstanceVertices = nullptr;
	_uint						m_iMaxInstanceCount = 0;
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
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount = 1, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bNeedCenterBone = false, const char* pCenterBoneName = "", _bool bCreateActor = true);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END