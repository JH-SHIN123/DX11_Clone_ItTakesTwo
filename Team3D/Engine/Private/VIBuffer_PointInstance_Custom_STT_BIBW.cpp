#include "..\Public\VIBuffer_PointInstance_Custom_STT_BIBW.h"
#include "Pipeline.h"

CVIBuffer_PointInstance_Custom_STT_BIBW::CVIBuffer_PointInstance_Custom_STT_BIBW(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_PointInstance_Custom_STT_BIBW::CVIBuffer_PointInstance_Custom_STT_BIBW(const CVIBuffer_PointInstance_Custom_STT_BIBW & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_PointInstance_Custom_STT_BIBW::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iMaxInstanceCount;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* For. VertexBuffer */
	VTXPOS* pVertices = new VTXPOS[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
		pVertices[iIndex].vPosition = _float3(0.f, 0.f, 0.f);

	Store_VertexBufferInfo(2, m_iMaxInstanceCount, sizeof(VTXPOS));
	Create_Buffer(&m_pVB, m_iVertexCount * m_iVertexStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, pVertices);

	/* For.InstanceBuffer */
	m_pInstanceVertices = new VTXMATRIX_CUSTOM_STT_BIBW[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vRight			= _float4(1.f, 0.f, 0.f, 0.f); // 매트릭스
		m_pInstanceVertices[iIndex].vUp				= _float4(0.f, 1.f, 0.f, 0.f); // 매트릭스
		m_pInstanceVertices[iIndex].vLook			= _float4(0.f, 0.f, 1.f, 0.f); // 매트릭스
		m_pInstanceVertices[iIndex].vPosition		= _float4(0.f, 0.f, 0.f, 1.f); // 매트릭스
		m_pInstanceVertices[iIndex].vSize			= _float2(1.f, 1.f);
		m_pInstanceVertices[iIndex].vTextureUV		= _float4(0.f, 0.f, 1.f, 1.f);
		m_pInstanceVertices[iIndex].fTime			= 0.f;
		m_pInstanceVertices[iIndex].vBlendIndex		= { 0, 0, 0, 0 };
		m_pInstanceVertices[iIndex].vBlendWeight	= { 0.f, 0.f, 0.f, 0.f };
	}

	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXMATRIX_CUSTOM_STT_BIBW), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX_CUSTOM_STT_BIBW), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	 D3D11_INPUT_PER_VERTEX_DATA,   0 },

		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,	 D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "PSIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		1,	64,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Size
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	72,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // UV
		{ "TEXCOORD",	1, DXGI_FORMAT_R32_FLOAT,			1,	88,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Time
		{ "TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_UINT,	1,	104, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Blend Index
		{ "TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	120, D3D11_INPUT_PER_INSTANCE_DATA, 1 }  // Blend Weight

	};

	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 10, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom_STT_BIBW::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom_STT_BIBW::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_PointInstance_Custom_STT_BIBW");

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom_STT_BIBW::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount, _uint iStartIndex)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, VertexMatrices, sizeof(VTXMATRIX_CUSTOM_STT_BIBW) * iInstanceCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.RenderBuffer */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXPOS), sizeof(VTXMATRIX_CUSTOM_STT_BIBW) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawInstanced(1, iInstanceCount, iStartIndex, iStartIndex);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom_STT_BIBW::Set_DefaultVariables()
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

CVIBuffer_PointInstance_Custom_STT_BIBW * CVIBuffer_PointInstance_Custom_STT_BIBW::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_PointInstance_Custom_STT_BIBW* pInstance = new CVIBuffer_PointInstance_Custom_STT_BIBW(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_PointInstance_Custom_STT_BIBW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_PointInstance_Custom_STT_BIBW::Clone_Component(void * pArg)
{
	CVIBuffer_PointInstance_Custom_STT_BIBW* pInstance = new CVIBuffer_PointInstance_Custom_STT_BIBW(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_PointInstance_Custom_STT_BIBW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PointInstance_Custom_STT_BIBW::Free()
{
	if (false == m_isClone)
		Safe_Delete_Array(m_pInstanceVertices);

	Safe_Release(m_pVBInstance);

	CVIBuffer::Free();
}
