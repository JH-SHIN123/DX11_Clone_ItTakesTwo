#include "..\Public\Model_Instance.h"
#include "Shadow_Manager.h"
#include "RenderTarget_Manager.h"
#include "Graphic_Device.h"
#include "Model_Loader.h"
#include "Mesh.h"
#include "Textures.h"
#include "Pipeline.h"
#include "Frustum.h"
#include "PhysX.h"

CModel_Instance::CModel_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pModel_Loader(CModel_Loader::GetInstance())
{
	Safe_AddRef(m_pModel_Loader);
}

CModel_Instance::CModel_Instance(const CModel_Instance & rhs)
	: CComponent					(rhs)
	, m_Meshes						(rhs.m_Meshes)
	, m_Materials					(rhs.m_Materials)
	, m_iMeshCount					(rhs.m_iMeshCount)
	, m_iMaterialCount				(rhs.m_iMaterialCount)
	, m_SortedMeshes				(rhs.m_SortedMeshes)
	, m_iMaterialSetCount			(rhs.m_iMaterialSetCount)
	, m_PxTriMeshes					(rhs.m_PxTriMeshes)
	, m_pVB							(rhs.m_pVB)
	, m_iVertexCount				(rhs.m_iVertexCount)
	, m_iVertexStride				(rhs.m_iVertexStride)
	, m_iVertexBufferCount			(rhs.m_iVertexBufferCount)
	, m_pIB							(rhs.m_pIB)
	, m_iFaceCount					(rhs.m_iFaceCount)
	, m_iFaceStride					(rhs.m_iFaceStride)
	, m_eIndexFormat				(rhs.m_eIndexFormat)
	, m_eTopology					(rhs.m_eTopology)
	, m_pEffect						(rhs.m_pEffect)
	, m_InputLayouts				(rhs.m_InputLayouts)
	, m_pFaces						(rhs.m_pFaces)
	, m_pVBInstance					(rhs.m_pVBInstance)
	, m_pInstanceVertices			(rhs.m_pInstanceVertices)
	, m_iMaxInstanceCount			(rhs.m_iMaxInstanceCount)
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
	{
		for (auto& pTexture : pMaterial->pMaterialTexture)
			Safe_AddRef(pTexture);
	}

	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pVBInstance);
	Safe_AddRef(m_pEffect);

	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_AddRef(InputLayout.pLayout);
		Safe_AddRef(InputLayout.pPass);
	}
}

HRESULT CModel_Instance::Set_Variable(const char * pConstantName, void * pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CModel_Instance::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView * pShaderResourceView)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetResource(pShaderResourceView);
}

HRESULT CModel_Instance::Set_ShaderResourceView(const char * pConstantName, _uint iMaterialIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	CTextures* pTextures = m_Materials[iMaterialIndex]->pMaterialTexture[eTextureType];

	if (nullptr == pTextures)
		return S_OK;

	ID3D11ShaderResourceView* pShaderResourceView = pTextures->Get_ShaderResourceView(iTextureIndex);
	NULL_CHECK_RETURN(pTextures, E_FAIL);

	return pVariable->SetResource(pShaderResourceView);
}

HRESULT CModel_Instance::Set_DefaultVariables_Perspective()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	NULL_CHECK_RETURN(pPipeline, E_FAIL);

	Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();

	Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));
	Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));

	return S_OK;
}

HRESULT CModel_Instance::Set_DefaultVariables_Shadow()
{
	CGraphic_Device* pGraphicDevice = CGraphic_Device::GetInstance();
	_float4	vViewportUVInfo;
	vViewportUVInfo = pGraphicDevice->Get_ViewportRadioInfo(CGraphic_Device::VP_MAIN);
	Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4));
	vViewportUVInfo = pGraphicDevice->Get_ViewportRadioInfo(CGraphic_Device::VP_SUB);
	Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4));

	CShadow_Manager* pShadowManager = CShadow_Manager::GetInstance();
	_matrix ShadowTransform[MAX_CASCADES]; /* Shadow View * Shadow Proj * NDC */
	pShadowManager->Get_CascadeShadowTransformsTranspose(CShadow_Manager::SHADOW_MAIN, ShadowTransform);
	Set_Variable("g_ShadowTransforms_Main", ShadowTransform, sizeof(_matrix) * MAX_CASCADES);
	pShadowManager->Get_CascadeShadowTransformsTranspose(CShadow_Manager::SHADOW_SUB, ShadowTransform);
	Set_Variable("g_ShadowTransforms_Sub", ShadowTransform, sizeof(_matrix) * MAX_CASCADES);
	Set_Variable("g_CascadeEnds", (void*)pShadowManager->Get_CascadedEnds(), sizeof(_float) * (MAX_CASCADES + 1));

	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();
	Set_ShaderResourceView("g_CascadedShadowDepthTexture", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_CascadedShadow_Depth")));

	return S_OK;
}

