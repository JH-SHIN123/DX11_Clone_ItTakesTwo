#include "..\public\Model.h"
#include "Model_Loader.h"
#include "Mesh.h"
#include "Textures.h"
#include "HierarchyNode.h"
#include "Anim.h"
#include "Transform.h"
#include "Pipeline.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent		(pDevice, pDeviceContext)
	, m_pModel_Loader	(CModel_Loader::GetInstance())
{
	Safe_AddRef(m_pModel_Loader);
}

CModel::CModel(const CModel & rhs)
	: CComponent					(rhs)
	, m_Meshes						(rhs.m_Meshes)
	, m_Materials					(rhs.m_Materials)
	, m_Nodes						(rhs.m_Nodes)
	, m_Anims						(rhs.m_Anims)
	, m_iMeshCount					(rhs.m_iMeshCount)
	, m_iMaterialCount				(rhs.m_iMaterialCount)
	, m_iNodeCount					(rhs.m_iNodeCount)
	, m_iAnimCount					(rhs.m_iAnimCount)
	, m_SortedMeshes				(rhs.m_SortedMeshes)
	, m_PivotMatrix					(rhs.m_PivotMatrix)
	, m_CombinedPivotMatrix			(rhs.m_CombinedPivotMatrix)
	, m_BaseTransformations			(rhs.m_BaseTransformations)
	, m_AnimTransformations			(rhs.m_AnimTransformations)
	, m_CombinedTransformations		(rhs.m_CombinedTransformations)
	, m_PreAnimKeyFrames			(rhs.m_PreAnimKeyFrames)
	, m_IsAnimFinished				(rhs.m_IsAnimFinished)
	, m_bNeedCenterBone				(rhs.m_bNeedCenterBone)
	, m_pCenterBoneNode				(rhs.m_pCenterBoneNode)
	, m_vAnimDistFromCenter			(rhs.m_vAnimDistFromCenter)
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
	, m_pVectorPositions			(rhs.m_pVectorPositions)
	, m_pFaces						(rhs.m_pFaces)
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
	{
		for (auto& pTexture : pMaterial->pMaterialTexture)
			Safe_AddRef(pTexture);
	}

	for (auto& pNode : m_Nodes)
		Safe_AddRef(pNode);

	for (auto& pAnim : m_Anims)
		Safe_AddRef(pAnim);

	Safe_AddRef(m_pCenterBoneNode);

	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pEffect);

	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_AddRef(InputLayout.pLayout);
		Safe_AddRef(InputLayout.pPass);
	}
}

_fmatrix CModel::Get_BoneMatrix(const char * pBoneName) const
{
	for (auto& pNode : m_Nodes)
	{
		if (!strcmp(pNode->Get_Name(), pBoneName))
		{
			_int iIndex = pNode->Get_NodeIndex();

			if (iIndex == -1)
				return XMMatrixIdentity();

			return XMLoadFloat4x4(&m_CombinedTransformations[iIndex]);
		}
	}

	return XMMatrixIdentity();
}

HRESULT CModel::Set_Animation(_uint iAnimIndex, CTransform * pTransform)
{
	if (iAnimIndex == m_iCurAnimIndex)
		return S_OK;

	NULL_CHECK_RETURN(iAnimIndex < m_iAnimCount, E_FAIL);

	/* For.Lerp */
	m_fLerpRatio = 1.f;
	m_PreAnimKeyFrames.assign(m_iNodeCount, nullptr);
	m_Anims[m_iCurAnimIndex]->Get_PreAnimKeyFrames(m_iCurAnimFrame, m_PreAnimKeyFrames);

	/* For.FinishCheck */
	m_IsAnimFinished.assign(m_iAnimCount, false);
	m_IsAnimFinished[m_iCurAnimIndex] = true;

	/* For.Update */
	m_dCurrentTime = 0.0;

	m_iCurAnimIndex = iAnimIndex;
	m_iCurAnimFrame = 0;
	m_AnimTransformations.assign(m_BaseTransformations.begin(), m_BaseTransformations.end());

	return S_OK;
}

HRESULT CModel::Set_NextAnimIndex(_uint iAnimIndex)
{
	NULL_CHECK_RETURN(iAnimIndex < m_iAnimCount, E_FAIL);

	m_iNextAnimIndex = iAnimIndex;

	return S_OK;
}

HRESULT CModel::Set_Variable(const char * pConstantName, void * pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CModel::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView* pShaderResourceView)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetResource(pShaderResourceView);
}

