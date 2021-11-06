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
	, m_pModel_Loader				(rhs.m_pModel_Loader)
	, m_pVertices					(rhs.m_pVertices)
	, m_pFaces						(rhs.m_pFaces)
	, m_Meshes						(rhs.m_Meshes)
	, m_Nodes						(rhs.m_Nodes)
	, m_Anims						(rhs.m_Anims)
	, m_iMeshCount					(rhs.m_iMeshCount)
	, m_iNodeCount					(rhs.m_iNodeCount)
	, m_iAnimCount					(rhs.m_iAnimCount)
	, m_PivotMatrix					(rhs.m_PivotMatrix)
	, m_CombinedPivotMatrix			(rhs.m_CombinedPivotMatrix)
	, m_iCurrentAnimIndex			(rhs.m_iCurrentAnimIndex)
	, m_dCurrectTime				(rhs.m_dCurrectTime)
	, m_CurrentKeyFrames			(rhs.m_CurrentKeyFrames)
	, m_AnimTransformations			(rhs.m_AnimTransformations)
	, m_CombinedTransformations		(rhs.m_CombinedTransformations)
	, m_iAdjBoneCount				(rhs.m_iAdjBoneCount)
	, m_pAdjBoneNode1				(rhs.m_pAdjBoneNode1)
	, m_pAdjBoneNode2				(rhs.m_pAdjBoneNode2)
	, m_fAnimDistX					(rhs.m_fAnimDistX)
	, m_fAnimDistZ					(rhs.m_fAnimDistZ)
	, m_pPreAnim					(rhs.m_pPreAnim)
	, m_dPreStopTime				(rhs.m_dPreStopTime)
	, m_dPreAnimDuration			(rhs.m_dPreAnimDuration)
	, m_dPreAnimTime				(rhs.m_dPreAnimTime)
	, m_PreKeyFrames				(rhs.m_PreKeyFrames)
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
{
	Safe_AddRef(m_pModel_Loader);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pNode : m_Nodes)
		Safe_AddRef(pNode);

	for (auto& pAnim : m_Anims)
		Safe_AddRef(pAnim);

	Safe_AddRef(m_pAdjBoneNode1);
	Safe_AddRef(m_pAdjBoneNode2);

	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pEffect);

	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_AddRef(InputLayout.pLayout);
		Safe_AddRef(InputLayout.pPass);
	}
}