HRESULT CModel_Instance::Set_DefaultVariables_ShadowDepth()
{
	_matrix ShadowViewProj[MAX_CASCADES];

	CShadow_Manager* pShadowManager = CShadow_Manager::GetInstance();

	pShadowManager->Get_CascadeShadowViewProjTranspose(CShadow_Manager::SHADOW_MAIN, ShadowViewProj);
	Set_Variable("g_ShadowTransforms_Main", ShadowViewProj, sizeof(_matrix) * MAX_CASCADES);

	pShadowManager->Get_CascadeShadowViewProjTranspose(CShadow_Manager::SHADOW_SUB, ShadowViewProj);
	Set_Variable("g_ShadowTransforms_Sub", ShadowViewProj, sizeof(_matrix) * MAX_CASCADES);

	return S_OK;
}


HRESULT CModel_Instance::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pModelFilePath, const _tchar * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char * pCenterBoneName)
{
	NULL_CHECK_RETURN(m_pModel_Loader, E_FAIL);

	CComponent::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iMaxInstanceCount;
	m_iMaterialSetCount = iMaterialSetCount;

	FAILED_CHECK_RETURN(m_pModel_Loader->Load_ModelFromFile(m_pDevice, m_pDeviceContext, CModel_Loader::TYPE_INSTANCE, this, pModelFilePath, pModelFileName, iMaterialSetCount), E_FAIL);
	FAILED_CHECK_RETURN(Apply_PivotMatrix(PivotMatrix), E_FAIL);
	FAILED_CHECK_RETURN(Store_TriMeshes(), E_FAIL);
	FAILED_CHECK_RETURN(Create_VIBuffer(pShaderFilePath, pTechniqueName), E_FAIL);
	FAILED_CHECK_RETURN(Sort_MeshesByMaterial(), E_FAIL);

	return S_OK;
}

HRESULT CModel_Instance::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	CPhysX* pPhysX = CPhysX::GetInstance();
	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pWorldMatrices = ArgDesc.pWorldMatrices;
	m_iInstanceCount = ArgDesc.iInstanceCount;
	NULL_CHECK_RETURN(m_iInstanceCount <= m_iMaxInstanceCount, E_FAIL);

	m_RealTimeMatrices.resize(m_iInstanceCount, MH_XMFloat4x4Identity());
	m_fCullingRadius = ArgDesc.fCullingRadius;

	strcpy(m_szActorName, ArgDesc.pActorName);
	m_ppActors = new PxRigidStatic*[m_iInstanceCount * m_iMeshCount];

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		_vector vScale, vRotQuat, vPosition;
		XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, XMLoadFloat4x4(&m_pWorldMatrices[iIndex]));

		for (_uint iMeshIndex = 0; iMeshIndex < m_iMeshCount; ++iMeshIndex)
		{
			_uint iActorIndex = iIndex + iMeshIndex * m_iInstanceCount;

			PxTriangleMeshGeometry geom(m_PxTriMeshes[iMeshIndex].pTriMesh, PxMeshScale(MH_PxVec3(vScale)));

			m_ppActors[iActorIndex] = pPhysX->Create_StaticActor(MH_PxTransform(vRotQuat, vPosition), geom, ArgDesc.pMaterial, m_szActorName);
			NULL_CHECK_RETURN(m_ppActors[iActorIndex], E_FAIL);

			PxShape* Shape;
			m_ppActors[iActorIndex]->getShapes(&Shape, 1);
			Shape->setContactOffset(0.02f);
			Shape->setRestOffset(-0.5f);
			//
			Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

			pPhysX->Add_ActorToScene(m_ppActors[iActorIndex]);
			Setup_PxFiltering(m_ppActors[iActorIndex], FilterGroup::eSTATIC, 0);
		}
	}

	return S_OK;
}