HRESULT CModel::Set_ShaderResourceView(const char * pConstantName, _uint iMaterialIndex, aiTextureType eTextureType, _uint iTextureIndex)
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

HRESULT CModel::Set_DefaultVariables_Perspective(_fmatrix WorldMatrix)
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	NULL_CHECK_RETURN(pPipeline, E_FAIL);

	Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
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

HRESULT CModel::NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char * pCenterBoneName)
{
	NULL_CHECK_RETURN(m_pModel_Loader, E_FAIL);

	CComponent::NativeConstruct_Prototype();

	m_bNeedCenterBone = bNeedCenterBone;
	m_vAnimDistFromCenter = _float4(0.f, 0.f, 0.f, 1.f);

	FAILED_CHECK_RETURN(m_pModel_Loader->Load_ModelFromFile(m_pDevice, m_pDeviceContext, this, pMeshFilePath, pMeshFileName), E_FAIL);
	FAILED_CHECK_RETURN(Apply_PivotMatrix(PivotMatrix), E_FAIL);
	FAILED_CHECK_RETURN(Create_VIBuffer(pShaderFilePath, pTechniqueName), E_FAIL);
	FAILED_CHECK_RETURN(Sort_MeshesByMaterial(), E_FAIL);

	if (true == m_bNeedCenterBone)
		FAILED_CHECK_RETURN(Set_CenterBone(pCenterBoneName), E_FAIL);

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CModel::Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*> & Meshes, vector<MATERIAL*> & Materials, vector<class CHierarchyNode*> & Nodes, vector<_float4x4> & Transformations, vector<class CAnim*> & Anims)
{
	m_iVertexCount		= iVertexCount;
	m_iFaceCount		= iFaceCount;
	m_iMeshCount		= (_uint)Meshes.size();
	m_iMaterialCount	= (_uint)Materials.size();
	m_iNodeCount		= (_uint)Nodes.size();
	m_iAnimCount		= (_uint)Anims.size();

	/* Bring_Containers */
	m_pVertices = pVertices;
	m_pFaces	= pFaces;
	m_Meshes.swap(Meshes);
	m_Materials.swap(Materials);
	m_Nodes.swap(Nodes);
	m_BaseTransformations.swap(Transformations);
	m_Anims.swap(Anims);

	m_AnimTransformations.resize(m_iNodeCount);
	m_CombinedTransformations.resize(m_iNodeCount);
	m_PreAnimKeyFrames.resize(m_iNodeCount, nullptr);
	m_IsAnimFinished.resize(m_iAnimCount, false);

	m_AnimTransformations.assign(m_BaseTransformations.begin(), m_BaseTransformations.end());

	return S_OK;
}

HRESULT CModel::Update_Animation(_double dTimeDelta, CTransform * pTransform)
{
	NULL_CHECK_RETURN(m_iAnimCount, E_FAIL);

	_double dDuration		= m_Anims[m_iCurAnimIndex]->Get_Duration();
	_double dTicksPerSecond	= m_Anims[m_iCurAnimIndex]->Get_TicksPerSecond();

	m_dCurrentTime += fmod(dTicksPerSecond * dTimeDelta, dDuration);

	if (m_dCurrentTime >= dDuration)
	{
		/* For.Lerp */
		m_fLerpRatio = 1.f;
		m_PreAnimKeyFrames.assign(m_iNodeCount, nullptr);
		m_Anims[m_iCurAnimIndex]->Get_PreAnimKeyFrames(m_iCurAnimFrame, m_PreAnimKeyFrames);

		/* For.FinishCheck */
		m_IsAnimFinished.assign(m_iAnimCount, false);
		m_IsAnimFinished[m_iCurAnimIndex] = true;

		/* For.Update */
		m_dCurrentTime -= dDuration;

		m_iCurAnimIndex	= m_iNextAnimIndex;
		m_iCurAnimFrame = 0;
		m_AnimTransformations.assign(m_BaseTransformations.begin(), m_BaseTransformations.end());
	}

	/* Update_AnimTransformations */
	Update_AnimTransformations(dTimeDelta);
	/* Update_CombinedTransformations */
	Update_CombinedTransformations();

	/* Update Progress */
	m_fProgressAnim = _float(m_dCurrentTime / dDuration);

	return S_OK;
}