const _uint CModel::Get_MeshCount() const
{
	return m_iMeshCount;
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

_fvector CModel::Get_AdjustedPos(_fmatrix WorldMatrix) const
{
	_vector vCurPos = XMVector3TransformCoord(XMVectorSet(m_fAnimDistX, 0.f, m_fAnimDistZ, 1.f), WorldMatrix);

	return vCurPos;
}

const _uint CModel::Get_CurrentAnimIndex() const
{
	return m_iCurrentAnimIndex;
}

const _float CModel::Get_ProgressAnim() const
{
	return m_fProgressAnim;
}

const _double CModel::Get_PreAnimTime() const
{
	return m_dPreAnimTime;
}

const _bool CModel::Check_ReserveNextAnim() const
{
	return m_bReserveNextAnim;
}

void CModel::Initialize_CurrentKeyFrames()
{
	for (auto& iKeyFrame : m_CurrentKeyFrames)
		iKeyFrame = 0;
}

HRESULT CModel::Set_Animation(_uint iAnimIndex, CTransform * pTransform)
{
	if (m_iAnimCount <= iAnimIndex)
		return E_FAIL;

	m_pPreAnim = m_Anims[m_iCurrentAnimIndex];
	m_dPreAnimDuration = 0.3;
	m_dPreAnimTime = 0.3;
	m_dPreStopTime = m_dCurrectTime;
	m_PreKeyFrames.swap(m_CurrentKeyFrames);

	Initialize_CurrentKeyFrames();
	m_dCurrectTime = 0.0;
	m_iCurrentAnimIndex = iAnimIndex;
	m_bReserveNextAnim = false;

	if (nullptr != m_pStepCounter)
	{
		++(*m_pStepCounter);
		m_pStepCounter = nullptr;
	}

	_vector vCurPos = XMVector3TransformCoord(XMVectorSet(m_fAnimDistX, 0.f, m_fAnimDistZ, 1.f), pTransform->Get_WorldMatrix());
	pTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

	/* Update_PivotMatrix */
	Update_CombinedPivotMatrix();

	return S_OK;
}

HRESULT CModel::Set_NextAnimIndex(_uint iAnimIndex)
{
	NULL_CHECK_RETURN(iAnimIndex < m_iAnimCount, E_FAIL);

	m_iNextAnimIndex = iAnimIndex;
	m_bReserveNextAnim = true;

	return S_OK;
}

void CModel::Set_StepCounter(_uint * pStepCounter)
{
	m_pStepCounter = pStepCounter;
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

HRESULT CModel::NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix, _uint iAdjBoneCount, const char* pBoneName1, const char* pBoneName2)
{
	NULL_CHECK_RETURN(m_pModel_Loader, E_FAIL);

	CComponent::NativeConstruct_Prototype();

	FAILED_CHECK_RETURN(m_pModel_Loader->Load_ModelFromFile(this, pMeshFilePath, pMeshFileName), E_FAIL);
	FAILED_CHECK_RETURN(Set_CenterBone(iAdjBoneCount, pBoneName1, pBoneName2), E_FAIL);
	FAILED_CHECK_RETURN(Apply_PivotMatrix(PivotMatrix), E_FAIL);
	FAILED_CHECK_RETURN(Create_VIBuffer(pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	if (0 < m_iAnimCount)
		Update_CombinedPivotMatrix();

	return S_OK;
}

HRESULT CModel::Bring_Containers(VTXMESH* pVertices, _uint iVertexCount, POLYGON_INDICES32* pFaces, _uint iFaceCount, vector<class CMesh*> & Meshes, vector<class CHierarchyNode*> & Nodes, vector<class CAnim*> & Anims)
{
	m_iVertexCount		= iVertexCount;
	m_iFaceCount		= iFaceCount;
	m_iMeshCount		= (_uint)Meshes.size();
	m_iNodeCount		= (_uint)Nodes.size();
	m_iAnimCount		= (_uint)Anims.size();

	/* Bring_Containers */
	m_pVertices = pVertices;
	m_pFaces	= pFaces;
	m_Meshes.swap(Meshes);
	m_Nodes.swap(Nodes);
	m_Anims.swap(Anims);

	m_CurrentKeyFrames.resize(m_iNodeCount);
	m_AnimTransformations.resize(m_iNodeCount);
	m_CombinedTransformations.resize(m_iNodeCount);

	m_PreKeyFrames.resize(m_iNodeCount);

	return S_OK;
}

HRESULT CModel::Set_Variable(const char * pConstantName, void * pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);
	
	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CModel::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView* pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectShaderResourceVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetResource(pResourceView);
}

HRESULT CModel::Update_NodeMatrices(_double dTimeDelta, CTransform * pTransform)
{
	if (0 == m_iAnimCount)
		return S_OK;

	_double dDuration		= m_Anims[m_iCurrentAnimIndex]->Get_Duration();
	_double dTicksPerSecond	= m_Anims[m_iCurrentAnimIndex]->Get_TicksPerSecond();

	m_dCurrectTime += fmod(dTicksPerSecond * dTimeDelta, dDuration);

	if (m_dCurrectTime >= dDuration)
	{
		m_pPreAnim			= m_Anims[m_iCurrentAnimIndex];
		m_dPreAnimDuration	= 0.3;
		m_dPreAnimTime		= 0.3;
		m_dPreStopTime		= -1.0;
		m_PreKeyFrames.swap(m_CurrentKeyFrames);

		m_dCurrectTime		-= dDuration;
		Initialize_CurrentKeyFrames();
		m_iCurrentAnimIndex	= m_iNextAnimIndex;
		m_bReserveNextAnim	= false;

		if (nullptr != m_pStepCounter)
		{
			++(*m_pStepCounter);
			m_pStepCounter = nullptr;
		}

		_vector vCurPos		= XMVector3TransformCoord(XMVectorSet(m_fAnimDistX, 0.f, m_fAnimDistZ, 1.f), pTransform->Get_WorldMatrix());
		pTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

		/* Update_PivotMatrix */
		Update_CombinedPivotMatrix();
	}

	m_dPreAnimTime -= dTimeDelta;
	if (m_dPreAnimTime <= 0.0)
		m_dPreAnimTime = 0.0;

	/* Update_AnimTransformations */
	if (m_pPreAnim == m_Anims[m_iCurrentAnimIndex])
		m_Anims[m_iCurrentAnimIndex]->Update_ChannelMatrices(m_dCurrectTime, m_CurrentKeyFrames, m_AnimTransformations, m_PreKeyFrames);
	else
		m_Anims[m_iCurrentAnimIndex]->Update_ChannelMatrices(m_dCurrectTime, m_CurrentKeyFrames, m_AnimTransformations, m_PreKeyFrames, m_pPreAnim, m_dPreStopTime, m_dPreAnimDuration, m_dPreAnimTime);

	/* Update_CombinedTransformations */
	Update_CombinedTransformations();

	/* Update_Distance_From_StartPoint */
	Update_DistanceFromStartPoint();

	/* Update Progress */
	m_fProgressAnim = _float(m_dCurrectTime / dDuration);

	return S_OK;
}

HRESULT CModel::Bind_VIBuffer()
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);

	_uint	iOffSet = 0;

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, &m_pVB, &m_iVertexStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CModel::Render_Model(_uint iMeshIndex, _uint iPassIndex)
{
	CMesh*	pMesh	= m_Meshes[iMeshIndex];
	_vector	vColor	= pMesh->Get_MeshColor();

	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	if (0 != m_Meshes[iMeshIndex]->Get_BoneCount())
	{
		_matrix	BoneMatrices[256];
		ZeroMemory(BoneMatrices, sizeof(_matrix) * 256);
		pMesh->Calc_BoneMatrices(BoneMatrices, m_CombinedTransformations);
		Set_Variable("g_BoneMatrices", BoneMatrices, sizeof(_matrix) * 256);
	}

	Set_Variable("g_vBaseColor", &vColor, sizeof(_vector));

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->DrawIndexed(3 * pMesh->Get_StratFaceCount(), 3 * pMesh->Get_StratFaceIndex(), pMesh->Get_StartVertexIndex());

	return S_OK;
}

HRESULT CModel::Update_State_DuringAnim(CTransform* pTransform)
{
	_vector vCurPos = XMVector3TransformCoord(XMVectorSet(m_fAnimDistX, 0.f, m_fAnimDistZ, 1.f), pTransform->Get_WorldMatrix());
	pTransform->Set_State(CTransform::STATE_POSITION, vCurPos);

	if (2 == m_iAdjBoneCount)
	{
		_uint iIndex1 = m_pAdjBoneNode1->Get_NodeIndex();
		_uint iIndex2 = m_pAdjBoneNode2->Get_NodeIndex();

		_float fAdjPosX = (m_CombinedTransformations[iIndex1]._41 + m_CombinedTransformations[iIndex2]._41) * 0.5f;
		_float fAdjPosZ = (m_CombinedTransformations[iIndex1]._43 + m_CombinedTransformations[iIndex2]._43) * 0.5f;

		m_CombinedPivotMatrix._41 -= fAdjPosX;
		m_CombinedPivotMatrix._43 -= fAdjPosZ;
	}
	else if (1 == m_iAdjBoneCount)
	{
		_uint iIndex = m_pAdjBoneNode1->Get_NodeIndex();

		_float fAdjPosX = (m_CombinedTransformations[iIndex]._41);
		_float fAdjPosZ = (m_CombinedTransformations[iIndex]._43);

		m_CombinedPivotMatrix._41 -= fAdjPosX;
		m_CombinedPivotMatrix._43 -= fAdjPosZ;
	}

	return S_OK;
}

HRESULT CModel::Create_Buffer(ID3D11Buffer ** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void * pSysMem, _uint iSysMemPitch, _uint iSysMemSlicePitch)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth				= iByteWidth;
	Desc.Usage					= Usage;
	Desc.BindFlags				= iBindFlags;
	Desc.CPUAccessFlags			= iCPUAccessFlags;
	Desc.MiscFlags				= iMiscFlags;
	Desc.StructureByteStride	= iStructureByteStride;

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem				= pSysMem;
	SubResourceData.SysMemPitch			= iSysMemPitch;
	SubResourceData.SysMemSlicePitch	= iSysMemSlicePitch;

	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&Desc, &SubResourceData, ppBuffer), E_FAIL);

	Safe_Delete_Array(pSysMem);

	return S_OK;
}

