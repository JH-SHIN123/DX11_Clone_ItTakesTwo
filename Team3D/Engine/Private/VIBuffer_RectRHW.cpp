#include "..\public\VIBuffer_RectRHW.h"

CVIBuffer_RectRHW::CVIBuffer_RectRHW(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_RectRHW::CVIBuffer_RectRHW(const CVIBuffer_RectRHW & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_RectRHW::NativeConstruct_Prototype(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	D3D11_VIEWPORT ViewPort;
	_uint iNumViewPort = 1;
	m_pDeviceContext->RSGetViewports(&iNumViewPort, &ViewPort);

	/* For. VertexBuffer */
	VTXTEXRHW* pVertices = new VTXTEXRHW[4];

	pVertices[0].vPosition	= _float4(fStartX / (ViewPort.Width * 0.5f) - 1.f, fStartY / (ViewPort.Height * -0.5f) + 1.f, 0.f, 1.f);
	pVertices[1].vPosition	= _float4((fStartX + fSizeX) / (ViewPort.Width * 0.5f) - 1.f, fStartY / (ViewPort.Height * -0.5f) + 1.f, 0.f, 1.f);
	pVertices[2].vPosition	= _float4((fStartX + fSizeX) / (ViewPort.Width * 0.5f) - 1.f, (fStartY + fSizeY) / (ViewPort.Height * -0.5f) + 1.f, 0.f, 1.f);
	pVertices[3].vPosition	= _float4(fStartX / (ViewPort.Width * 0.5f) - 1.f, (fStartY + fSizeY) / (ViewPort.Height * -0.5f) + 1.f, 0.f, 1.f);
	pVertices[0].vTexUV		= _float2(0.f, 0.f);
	pVertices[1].vTexUV		= _float2(1.f, 0.f);
	pVertices[2].vTexUV		= _float2(1.f, 1.f);
	pVertices[3].vTexUV		= _float2(0.f, 1.f);

	Store_VertexBufferInfo(1, 4, sizeof(VTXTEXRHW));
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

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 2, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_RectRHW::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);
	
	return S_OK;
}

CVIBuffer_RectRHW * CVIBuffer_RectRHW::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer_RectRHW* pInstance = new CVIBuffer_RectRHW(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(fStartX, fStartY, fSizeX, fSizeY, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_RectRHW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectRHW::Clone_Component(void * pArg)
{	
	CVIBuffer_RectRHW* pInstance = new CVIBuffer_RectRHW(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_RectRHW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectRHW::Free()
{
	CVIBuffer::Free();
}