HRESULT CModel::Render_Model(_uint iPassIndex)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);

	_uint iOffSet = 0;

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, &m_pVB, &m_iVertexStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	if (0 < m_iAnimCount)
	{
		_matrix	BoneMatrices[256];

		for (_uint iMaterialIndex = 0; iMaterialIndex < m_iMaterialCount; ++iMaterialIndex)
		{

			Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE);
			
			FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);
			
			for (auto& pMesh : m_SortedMeshes[iMaterialIndex])
			{
				ZeroMemory(BoneMatrices, sizeof(_matrix) * 256);
				pMesh->Calc_BoneMatrices(BoneMatrices, m_CombinedTransformations);
				Set_Variable("g_BoneMatrices", BoneMatrices, sizeof(_matrix) * 256);

				if(iMaterialIndex != 0)
					m_pDeviceContext->DrawIndexed(3 * pMesh->Get_StratFaceCount(), 3 * pMesh->Get_StratFaceIndex(), pMesh->Get_StartVertexIndex());
			}
		}
	}
	else
	{
		for (_uint iMaterialIndex = 0; iMaterialIndex < m_iMaterialCount; ++iMaterialIndex)
		{
			Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE);
			/*Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS);*/
			FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

			for (auto& pMesh : m_SortedMeshes[iMaterialIndex])
				m_pDeviceContext->DrawIndexed(3 * pMesh->Get_StratFaceCount(), 3 * pMesh->Get_StratFaceIndex(), pMesh->Get_StartVertexIndex());
		}
	}

	return S_OK;
}

HRESULT CModel::Sort_MeshesByMaterial()
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

HRESULT CModel::Set_CenterBone(const char* pCenterBoneName)
{
	auto& iter = find_if(m_Nodes.begin(), m_Nodes.end(), [pCenterBoneName](CHierarchyNode* pNode) { return !strcmp(pCenterBoneName, pNode->Get_Name()); });
	if (iter == m_Nodes.end()) { return E_FAIL; }
	m_pCenterBoneNode = *iter;
	Safe_AddRef(m_pCenterBoneNode);

	return S_OK;
}

HRESULT CModel::Apply_PivotMatrix(_fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());

	if (0 < m_iAnimCount)
	{
		XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
		memcpy(&m_CombinedPivotMatrix, &m_PivotMatrix, sizeof(_float4x4));
		return S_OK;
	}
	else
	{
		m_pVectorPositions = new _vector[m_iVertexCount];

		for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
		{
			_vector	vAdjustedPosition = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[iIndex].vPosition), PivotMatrix);
			XMStoreFloat3(&m_pVertices[iIndex].vPosition, vAdjustedPosition);
			_vector	vAdjustedNormal = XMVector3TransformNormal(XMLoadFloat3(&m_pVertices[iIndex].vNormal), PivotMatrix);
			XMStoreFloat3(&m_pVertices[iIndex].vNormal, vAdjustedNormal);
			memcpy(&m_pVectorPositions[iIndex], &XMVectorSetW(vAdjustedPosition, 1.f), sizeof(_vector));
		}
	}

	return S_OK;
}

void CModel::Update_AnimTransformations(_double dTimeDelta)
{
	if (m_fLerpRatio > 0.f)
	{
		m_Anims[m_iCurAnimIndex]->Update_Transformations_Blend(m_dCurrentTime, m_iCurAnimFrame, m_AnimTransformations, m_PreAnimKeyFrames, m_fLerpRatio);
		m_fLerpRatio -= (_float)dTimeDelta * 8.f;
	}
	else
		m_Anims[m_iCurAnimIndex]->Update_Transformations(m_dCurrentTime, m_iCurAnimFrame, m_AnimTransformations);
}

void CModel::Update_CombinedTransformations()
{
	for (auto& pNode : m_Nodes)
	{
		_int iParentNodeIndex = pNode->Get_ParentNodeIndex();

		if (0 <= iParentNodeIndex)
			XMStoreFloat4x4(&m_CombinedTransformations[pNode->Get_NodeIndex()], XMLoadFloat4x4(&m_AnimTransformations[pNode->Get_NodeIndex()]) * XMLoadFloat4x4(&m_CombinedTransformations[iParentNodeIndex]));
		else
			XMStoreFloat4x4(&m_CombinedTransformations[pNode->Get_NodeIndex()], XMLoadFloat4x4(&m_AnimTransformations[pNode->Get_NodeIndex()]) * XMLoadFloat4x4(&m_CombinedPivotMatrix));
	}
}

