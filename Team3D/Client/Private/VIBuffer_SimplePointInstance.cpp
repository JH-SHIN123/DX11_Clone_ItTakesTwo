#include "stdafx.h"
#include "..\Public\VIBuffer_SimplePointInstance.h"

CVIBuffer_SimplePointInstance::CVIBuffer_SimplePointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_SimplePointInstance::CVIBuffer_SimplePointInstance(const CVIBuffer_SimplePointInstance & rhs)
	: CVIBuffer(rhs)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
{
}

HRESULT CVIBuffer_SimplePointInstance::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iMaxInstanceCount;
	m_eTopology			= D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* For.InstanceBuffer */
	m_pInstanceVertices = new VTXMATRIX[m_iMaxInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iMaxInstanceCount; ++iIndex)
	{
		m_pInstanceVertices[iIndex].vRight		= _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vUp			= _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vLook		= _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[iIndex].vPosition	= _float4(0.f, 0.f, 0.f, 1.f);
	}

	Store_VertexBufferInfo(1, m_iMaxInstanceCount, sizeof(VTXMATRIX));
	Create_Buffer(&m_pVB, m_iMaxInstanceCount * sizeof(VTXMATRIX), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 4, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_SimplePointInstance::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_SimplePointInstance::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_SimplePointInstance");

	return S_OK;
}

HRESULT CVIBuffer_SimplePointInstance::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, VertexMatrices, sizeof(VTXMATRIX) * iInstanceCount);
	m_pDeviceContext->Unmap(m_pVB, 0);


	/* For.RenderBuffer */
	_uint iOffSet = 0;

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, &m_pVB, &m_iVertexStride, &iOffSet);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawInstanced(1, iInstanceCount, 0, 0);

	return S_OK;
}

CVIBuffer_SimplePointInstance * CVIBuffer_SimplePointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_SimplePointInstance* pInstance = new CVIBuffer_SimplePointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_SimplePointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_SimplePointInstance::Clone_Component(void * pArg)
{
	CVIBuffer_SimplePointInstance* pInstance = new CVIBuffer_SimplePointInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_SimplePointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SimplePointInstance::Free()
{
	if (false == m_isClone)
		Safe_Delete_Array(m_pInstanceVertices);

	CVIBuffer::Free();
}
