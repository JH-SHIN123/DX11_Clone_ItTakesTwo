#include "..\public\VIBuffer_RectInstance.h"

CVIBuffer_RectInstance::CVIBuffer_RectInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_RectInstance::CVIBuffer_RectInstance(const CVIBuffer_RectInstance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_RectInstance::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iMaxInstanceCount;

	/* For. VertexBuffer */
	VTXTEX*	pVertices = new VTXTEX[4];

	pVertices[0].vPosition	= _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV		= _float2(0.f, 0.f);
	pVertices[1].vPosition	= _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV		= _float2(1.f, 0.f);
	pVertices[2].vPosition	= _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV		= _float2(1.f, 1.f);
	pVertices[3].vPosition	= _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV		= _float2(0.f, 1.f);

	Store_VertexBufferInfo(2, 4, sizeof(VTXTEX));
	Create_Buffer(&m_pVB, m_iVertexCount * m_iVertexStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, pVertices);

	/* For.IndexBuffer */
	POLYGON_INDICES16* pFaces = new POLYGON_INDICES16[2 * m_iMaxInstanceCount];

	_uint iFaceIndex = 0;

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		pFaces[iFaceIndex]._0 = 0;
		pFaces[iFaceIndex]._1 = 1;
		pFaces[iFaceIndex]._2 = 2;
		++iFaceIndex;
		pFaces[iFaceIndex]._0 = 0;
		pFaces[iFaceIndex]._1 = 2;
		pFaces[iFaceIndex]._2 = 3;
		++iFaceIndex;
	}

	Store_IndexBufferInfo(2 * m_iMaxInstanceCount, DXGI_FORMAT_R16_UINT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Create_Buffer(&m_pIB, m_iFaceCount * m_iFaceStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0, 0, pFaces);

	/* For.InstanceBuffer */
	m_pInstanceVertices = new VTXMATRIX[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vRight	= _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vUp		= _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vLook		= _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[iIndex].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXMATRIX), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 6, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_RectInstance::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_RectInstance::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_RectInstance");

	return S_OK;
}

HRESULT CVIBuffer_RectInstance::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, VertexMatrices, sizeof(VTXMATRIX) * iInstanceCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.RenderBuffer */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXTEX), sizeof(VTXMATRIX) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawIndexedInstanced(6, iInstanceCount, 0, 0, 0);

	return S_OK;
}

CVIBuffer_RectInstance * CVIBuffer_RectInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_RectInstance* pInstance = new CVIBuffer_RectInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_RectInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectInstance::Clone_Component(void * pArg)
{
	CVIBuffer_RectInstance* pInstance = new CVIBuffer_RectInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_RectInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectInstance::Free()
{
	Safe_Release(m_pVBInstance);

	CVIBuffer::Free();
}