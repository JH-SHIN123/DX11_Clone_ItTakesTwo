#ifndef Model_h__
#define Model_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public: /* Getter */
	const _uint		Get_MeshCount			() const;
	_fmatrix		Get_BoneMatrix			(const char* pBoneName) const;
	_fvector		Get_AdjustedPos			(_fmatrix WorldMatrix) const;
	const _uint		Get_CurrentAnimIndex	() const;
	const _float	Get_ProgressAnim		() const;
	const _double	Get_PreAnimTime			() const;
	const _bool		Check_ReserveNextAnim	() const;

public: /* Setter */
	void	Initialize_CurrentKeyFrames	();
	HRESULT	Set_Animation				(_uint iAnimIndex, class CTransform* pTransform);
	HRESULT	Set_NextAnimIndex			(_uint iAnimIndex);
	void	Set_StepCounter				(_uint* pStepCounter);
	HRESULT	Set_DefaultVariables_Perspective(_fmatrix WorldMatrix);

public:
	virtual HRESULT	NativeConstruct_Prototype	(const char* pMeshFilePath, const char* pMeshFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix, _uint iAdjBoneCount, const char* pBoneName1, const char* pBoneName2);
	virtual HRESULT	NativeConstruct				(void* pArg) override;
	HRESULT			Bring_Containers			(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<class CHierarchyNode*>& Nodes, vector<class CAnim*>& Anims);
	/* Client */
	HRESULT	Set_Variable						(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView				(const char* pConstantName, ID3D11ShaderResourceView* pResourceView);
	HRESULT	Update_NodeMatrices					(_double dTimeDelta, class CTransform* pTransform);
	HRESULT	Bind_VIBuffer						();
	HRESULT	Render_Model						(_uint iMeshIndex, _uint iPassIndex);
	HRESULT	Update_State_DuringAnim				(class CTransform* pTransform);

private:
	typedef vector<class CMesh*>			MESHES;
	typedef	vector<class CHierarchyNode*>	NODES;
	typedef vector<class CAnim*>			ANIMS;
	typedef vector<_uint>					CURRENT_KEYFRAMES;
	typedef vector<_float4x4>				TRANSFORMATIONS;
private:
	class CModel_Loader*		m_pModel_Loader				= nullptr;
	VTXMESH*					m_pVertices					= nullptr;
	POLYGON_INDICES32*			m_pFaces					= nullptr;
	MESHES						m_Meshes;
	NODES						m_Nodes;
	ANIMS						m_Anims;
	_uint						m_iMeshCount				= 0;
	_uint						m_iNodeCount				= 0;
	_uint						m_iAnimCount				= 0;
	_float4x4					m_PivotMatrix;
	_float4x4					m_CombinedPivotMatrix;
	/* For.Animating */
	_uint						m_iCurrentAnimIndex			= 0;
	_double						m_dCurrectTime				= 0.0;
	CURRENT_KEYFRAMES			m_CurrentKeyFrames;			// 앞 채널부터 현재 프레임 번호 보관.
	TRANSFORMATIONS				m_AnimTransformations;		// 앞 노드부터 연결된 채널의 TransformationMatrix 보관. 채널 순서X
	TRANSFORMATIONS				m_CombinedTransformations;
	_uint						m_iNextAnimIndex			= 0;
	_bool						m_bReserveNextAnim			= false;
	_float						m_fProgressAnim				= 0.f;
	/* For.Adjust_Center */
	_uint						m_iAdjBoneCount				= 0;
	class CHierarchyNode*		m_pAdjBoneNode1				= nullptr;
	class CHierarchyNode*		m_pAdjBoneNode2				= nullptr;
	/* For.Update_Distance_From_StartPoint */
	_float						m_fAnimDistX				= 0.f;
	_float						m_fAnimDistZ				= 0.f;
	/* For.StepCounter */
	_uint*						m_pStepCounter				= nullptr;
	/* For.Lerp */
	class CAnim*				m_pPreAnim					= nullptr;
	_double						m_dPreStopTime				= 0.0;
	_double						m_dPreAnimDuration			= 0.0;
	_double						m_dPreAnimTime				= 0.0;
	CURRENT_KEYFRAMES			m_PreKeyFrames;
	/* For.Vertices */
	ID3D11Buffer*				m_pVB						= nullptr;
	_uint						m_iVertexCount				= 0;
	_uint						m_iVertexStride				= 0;
	_uint						m_iVertexBufferCount		= 0;
	/* For.Indices */
	ID3D11Buffer*				m_pIB						= nullptr;
	_uint						m_iFaceCount				= 0;
	_uint						m_iFaceStride				= 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	/* For.Shader */
	ID3DX11Effect*				m_pEffect					= nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts;
private:
	HRESULT		Create_Buffer					(ID3D11Buffer** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void* pSysMem, _uint iSysMemPitch = 0, _uint iSysMemSlicePitch = 0);
	HRESULT		Create_VIBuffer					(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT		Set_CenterBone					(_uint iAdjBoneCount, const char* pBoneName1 = "", const char* pBoneName2 = "");
	HRESULT		Apply_PivotMatrix				(_fmatrix PivotMatrix);
	HRESULT		SetUp_InputLayouts				(D3D11_INPUT_ELEMENT_DESC* pInputElementDesc, _uint iElementCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	void		Update_CombinedTransformations	();
	void		Update_CombinedPivotMatrix		();
	void		Update_DistanceFromStartPoint	();

public:
	static CModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pMeshFilePath, const char* pMeshFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix = XMMatrixIdentity(), _uint iAdjBoneCount = 0, const char* pBoneName1 = "", const char* pBoneName2 = "");
	virtual CComponent*	Clone_Component	(void* pArg) override;
	virtual void		Free			() override;
};

END

#endif // Model_h__