HRESULT CModel_Instance::Bring_Containers(VTXMESH * pVertices, _uint iVertexCount, POLYGON_INDICES32 * pFaces, _uint iFaceCount, vector<class CMesh*>& Meshes, vector<MATERIAL*>& Materials)
{
	m_iVertexCount = iVertexCount;
	m_iFaceCount = iFaceCount;
	m_iMeshCount = (_uint)Meshes.size();
	m_iMaterialCount = (_uint)Materials.size();

	/* Bring_Containers */
	m_pVertices = pVertices;
	m_pFaces = pFaces;
	m_Meshes.swap(Meshes);
	m_Materials.swap(Materials);

	return S_OK;
}

HRESULT CModel_Instance::Update_Model(_fmatrix TransformMatrix)
{
	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		XMStoreFloat4x4(&m_pWorldMatrices[iIndex], XMLoadFloat4x4(&m_pWorldMatrices[iIndex]) * TransformMatrix);

		_vector vScale, vRotQuat, vPosition;
		XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, XMLoadFloat4x4(&m_pWorldMatrices[iIndex]));

		for (_uint iMeshIndex = 0; iMeshIndex < m_iMeshCount; ++iMeshIndex)
		{
			_uint iActorIndex = iIndex + iMeshIndex * m_iInstanceCount;
			m_ppActors[iActorIndex]->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
		}
	}

	return S_OK;
}

HRESULT CModel_Instance::Render_Model(_uint iPassIndex, _uint iMaterialSetNum, _bool bShadowWrite)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);

	/* For.Culling */
	_uint iRenderCount = 0;

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		if (CFrustum::GetInstance()->IsIn_WorldSpace(MH_GetXMPosition(m_pWorldMatrices[iIndex]), m_fCullingRadius))
			m_RealTimeMatrices[iRenderCount++] = m_pWorldMatrices[iIndex];
	}

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, &m_RealTimeMatrices[0], sizeof(VTXMATRIX) * iRenderCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.Render */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXMESH), sizeof(VTXMATRIX) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	for (_uint iMaterialIndex = 0; iMaterialIndex < m_iMaterialCount; ++iMaterialIndex)
	{
		/* Write Shadow - ÅØ½ºÃÄ ¿¬°á¾ÈÇØÁàµµ µÊ */
		if (false == bShadowWrite) {
			Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, iMaterialSetNum);
			Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, iMaterialSetNum);
			Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, iMaterialSetNum);
			Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, iMaterialSetNum);
			
			FAILED_CHECK_RETURN(Is_BindMaterials(iMaterialIndex), E_FAIL);
		}

		FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

		for (auto& pMesh : m_SortedMeshes[iMaterialIndex])
			m_pDeviceContext->DrawIndexedInstanced(3 * pMesh->Get_FaceCount(), iRenderCount, 3 * pMesh->Get_StratFaceIndex(), pMesh->Get_StartVertexIndex(), 0);
	}

	return S_OK;
}

_uint CModel_Instance::Frustum_Culling()
{
	/* For.Culling */
	_uint iRenderCount = 0;

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		if (CFrustum::GetInstance()->IsIn_WorldSpace(MH_GetXMPosition(m_pWorldMatrices[iIndex]), m_fCullingRadius))
			m_RealTimeMatrices[iRenderCount++] = m_pWorldMatrices[iIndex];
	}

	return iRenderCount;
}

HRESULT CModel_Instance::Bind_GBuffers(_uint iRenderCount)
{
	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, &m_RealTimeMatrices[0], sizeof(VTXMATRIX) * iRenderCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.Render */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXMESH), sizeof(VTXMATRIX) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CModel_Instance::Render_ModelByPass(_uint iRenderCount, _uint iMaterialIndex, _uint iPassIndex, _bool bShadowWrite)
{
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	if (false == bShadowWrite)
		FAILED_CHECK_RETURN(Is_BindMaterials(iMaterialIndex), E_FAIL);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	for (auto& pMesh : m_SortedMeshes[iMaterialIndex])
		m_pDeviceContext->DrawIndexedInstanced(3 * pMesh->Get_FaceCount(), iRenderCount, 3 * pMesh->Get_StratFaceIndex(), pMesh->Get_StartVertexIndex(), 0);

	return S_OK;
}