HRESULT CModel::Create_VIBuffer(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	/* For.VertexBuffer */
	m_iVertexBufferCount	= 1;
	m_iVertexStride			= sizeof(VTXMESH);
	Create_Buffer(&m_pVB, m_iVertexStride * m_iVertexCount, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, m_pVertices);

	/* For.IndexBuffer */
	m_eIndexFormat			= DXGI_FORMAT_R32_UINT;
	m_iFaceStride			= sizeof(POLYGON_INDICES32);
	m_eTopology				= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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

HRESULT CModel::Set_CenterBone(_uint iAdjBoneCount, const char * pBoneName1, const char * pBoneName2)
{
	m_iAdjBoneCount = iAdjBoneCount;

	if (2 == iAdjBoneCount)
	{
		auto& iter = find_if(m_Nodes.begin(), m_Nodes.end(), [pBoneName1](CHierarchyNode* pNode) { return !strcmp(pBoneName1, pNode->Get_Name()); });
		if (iter == m_Nodes.end()) { return E_FAIL; }
		m_pAdjBoneNode1 = *iter;
		Safe_AddRef(m_pAdjBoneNode1);

		iter = find_if(m_Nodes.begin(), m_Nodes.end(), [pBoneName2](CHierarchyNode* pNode) { return !strcmp(pBoneName2, pNode->Get_Name()); });
		if (iter == m_Nodes.end()) { return E_FAIL; }
		m_pAdjBoneNode2 = *iter;
		Safe_AddRef(m_pAdjBoneNode2);
	}
	else if (1 == iAdjBoneCount)
	{
		auto& iter = find_if(m_Nodes.begin(), m_Nodes.end(), [pBoneName1](CHierarchyNode* pNode) { return !strcmp(pBoneName1, pNode->Get_Name()); });
		if (iter == m_Nodes.end()) { return E_FAIL; }
		m_pAdjBoneNode1 = *iter;
		Safe_AddRef(m_pAdjBoneNode1);
	}

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
		for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
		{
			_vector	vAdjustedPosition = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[iIndex].vPosition), PivotMatrix);
			XMStoreFloat3(&m_pVertices[iIndex].vPosition, vAdjustedPosition);
			_vector	vAdjustedNormal = XMVector3TransformNormal(XMLoadFloat3(&m_pVertices[iIndex].vNormal), PivotMatrix);
			XMStoreFloat3(&m_pVertices[iIndex].vNormal, vAdjustedNormal);
		}
	}

	//for (_uint iIndex = 0; iIndex < m_iMeshCount; ++iIndex)
	//{
	//	if (0 < m_iAnimCount && 0 != m_Meshes[iIndex]->Get_BoneCount())
	//	{
	//		XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	//		memcpy(&m_CombinedPivotMatrix, &m_PivotMatrix, sizeof(_float4x4));
	//		return S_OK;
	//	}
	//}

	//_matrix CombinedPivot = PivotMatrix;

	//if (0 == m_iAnimCount && 1 == m_iAdjBoneCount)
	//	CombinedPivot = m_pAdjBoneNode1->Get_TransformationMatrix() * PivotMatrix;

	//for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
	//{
	//	_vector	vAdjustedPosition = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[iIndex].vPosition), CombinedPivot);
	//	XMStoreFloat3(&m_pVertices[iIndex].vPosition, vAdjustedPosition);
	//}

	return S_OK;
}