#pragma region For_Buffer
HRESULT CModel::Create_Buffer(ID3D11Buffer ** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void * pSysMem, _uint iSysMemPitch, _uint iSysMemSlicePitch)
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
HRESULT CModel::Create_VIBuffer(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	/* For.VertexBuffer */
	m_iVertexBufferCount = 1;
	m_iVertexStride = sizeof(VTXMESH);
	Create_Buffer(&m_pVB, m_iVertexStride * m_iVertexCount, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, m_pVertices);
	Safe_Delete_Array(m_pVertices);

	/* For.IndexBuffer */
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iFaceStride = sizeof(POLYGON_INDICES32);
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Create_Buffer(&m_pIB, m_iFaceCount * m_iFaceStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0, 0, m_pFaces);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC	ElementDesc[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX",		0, DXGI_FORMAT_R32G32B32A32_UINT,	0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 60,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 6, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}
HRESULT CModel::SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC * pInputElementDesc, _uint iElementCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
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
#pragma endregion

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pMeshFilePath, const char * pMeshFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix, _bool bNeedCenterBone, const char * pCenterBoneName)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pMeshFilePath, pMeshFileName, pShaderFilePath, pTechniqueName, PivotMatrix, bNeedCenterBone, pCenterBoneName)))
	{
		MSG_BOX("Failed to Create Instance - CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone_Component(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_Release(InputLayout.pLayout);
		Safe_Release(InputLayout.pPass);
	}

	Safe_Release(m_pEffect);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	for (auto& pAnim : m_Anims)
		Safe_Release(pAnim);
	m_Anims.clear();

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

	for (auto& pNode : m_Nodes)
		Safe_Release(pNode);
	m_Nodes.clear();

	m_BaseTransformations.clear();
	m_AnimTransformations.clear();
	m_CombinedTransformations.clear();
	
	Safe_Release(m_pCenterBoneNode);

	if (false == m_isClone)
	{
		Safe_Release(m_pModel_Loader);
		Safe_Delete_Array(m_pVectorPositions);
		Safe_Delete_Array(m_pFaces);
	}

	CComponent::Free();
}

//HRESULT CModel::Update_State_DuringAnim(CTransform* pTransform)
//{
//	_vector vCurPos = XMVector3TransformCoord(XMVectorSet(m_vAnimDistFromCenter.x, m_vAnimDistFromCenter.y, m_vAnimDistFromCenter.z, 1.f), pTransform->Get_WorldMatrix());
//	pTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
//
//	if (true == m_bNeedCenterBone)
//	{
//		_uint iIndex = m_pCenterBoneNode->Get_NodeIndex();
//
//		_float fAdjPosX = (m_CombinedTransformations[iIndex]._41);
//		_float fAdjPosY = (m_CombinedTransformations[iIndex]._42);
//		_float fAdjPosZ = (m_CombinedTransformations[iIndex]._43);
//
//		m_CombinedPivotMatrix._41 -= fAdjPosX;
//		m_CombinedPivotMatrix._42 -= fAdjPosY;
//		m_CombinedPivotMatrix._43 -= fAdjPosZ;
//	}
//
//	return S_OK;
//}
//void CModel::Update_CombinedPivotMatrix()
//{
//	memcpy(&m_CombinedPivotMatrix, &m_PivotMatrix, sizeof(_float4x4));
//
//	_double dStartTime = 0.0;
//	//m_Anims[m_iCurrentAnimIndex]->Update_ChannelMatrices(dStartTime, m_iCurAnimKeyFrame, m_AnimTransformations, /*m_PreKeyFrames*/);
//	Update_CombinedTransformations();
//
//	if (true == m_bNeedCenterBone)
//	{
//		_uint iIndex = m_pCenterBoneNode->Get_NodeIndex();
//
//		_float fAdjPosX = (m_CombinedTransformations[iIndex]._41);
//		_float fAdjPosY = (m_CombinedTransformations[iIndex]._42);
//		_float fAdjPosZ = (m_CombinedTransformations[iIndex]._43);
//
//		m_CombinedPivotMatrix._41 -= fAdjPosX;
//		m_CombinedPivotMatrix._42 -= fAdjPosY;
//		m_CombinedPivotMatrix._43 -= fAdjPosZ;
//	}
//}
//
//void CModel::Update_DistanceFromStartPoint()
//{
//	if (true == m_bNeedCenterBone)
//		memcpy(&m_vAnimDistFromCenter, &m_CombinedTransformations[m_pCenterBoneNode->Get_NodeIndex()]._41, sizeof(_float) * 3);
//}