HRESULT CModel_Instance::Sort_MeshesByMaterial()
{
	m_SortedMeshes.resize(m_iMaterialCount);

	for (auto& pMesh : m_Meshes)
	{
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();
		NULL_CHECK_RETURN(iMaterialIndex < m_iMaterialCount, E_FAIL);

		m_SortedMeshes[iMaterialIndex].emplace_back(pMesh);
		Safe_AddRef(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Instance::Apply_PivotMatrix(_fmatrix PivotMatrix)
{
	for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
	{
		_vector	vAdjustedPosition = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[iIndex].vPosition), PivotMatrix);
		XMStoreFloat3(&m_pVertices[iIndex].vPosition, vAdjustedPosition);
		_vector	vAdjustedNormal = XMVector3TransformNormal(XMLoadFloat3(&m_pVertices[iIndex].vNormal), PivotMatrix);
		XMStoreFloat3(&m_pVertices[iIndex].vNormal, vAdjustedNormal);
	}

	return S_OK;
}

HRESULT CModel_Instance::Store_TriMeshes()
{
	CPhysX* pPhysX = CPhysX::GetInstance();

	m_PxTriMeshes.reserve(m_iMeshCount);

	for (_uint iIndex = 0; iIndex < m_iMeshCount; ++iIndex)
	{
		PX_TRIMESH TriMesh;

		_uint iMeshStartVertexIndex = m_Meshes[iIndex]->Get_StartVertexIndex();
		_uint iMeshStartFaceIndex = m_Meshes[iIndex]->Get_StratFaceIndex();
		_uint iMeshVertexCount = (iIndex + 1 == m_iMeshCount) ? m_iVertexCount - iMeshStartVertexIndex : m_Meshes[iIndex + 1]->Get_StartVertexIndex() - iMeshStartVertexIndex;
		_uint iMeshFaceCount = m_Meshes[iIndex]->Get_FaceCount();

		TriMesh.pVertices = new PxVec3[iMeshVertexCount];
		TriMesh.pFaces = new POLYGON_INDICES32[iMeshVertexCount];

		for (_uint iVertexIndex = 0; iVertexIndex < iMeshVertexCount; ++iVertexIndex)
			memcpy(&TriMesh.pVertices[iVertexIndex], &m_pVertices[iMeshStartVertexIndex + iVertexIndex], sizeof(PxVec3));
		for (_uint iFaceIndex = 0; iFaceIndex < iMeshFaceCount; ++iFaceIndex)
			memcpy(&TriMesh.pFaces[iFaceIndex], &m_pFaces[iMeshStartFaceIndex + iFaceIndex], sizeof(POLYGON_INDICES32));
		TriMesh.pTriMesh = pPhysX->Create_Mesh(MESHACTOR_DESC(iMeshVertexCount, TriMesh.pVertices, iMeshFaceCount, TriMesh.pFaces));

		m_PxTriMeshes.emplace_back(TriMesh);
	}

	return S_OK;
}

HRESULT CModel_Instance::Is_BindMaterials(_uint iMaterialIndex)
{
	ZeroMemory(m_IsBindMaterials, sizeof(m_IsBindMaterials));

	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		if (nullptr == m_Materials[iMaterialIndex]->pMaterialTexture[i])
			m_IsBindMaterials[i] = false;
		else
			m_IsBindMaterials[i] = true;
	}
	Set_Variable("g_IsMaterials", m_IsBindMaterials, sizeof(m_IsBindMaterials));

	return S_OK;
}

HRESULT CModel_Instance::Create_Buffer(ID3D11Buffer ** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void * pSysMem, _uint iSysMemPitch, _uint iSysMemSlicePitch)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = iByteWidth;
	Desc.Usage = Usage;
	Desc.BindFlags = iBindFlags;
	Desc.CPUAccessFlags = iCPUAccessFlags;
	Desc.MiscFlags = iMiscFlags;
	Desc.StructureByteStride = iStructureByteStride;

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem = pSysMem;
	SubResourceData.SysMemPitch = iSysMemPitch;
	SubResourceData.SysMemSlicePitch = iSysMemSlicePitch;

	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&Desc, &SubResourceData, ppBuffer), E_FAIL);

	return S_OK;
}

