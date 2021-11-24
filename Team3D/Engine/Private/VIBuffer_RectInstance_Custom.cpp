#include "..\Public\VIBuffer_RectInstance_Custom.h"
#include "Pipeline.h"

CVIBuffer_RectInstance_Custom::CVIBuffer_RectInstance_Custom(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_RectInstance_Custom::CVIBuffer_RectInstance_Custom(const CVIBuffer_RectInstance_Custom & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iMaxInstanceCount(rhs.m_iMaxInstanceCount)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_RectInstance_Custom::NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer::NativeConstruct_Prototype();

	m_iMaxInstanceCount = iMaxInstanceCount;

	/* For. VertexBuffer */
	VTXTEX_TRIPLE*	pVertices = new VTXTEX_TRIPLE[4];

	pVertices[0].vPosition	= _float3(-0.5f, 0.f, 0.5f);
	pVertices[0].vTexUV		= _float2(0.f, 0.f);
	pVertices[0].vTexUV_2	= _float2(0.f, 0.f); // Weight
	pVertices[0].vTexUV_3	= _float2(0.f, 0.f); // Color Ramp

	pVertices[1].vPosition	= _float3(0.5f, 0.f, 0.5f);
	pVertices[1].vTexUV		= _float2(1.f, 0.f);
	pVertices[1].vTexUV_2	= _float2(0.125f, 0.f);
	pVertices[1].vTexUV_3	= _float2(1.f, 0.f);

	pVertices[2].vPosition	= _float3(0.5f, 0.f, -0.5f);
	pVertices[2].vTexUV		= _float2(1.f, 1.f);
	pVertices[2].vTexUV_2	= _float2(0.125f, 0.125f);
	pVertices[2].vTexUV_3	= _float2(1.f, 1.f);

	pVertices[3].vPosition	= _float3(-0.5f, 0.f, -0.5f);
	pVertices[3].vTexUV		= _float2(0.f, 1.f);
	pVertices[3].vTexUV_2	= _float2(0.f, 0.125f);
	pVertices[3].vTexUV_3	= _float2(0.f, 1.f);


	Store_VertexBufferInfo(2, 4, sizeof(VTXTEX_TRIPLE));
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
		m_pInstanceVertices[iIndex].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[iIndex].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[iIndex].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	Create_Buffer(&m_pVBInstance, m_iMaxInstanceCount * sizeof(VTXMATRIX), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, sizeof(VTXMATRIX), m_pInstanceVertices, false);

	/* For.InputLayouts*/
	D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	FAILED_CHECK_RETURN(SetUp_InputLayouts(ElementDesc, 8, pShaderFilePath, pTechniqueName), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_RectInstance_Custom::NativeConstruct(void * pArg)
{
	CVIBuffer::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer_RectInstance_Custom::Render(_uint iPassIndex)
{
	MSG_BOX("Wrong Access - CVIBuffer_RectInstance_Custom");

	return S_OK;
}

HRESULT CVIBuffer_RectInstance_Custom::Render(_uint iPassIndex, void * VertexMatrices, _uint iInstanceCount)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_iMaxInstanceCount >= iInstanceCount, E_FAIL);

	/* For.UpdateBuffer */
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXMATRIX* pWorldMatrix = new VTXMATRIX[iInstanceCount];
	memcpy(pWorldMatrix, VertexMatrices, sizeof(VTXMATRIX) * iInstanceCount);

	for (_uint iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
 		_float4x4 vWolrd4x4;
 		memcpy(&vWolrd4x4, &pWorldMatrix[iIndex], sizeof(VTXMATRIX));

		pWorldMatrix[iIndex].vRight		= { vWolrd4x4._11, vWolrd4x4._21 , vWolrd4x4._31 , vWolrd4x4._41 };
		pWorldMatrix[iIndex].vUp		= { vWolrd4x4._12, vWolrd4x4._22 , vWolrd4x4._32 , vWolrd4x4._42 };
		pWorldMatrix[iIndex].vLook		= { vWolrd4x4._13, vWolrd4x4._23 , vWolrd4x4._33 , vWolrd4x4._43 };
		pWorldMatrix[iIndex].vPosition	= { vWolrd4x4._14, vWolrd4x4._24 , vWolrd4x4._34 , vWolrd4x4._44 };
	}
	memcpy(MappedSubResource.pData, pWorldMatrix, sizeof(VTXMATRIX) * iInstanceCount); // 전치로 변경해서 던지기

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
	Safe_Delete_Array(pWorldMatrix);

	/* For.RenderBuffer */
	ID3D11Buffer* pBuffers[2] = { m_pVB, m_pVBInstance };
	_uint iStrides[2] = { sizeof(VTXTEX_TRIPLE), sizeof(VTXMATRIX) };
	_uint iOffsets[2] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);

	FAILED_CHECK_RETURN(m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->DrawIndexedInstanced(6, iInstanceCount, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_RectInstance_Custom::Set_DefaultVariables()
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

CVIBuffer_RectInstance_Custom * CVIBuffer_RectInstance_Custom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iMaxInstanceCount, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_RectInstance_Custom* pInstance = new CVIBuffer_RectInstance_Custom(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaxInstanceCount, pShaderFilePath, pTechniqueName)))
	{
		MSG_BOX("Failed to Create Instance - CVIBuffer_RectInstance_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectInstance_Custom::Clone_Component(void * pArg)
{
	CVIBuffer_RectInstance_Custom* pInstance = new CVIBuffer_RectInstance_Custom(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVIBuffer_RectInstance_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectInstance_Custom::Free()
{
	Safe_Release(m_pVBInstance);
	
	Safe_Delete(m_pInstanceVertices);
	CVIBuffer::Free();
}