HRESULT CModel::SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC * pInputElementDesc, _uint iElementCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	_uint iFlag	= 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob*	pCompiledShaderCode		= nullptr;
	ID3DBlob*	pCompileErrorMsg		= nullptr;

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

void CModel::Update_CombinedTransformations()
{
	_matrix	TransformationMatrix;

	for (auto& pNode : m_Nodes)
	{
		_int	iConnectedChannelIndex = pNode->Get_AnimChannelIndex(m_iCurrentAnimIndex);
		_int	iParentNodeIndex = pNode->Get_ParentNodeIndex();

		if (-1 == iConnectedChannelIndex)
			TransformationMatrix = pNode->Get_TransformationMatrix();
		else
			TransformationMatrix = XMLoadFloat4x4(&m_AnimTransformations[iConnectedChannelIndex]);

		if (0 <= iParentNodeIndex)
			XMStoreFloat4x4(&m_CombinedTransformations[pNode->Get_NodeIndex()], TransformationMatrix * XMLoadFloat4x4(&m_CombinedTransformations[iParentNodeIndex]));
		else
			XMStoreFloat4x4(&m_CombinedTransformations[pNode->Get_NodeIndex()], TransformationMatrix * XMLoadFloat4x4(&m_CombinedPivotMatrix));
	}
}

