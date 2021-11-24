#include "..\Public\VIBuffer_Rect_TripleUV.h"

CVIBuffer_Rect_TripleUV::CVIBuffer_Rect_TripleUV(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect_TripleUV::CVIBuffer_Rect_TripleUV(const CVIBuffer_Rect_TripleUV & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_TripleUV::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	/* For.VertexBuffer */
	VTXTEX_TRIPLE* pVertices = new VTXTEX_TRIPLE[4];

	pVertices[0].vPosition = _float3(-0.5f, 0.f, 0.5f);
	pVertices[0].vTexUV		= _float2(0.f, 0.f);
	pVertices[0].vTexUV_2	= _float2(0.f, 0.f); // Weight
	pVertices[0].vTexUV_3	= _float2(0.f, 0.f); // Color Ramp

	pVertices[1].vPosition = _float3(0.5f, 0.f, 0.5f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);
	pVertices[1].vTexUV_2 = _float2(0.125f, 0.f);
	pVertices[1].vTexUV_3 = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, 0.f, -0.5f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);
	pVertices[2].vTexUV_2 = _float2(0.125f, 0.125f);
	pVertices[2].vTexUV_3 = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, 0.f, -0.5f);
	pVertices[3].vTexUV = _float2(0.f, 1.f);
	pVertices[3].vTexUV_2 = _float2(0.f, 0.125f);
	pVertices[3].vTexUV_3 = _float2(0.f, 1.f);

	Store_VertexBufferInfo(1, 4, sizeof(VTXTEX_TRIPLE));
	Create_Buffer(&m_pVB, m_iVertexCount * m_iVertexStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, pVertices);

	/* For.IndexBuffer */
	POLYGON_INDICES16* pFaces = new POLYGON_INDICES16[2];

	pFaces[0]._0 = 0;
	pFaces[0]._1 = 1;
	pFaces[0]._2 = 2;
	pFaces[1]._0 = 0;
	pFaces[1]._1 = 2;
	pFaces[1]._2 = 3;

	Store_IndexBufferInfo(2, DXGI_FORMAT_R16_UINT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Create_Buffer(&m_pIB, m_iFaceCount * m_iFaceStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0, 0, pFaces);

	/* For.InputLayouts */
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 4, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_Rect_TripleUV::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

CVIBuffer_Rect_TripleUV * CVIBuffer_Rect_TripleUV::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_Rect_TripleUV* pInstance = new CVIBuffer_Rect_TripleUV(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_Rect_TripleUV");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_TripleUV::Clone_Component(void * pArg)
{
	CVIBuffer_Rect_TripleUV*	pInstance = new CVIBuffer_Rect_TripleUV(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_Rect_TripleUV");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_TripleUV::Free()
{
	__super::Free();
}
