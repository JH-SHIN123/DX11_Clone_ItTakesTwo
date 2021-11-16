#include "..\public\VIBuffer_FontInstance.h"

CVIBuffer_FontInstance::CVIBuffer_FontInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_FontInstance::CVIBuffer_FontInstance(const CVIBuffer_FontInstance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_FontInstance::NativeConstruct_Prototype(_uint iNumInstance, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iNumInstance;

	/* For. VertexBuffer */
	VTXPOINTPOS* pVertices = new  VTXPOINTPOS[m_iMaxInstanceCount];
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
		pVertices[iIndex].vPosition = _float3(0.f, 0.f, 0.f);

	Store_VertexBufferInfo(2, 4, sizeof(VTXPOINTPOS));
	Create_Buffer(&m_pVB, m_iVertexCount * m_iVertexStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, pVertices);

	/* For.InstanceBuffer */
	m_pInstanceVertices = new VTXFONT[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vPosition	= _float3(0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vScale		= _float2(0.f, 0.f);
		m_pInstanceVertices[iIndex].vTexUV		= _float4(0.f, 0.f, 1.f, 1.f);
	}

	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXFONT), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXFONT), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	//D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "PSIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	//	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	//	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	//};

	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 20, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 5, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_FontInstance::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_FontInstance::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_FontInstance");

	return S_OK;
}

HRESULT CVIBuffer_FontInstance::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, (VTXFONT*)VertexMatrices, sizeof(VTXFONT) * iInstanceCount);
	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/* For.RenderBuffer */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXPOINTPOS), sizeof(VTXFONT) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawIndexedInstanced(6, iInstanceCount, 0, 0, 0);

	return S_OK;
}

CVIBuffer_FontInstance * CVIBuffer_FontInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_FontInstance* pInstance = new CVIBuffer_FontInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_FontInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_FontInstance::Clone_Component(void * pArg)
{
	CVIBuffer_FontInstance* pInstance = new CVIBuffer_FontInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_FontInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_FontInstance::Free()
{
	if (false == m_isClone)
		Safe_Delete_Array(m_pInstanceVertices);

	Safe_Release(m_pVBInstance);

	CVIBuffer::Free();
}