void CModel::Update_CombinedPivotMatrix()
{
	memcpy(&m_CombinedPivotMatrix, &m_PivotMatrix, sizeof(_float4x4));

	_double dStartTime = 0.0;
	m_Anims[m_iCurrentAnimIndex]->Update_ChannelMatrices(dStartTime, m_CurrentKeyFrames, m_AnimTransformations, m_PreKeyFrames);
	Update_CombinedTransformations();

	if (2 == m_iAdjBoneCount)
	{
		_uint iIndex1 = m_pAdjBoneNode1->Get_NodeIndex();
		_uint iIndex2 = m_pAdjBoneNode2->Get_NodeIndex();

		_float fAdjPosX = (m_CombinedTransformations[iIndex1]._41 + m_CombinedTransformations[iIndex2]._41) * 0.5f;
		_float fAdjPosZ = (m_CombinedTransformations[iIndex1]._43 + m_CombinedTransformations[iIndex2]._43) * 0.5f;

		m_CombinedPivotMatrix._41 -= fAdjPosX;
		m_CombinedPivotMatrix._43 -= fAdjPosZ;
	}
	else if (1 == m_iAdjBoneCount)
	{
		_uint iIndex = m_pAdjBoneNode1->Get_NodeIndex();

		_float fAdjPosX = (m_CombinedTransformations[iIndex]._41);
		_float fAdjPosZ = (m_CombinedTransformations[iIndex]._43);

		m_CombinedPivotMatrix._41 -= fAdjPosX;
		m_CombinedPivotMatrix._43 -= fAdjPosZ;
	}
}

void CModel::Update_DistanceFromStartPoint()
{
	if (2 == m_iAdjBoneCount)
	{
		_uint iIndex1 = m_pAdjBoneNode1->Get_NodeIndex();
		_uint iIndex2 = m_pAdjBoneNode2->Get_NodeIndex();

		m_fAnimDistX = (m_CombinedTransformations[iIndex1]._41 + m_CombinedTransformations[iIndex2]._41) * 0.5f;
		m_fAnimDistZ = (m_CombinedTransformations[iIndex1]._43 + m_CombinedTransformations[iIndex2]._43) * 0.5f;
	}
	else if (1 == m_iAdjBoneCount)
	{
		_uint iIndex = m_pAdjBoneNode1->Get_NodeIndex();

		m_fAnimDistX = m_CombinedTransformations[iIndex]._41;
		m_fAnimDistZ = m_CombinedTransformations[iIndex]._43;
	}
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pMeshFilePath, const char * pMeshFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix, _uint iAdjBoneCount, const char * pBoneName1, const char * pBoneName2)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pMeshFilePath, pMeshFileName, pShaderFilePath, pTechniqueName, PivotMatrix, iAdjBoneCount, pBoneName1, pBoneName2)))
	{
		MSG_BOX("Failed to Creating Instance(CModel).");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone_Component(void * pArg)
{
	CModel*	pClone = new CModel(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Creating Instance(CModel).");
		Safe_Release(pClone);
	}

	return pClone;
}

void CModel::Free()
{
	CComponent::Free();

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

	for (auto& pNode : m_Nodes)
		Safe_Release(pNode);
	m_Nodes.clear();

	m_CurrentKeyFrames.clear();
	m_AnimTransformations.clear();
	m_PreKeyFrames.clear();

	Safe_Release(m_pAdjBoneNode1);
	Safe_Release(m_pAdjBoneNode2);


	Safe_Release(m_pModel_Loader);
}