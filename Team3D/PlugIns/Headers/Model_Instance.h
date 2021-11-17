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
		PxMaterial*		pMaterial;
		const char*		pActorName;

		tagArgumentDesc() {}
		tagArgumentDesc(_float4x4* _pWorldMatrices, _uint _iInstanceCount, _float _fCullingRadius, PxMaterial* _pMaterial, const char* pName)
			: pWorldMatrices(_pWorldMatrices), iInstanceCount(_iInstanceCount), fCullingRadius(_fCullingRadius), pMaterial(_pMaterial), pActorName(pName) {}
	}ARG_DESC;
	typedef struct tagPxTriMesh
	{
		PxVec3*				pVertices;
		POLYGON_INDICES32*	pFaces;
		PxTriangleMesh*		pTriMesh;
	}PX_TRIMESH;

public: /* Setter */
	/* For.Shader */
	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pShaderResourceView);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType, _uint iTextureIndex = 0);
	HRESULT	Set_DefaultVariables_Perspective();
	HRESULT	Set_DefaultVariables_Shadow();
	HRESULT	Set_DefaultVariables_ShadowDepth();

public:
	virtual HRESULT	NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char* pCenterBoneName);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	/* For.ModelLoader */
	HRESULT	Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<MATERIAL*>& Materials);
	/* For.Client */
	HRESULT Update_Model(_fmatrix TransformMatrix);
	HRESULT	Render_Model(_uint iPassIndex, _uint iMaterialSetNum = 0, _bool bShadowWrite = false);

public:
	_uint	Frustum_Culling(); /* @Return : RenderCount */
	HRESULT Bind_GBuffers(_uint iRenderCount);
	HRESULT	Render_ModelByPass(_uint iRenderCount, _uint iMaterialIndex, _uint iPassIndex, _bool bShadowWrite = false); /* 텍스쳐 외부에서 따로 연결해줘야함. */

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
	_float4x4*				m_pWorldMatrices = nullptr;
	vector<_float4x4>		m_RealTimeMatrices;
	_float					m_fCullingRadius = 0.f;
	/* For.PhyX */
	PxRigidStatic**			m_ppActors = nullptr;
	vector<PX_TRIMESH>		m_PxTriMeshes;
	char					m_szActorName[MAX_PATH];
	/* For.MaterialSet */
	_uint					m_iMaterialSetCount = 0;
	/*For. Check Bind Materials */
	_bool					m_IsBindMaterials[AI_TEXTURE_TYPE_MAX];
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
	VTXMATRIX*					m_pInstanceVertices = nullptr;
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
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pModelFilePath, const _tchar* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iMaterialSetCount = 1, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bNeedCenterBone = false, const char* pCenterBoneName = "");
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END