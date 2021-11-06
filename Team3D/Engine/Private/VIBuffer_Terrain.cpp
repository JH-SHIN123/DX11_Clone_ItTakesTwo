#include "..\public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iVertexCountX(rhs.m_iVertexCountX)
	, m_iVertexCountZ(rhs.m_iVertexCountZ)
{
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(_uint iVertexCountX, _uint iVertexCountZ, _float fInterval, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iVertexCountX	= iVertexCountX;
	m_iVertexCountZ	= iVertexCountZ;

	_float	fVertexInterval = fInterval;
	_uint	iVertexCount	= iVertexCountX * iVertexCountZ;
	_uint	iFaceCount		= (iVertexCountX - 1) * (iVertexCountZ - 1) * 2;

	/* For.VertexBuffer */
	VTXNORTEX* pVertices = new VTXNORTEX[iVertexCount];

	for (_uint i = 0; i < iVertexCountZ; ++i)
	{
		for (_uint j = 0; j < iVertexCountX; ++j)
		{
			_uint	iIndex = i * iVertexCountX + j;

			pVertices[iIndex].vPosition	= _float3(j * fVertexInterval, 0.f, i * fVertexInterval);
			pVertices[iIndex].vNormal	= _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexUV	= _float2(j / (iVertexCountX - 1.f), i / (iVertexCountZ - 1.f));
		}
	}

	Store_VertexBufferInfo(1, iVertexCount, sizeof(VTXNORTEX));

	/* For.IndexBuffer */
	POLYGON_INDICES32* pFaces = new POLYGON_INDICES32[iFaceCount];

	_uint iFaceIndex = 0;

	for (_uint i = 0; i < iVertexCountZ - 1; ++i)
	{
		for (_uint j = 0; j < iVertexCountX - 1; ++j)
		{
			_uint	iIndex = i * iVertexCountX + j;

			pFaces[iFaceIndex]._0 = iIndex + iVertexCountX;
			pFaces[iFaceIndex]._1 = iIndex + iVertexCountX + 1;
			pFaces[iFaceIndex]._2 = iIndex + 1;
			++iFaceIndex;

			pFaces[iFaceIndex]._0 = iIndex + iVertexCountX;
			pFaces[iFaceIndex]._1 = iIndex + 1;
			pFaces[iFaceIndex]._2 = iIndex;
			++iFaceIndex;
		}
	}

	Store_IndexBufferInfo(iFaceCount, DXGI_FORMAT_R32_UINT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////////// VertexBuffer_Normal, 지형 높이 있으면 적용하고 기본 Normal의 y를 0으로 바꿀 것.
	////////for (_uint iIndex = 0; iIndex < iFaceIndex; ++iIndex)
	////////{
	////////	_vector	vPos0 = XMLoadFloat3(&pVertices[pFaces[iIndex]._0].vPosition);
	////////	_vector	vPos1 = XMLoadFloat3(&pVertices[pFaces[iIndex]._1].vPosition);
	////////	_vector	vPos2 = XMLoadFloat3(&pVertices[pFaces[iIndex]._2].vPosition);
	////////	_vector vNormal = XMVector3Normalize(XMVector3Cross(vPos1 - vPos0, vPos2 - vPos1));

	////////	XMStoreFloat3(&pVertices[pFaces[iIndex]._0].vNormal, XMLoadFloat3(&pVertices[pFaces[iIndex]._0].vNormal) + vNormal);
	////////	XMStoreFloat3(&pVertices[pFaces[iIndex]._1].vNormal, XMLoadFloat3(&pVertices[pFaces[iIndex]._1].vNormal) + vNormal);
	////////	XMStoreFloat3(&pVertices[pFaces[iIndex]._2].vNormal, XMLoadFloat3(&pVertices[pFaces[iIndex]._2].vNormal) + vNormal);
	////////}

	////////for (_uint iIndex = 0; iIndex < iVertexCount; ++iIndex)
	////////	XMStoreFloat3(&pVertices[iIndex].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndex].vNormal)));

	/* For.CreateBuffer */
	// Vertex
	Create_Buffer(&m_pVB, m_iVertexCount * m_iVertexStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0, m_iVertexStride, pVertices);
	// Index
	Create_Buffer(&m_pIB, m_iFaceCount * m_iFaceStride, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0, 0, pFaces);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC	ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA }
	};
	FAILED_CHECK_RETURN(CVIBuffer::SetUp_InputLayouts(ElementDesc, 3, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iVertexCountX, _uint iVertexCountZ, _float fInterval, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iVertexCountX, iVertexCountZ, fInterval, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone_Component(void * pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	CVIBuffer::Free();
}