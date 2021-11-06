#include "..\public\VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	/* For.VertexBuffer */
	VTXTEX* pVertices = new VTXTEX[4];

	pVertices[0].vPosition	= _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV		= _float2(0.f, 0.f);
	pVertices[1].vPosition	= _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV		= _float2(1.f, 0.f);
	pVertices[2].vPosition	= _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV		= _float2(1.f, 1.f);
	pVertices[3].vPosition	= _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV		= _float2(0.f, 1.f);

	Store_VertexBufferInfo(1, 4, sizeof(VTXTEX));
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 2, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_Rect::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

CVIBuffer_Rect * CVIBuffer_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect::Clone_Component(void * pArg)
{
	CVIBuffer_Rect*	pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	CVIBuffer::Free();
}