HRESULT CModel_Instance::Create_VIBuffer(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	/* For.VertexBuffer */
	m_iVertexBufferCount = 2;
	m_iVertexStride = sizeof(VTXMESH);
	Create_Buffer(&m_pVB, m_iVertexStride * m_iVertexCount, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, m_pVertices);
	Safe_Delete_Array(m_pVertices);

	/* For.IndexBuffer */
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iFaceStride = sizeof(POLYGON_INDICES32);
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Create_Buffer(&m_pIB, m_iFaceCount * m_iFaceStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0, 0, m_pFaces);
	Safe_Delete_Array(m_pFaces);

	/* For.InstanceBuffer */
	m_pInstanceVertices = new VTXMATRIX[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[iIndex].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}
	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXMATRIX), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX), m_pInstanceVertices);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC	ElementDesc[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX",		0, DXGI_FORMAT_R32G32B32A32_UINT,	0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 60,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD",			0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",			1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 16,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",			2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",			3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,	D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 10, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CModel_Instance::SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC * pInputElementDesc, _uint iElementCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pCompiledShaderCode = nullptr;
	ID3DBlob* pCompileErrorMsg = nullptr;

	FAILED_CHECK_RETURN(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCompiledShaderCode, &pCompileErrorMsg), E_FAIL);
	FAILED_CHECK_RETURN(D3DX11CreateEffectFromMemory(pCompiledShaderCode->GetBufferPointer(), pCompiledShaderCode->GetBufferSize(), 0, m_pDevice, &m_pEffect), E_FAIL);

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByName(pTechniqueName);
	NULL_CHECK_RETURN(pTechnique, E_FAIL);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	FAILED_CHECK_RETURN(pTechnique->GetDesc(&TechniqueDesc), E_FAIL);

	m_InputLayouts.reserve(TechniqueDesc.Passes);

	for (_uint iPassIndex = 0; iPassIndex < TechniqueDesc.Passes; ++iPassIndex)
	{
		INPUT_LAYOUT_DESC	InputLayoutDesc;
		D3DX11_PASS_DESC	PassDesc;

		InputLayoutDesc.pPass = pTechnique->GetPassByIndex(iPassIndex);
		FAILED_CHECK_RETURN(InputLayoutDesc.pPass->GetDesc(&PassDesc), E_FAIL);
		FAILED_CHECK_RETURN(m_pDevice->CreateInputLayout(pInputElementDesc, iElementCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &InputLayoutDesc.pLayout), E_FAIL);

		m_InputLayouts.emplace_back(InputLayoutDesc);
	}

	Safe_Release(pTechnique);
	Safe_Release(pCompiledShaderCode);
	Safe_Release(pCompileErrorMsg);

	return S_OK;
}

CModel_Instance * CModel_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pModelFilePath, const _tchar * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _uint iMaterialSetCount, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char * pCenterBoneName)
{
	CModel_Instance* pInstance = new CModel_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pModelFilePath, pModelFileName, pShaderFilePath, pTechniqueName, iMaterialSetCount ,PivotMatrix, bNeedCenterBone, pCenterBoneName)))
	{
		MSG_BOX("Failed to Create Instance - CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel_Instance::Clone_Component(void * pArg)
{
	CModel_Instance* pInstance = new CModel_Instance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instance::Free()
{
	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_Release(InputLayout.pLayout);
		Safe_Release(InputLayout.pPass);
	}

	Safe_Release(m_pEffect);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	Safe_Release(m_pVBInstance);

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pMaterial : m_Materials)
	{
		for (auto& pTexture : pMaterial->pMaterialTexture)
			Safe_Release(pTexture);
		if (false == m_isClone)
			Safe_Delete(pMaterial);
	}
	m_Materials.clear();

	for (auto& Meshes : m_SortedMeshes)
	{
		for (auto& pMesh : Meshes)
			Safe_Release(pMesh);
	}
	m_SortedMeshes.clear();

	if (false == m_isClone)
	{
		Safe_Release(m_pModel_Loader);
		Safe_Delete_Array(m_pInstanceVertices);

		for (auto& TriMesh : m_PxTriMeshes)
		{
			Safe_Delete_Array(TriMesh.pVertices);
			Safe_Delete_Array(TriMesh.pFaces);
			TriMesh.pTriMesh->release();
		}
		m_PxTriMeshes.clear();
	}

	Safe_Delete_Array(m_pWorldMatrices);
	m_RealTimeMatrices.clear();

	if (true == m_isClone)
	{
		_uint iActorCount = m_iInstanceCount * m_iMeshCount;

		for (_uint iIndex = 0; iIndex < iActorCount; ++iIndex)
			CPhysX::GetInstance()->Remove_Actor(&m_ppActors[iIndex]);
		Safe_Delete_Array(m_ppActors);
	}

	CComponent::Free();
}