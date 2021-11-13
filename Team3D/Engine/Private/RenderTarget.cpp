#include "..\public\RenderTarget.h"
#include "VIBuffer_RectRHW.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTarget::NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width				= iWidth;
	TextureDesc.Height				= iHeight;
	TextureDesc.MipLevels			= 1;
	TextureDesc.ArraySize			= 1;
	TextureDesc.Format				= eFormat;
	TextureDesc.SampleDesc.Count	= 1;
	TextureDesc.SampleDesc.Quality	= 0;
	TextureDesc.Usage				= D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTargetTexture)))
	{
		MSG_BOX("Failed to Create Texture2D - CRenderTarget");
		return E_FAIL;
	}

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
	ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RenderTargetViewDesc.Format				= eFormat;
	RenderTargetViewDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	RenderTargetViewDesc.Texture2D.MipSlice = 0;

	if (m_pDevice->CreateRenderTargetView(m_pTargetTexture, &RenderTargetViewDesc, &m_pRenderTargetView))
	{
		MSG_BOX("Failed to Create RenderTargetView - CRenderTarget");
		return E_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	ShaderResourceViewDesc.Format				= eFormat;
	ShaderResourceViewDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels	= 1;

	if(FAILED(m_pDevice->CreateShaderResourceView(m_pTargetTexture, &ShaderResourceViewDesc, &m_pShaderResourceView)))
	{
		MSG_BOX("Failed to Create ShaderResourceView - CRenderTarget");
		return E_FAIL;
	}

	m_IsDepthStencil = isDepthBuffer;
	if (m_IsDepthStencil)
	{
		D3D11_TEXTURE2D_DESC		TextureDesc_Depth;
		TextureDesc_Depth = TextureDesc;
		TextureDesc_Depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		TextureDesc_Depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc_Depth, nullptr, &m_pDepthTargetTexture)))
		{
			MSG_BOX("Failed to Creating Texture2D For RenderTarget");
			return E_FAIL;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthTargetTexture, nullptr, &m_pDepthStencilView)))
			return E_FAIL;
	}

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Clear_View()
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pRenderTargetView, E_FAIL);

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget::Clear_Depth_Stencil_Buffer()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return S_OK;
}

CRenderTarget * CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pDevice_Context);

	if (FAILED(pInstance->NativeConstruct(iWidth, iHeight, eFormat, vClearColor)))
	{
		MSG_BOX("Failed to Create Instance - CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pDepthTargetTexture);
	Safe_Release(m_pDepthStencilView);

	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pTargetTexture);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, fX, fY, fSizeX, fSizeY, TEXT("../Bin/ShaderFiles/Shader_RenderTarget.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

HRESULT CRenderTarget::Render_DebugBuffer()
{
	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pShaderResourceView);
	m_pVIBuffer->Render(0);

	return S_OK;
}

void CRenderTarget::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer);
}
#endif