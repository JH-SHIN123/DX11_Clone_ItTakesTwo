#include "VIBuffer_PointInstance_Custom.h"

CVIBuffer_PointInstance_Custom::CVIBuffer_PointInstance_Custom(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_PointInstance_Custom::CVIBuffer_PointInstance_Custom(const CVIBuffer_PointInstance_Custom & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_PointInstance_Custom::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
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
	m_pInstanceVertices = new VTXMATRIX_CUSTOM_ST[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vRight		= _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vUp			= _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vLook		= _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[iIndex].vPosition	= _float4(0.f, 0.f, 0.f, 1.f);
		m_pInstanceVertices[iIndex].vTextureUV	= _float4(0.f, 0.f, 1.f, 1.f);
		m_pInstanceVertices[iIndex].vSize		= _float2(1.f, 1.f);
	}

	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXMATRIX_CUSTOM_ST), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	72, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // UV
		{ "PSIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		1,	80, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Size
	};

	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 7, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_PointInstance_Custom");

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Custom::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount, _uint iStartIndex)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, VertexMatrices, sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.RenderBuffer */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXPOS), sizeof(VTXMATRIX_CUSTOM_ST) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawInstanced(1, iInstanceCount, iStartIndex, iStartIndex);

	return S_OK;
}

CVIBuffer_PointInstance_Custom * CVIBuffer_PointInstance_Custom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_PointInstance_Custom* pInstance = new CVIBuffer_PointInstance_Custom(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_PointInstance_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_PointInstance_Custom::Clone_Component(void * pArg)
{
	CVIBuffer_PointInstance_Custom* pInstance = new CVIBuffer_PointInstance_Custom(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_PointInstance_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PointInstance_Custom::Free()
{
	if (false == m_isClone)
		Safe_Delete_Array(m_pInstanceVertices);

	Safe_Release(m_pVBInstance);

	CVIBuffer::Free();
}
