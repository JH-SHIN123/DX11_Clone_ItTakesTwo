#include "..\public\Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

const D3D11_VIEWPORT CGraphic_Device::Get_ViewportInfo(_uint iViewportIndex) const
{
	NULL_CHECK_RETURN(iViewportIndex < VP_END, D3D11_VIEWPORT());

	return m_Viewports[iViewportIndex];
}

const _float4 CGraphic_Device::Get_ViewportUVInfo(_uint iViewportIndex) const
{
	NULL_CHECK_RETURN(iViewportIndex < VP_END, _float4());

	_float fLeft	= m_vViewportInfo[iViewportIndex].x;
	_float fTop		= m_vViewportInfo[iViewportIndex].y;
	_float fRight	= m_vViewportInfo[iViewportIndex].x + m_vViewportInfo[iViewportIndex].z;
	_float fBottom	= m_vViewportInfo[iViewportIndex].y + m_vViewportInfo[iViewportIndex].w;

	return _float4(fLeft, fTop, fRight, fBottom);
}

const _float CGraphic_Device::Get_ViewportAspect(_uint iViewportIndex) const
{
	if (0.f >= m_Viewports[iViewportIndex].Height)
		return 1.f;

	_float fAspect = m_Viewports[iViewportIndex].Width / m_Viewports[iViewportIndex].Height;

	if (0.f >= fAspect)
		return 1.f;

	return fAspect;
}

void CGraphic_Device::Set_ViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo)
{
	m_fWeight				= 1.f;
	m_isViewportChanging	= false;

	XMStoreFloat4(&m_vGoalInfo[VP_MAIN], vMainViewportInfo);
	XMStoreFloat4(&m_vGoalInfo[VP_SUB], vSubViewportInfo);

	memcpy(&m_vViewportInfo[VP_MAIN], &m_vGoalInfo[VP_MAIN], sizeof(_float4));
	memcpy(&m_vViewportInfo[VP_SUB], &m_vGoalInfo[VP_SUB], sizeof(_float4));

	Set_Viewport();
}

void CGraphic_Device::Set_GoalViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo, _float fLerpSpeed)
{
	if (fLerpSpeed <= 0.f)
	{
		MSG_BOX("Wrong LerpSpeed - CGraphic_Device");
		return;
	} 

	m_fWeight				= 0.f;
	m_fLerpSpeed			= fLerpSpeed;
	m_isViewportChanging	= true;

	XMStoreFloat4(&m_vGoalInfo[VP_MAIN], vMainViewportInfo);
	XMStoreFloat4(&m_vGoalInfo[VP_SUB], vSubViewportInfo);

	memcpy(&m_vStartInfo[VP_MAIN], &m_vViewportInfo[VP_MAIN], sizeof(_float4));
	memcpy(&m_vStartInfo[VP_SUB], &m_vViewportInfo[VP_SUB], sizeof(_float4));
}

HRESULT CGraphic_Device::Ready_GraphicDevice(WINMODE eWinMode, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
	m_fWinSizeX = (_float)iWinSizeX;
	m_fWinSizeY = (_float)iWinSizeY;

	m_vViewportInfo[VP_FULL] = _float4(0.f, 0.f, 1.f, 1.f);

	_uint	iCreateFlag = 0;

#ifdef _DEBUG
	iCreateFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	D3D_FEATURE_LEVEL FeatureLevel;

	Microsoft::WRL::ComPtr<IDXGIFactory> mdxgiFactory;
	FAILED_CHECK_RETURN(CreateDXGIFactory(IID_PPV_ARGS(&mdxgiFactory)), E_FAIL);

	std::size_t ui64VideoMemory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
	DXGI_ADAPTER_DESC adapterDesc;

	// 그래픽 카드 어뎁터 체크 
	// 요청한 그래픽 카드 인터페이스에 대한 어뎁터가 없습니다.
	FAILED_CHECK_RETURN(mdxgiFactory->EnumAdapters(0, (IDXGIAdapter**)&pAdapter), E_FAIL);
	FAILED_CHECK_RETURN(pAdapter->GetDesc(&adapterDesc), E_FAIL);

	ui64VideoMemory = (std::size_t)(adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory);

	// Compare Video Memory and Find better Gpu
	int gpu_idx = 0;
	int select = 0;
	std::size_t comparison_videoMemory = 0;

	while (mdxgiFactory->EnumAdapters(gpu_idx, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		pAdapter->GetDesc(&adapterDesc);
		comparison_videoMemory = adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory;

		if (comparison_videoMemory > ui64VideoMemory)
		{
			ui64VideoMemory = comparison_videoMemory;
			select = gpu_idx;
		}
		++gpu_idx;
	}

	FAILED_CHECK_RETURN(mdxgiFactory->EnumAdapters(select, &pAdapter), E_FAIL);

	FAILED_CHECK_RETURN(D3D11CreateDevice(pAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, iCreateFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext), E_FAIL);
	FAILED_CHECK_RETURN(Ready_SwapChain(eWinMode, hWnd, iWinSizeX, iWinSizeY), E_FAIL);
	FAILED_CHECK_RETURN(Ready_BackBufferRenderTargetView(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DepthStencilRenderTargetView(iWinSizeX, iWinSizeY), E_FAIL);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
	Set_Viewport();

	if (nullptr != ppDevice)
	{
		*ppDevice = m_pDevice;
		Safe_AddRef(m_pDevice);
	}

	if (nullptr != ppDeviceContext)
	{
		*ppDeviceContext = m_pDeviceContext;
		Safe_AddRef(m_pDeviceContext);
	}

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer(_float4 vClearColor)
{
	NULL_CHECK_RETURN(m_pBackBufferView, E_FAIL);
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferView, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencilBuffer()
{
	NULL_CHECK_RETURN(m_pDepthStencilView, E_FAIL);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	NULL_CHECK_RETURN(m_pSwapChain, E_FAIL);
	m_pSwapChain->Present(0, 0);

	return S_OK;
}

void CGraphic_Device::Tick(_double dTimeDelta)
{
	if (m_fWeight < 1.f)
	{
		XMStoreFloat4(&m_vViewportInfo[VP_MAIN], XMVectorLerp(XMLoadFloat4(&m_vStartInfo[VP_MAIN]), XMLoadFloat4(&m_vGoalInfo[VP_MAIN]), m_fWeight));
		XMStoreFloat4(&m_vViewportInfo[VP_SUB], XMVectorLerp(XMLoadFloat4(&m_vStartInfo[VP_SUB]), XMLoadFloat4(&m_vGoalInfo[VP_SUB]), m_fWeight));

		m_fWeight += (_float)dTimeDelta * m_fLerpSpeed;

		Set_Viewport();
	}
	else if (true == m_isViewportChanging)
	{
		m_isViewportChanging = false;
		memcpy(&m_vViewportInfo[VP_MAIN], &m_vGoalInfo[VP_MAIN], sizeof(_float4));
		memcpy(&m_vViewportInfo[VP_SUB], &m_vGoalInfo[VP_SUB], sizeof(_float4));

		Set_Viewport();
	}
}

HRESULT CGraphic_Device::Ready_SwapChain(WINMODE eWinMode, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	IDXGIDevice*	pDevice		= nullptr;
	IDXGIAdapter*	pAdapter	= nullptr;
	IDXGIFactory*	pFactory	= nullptr;

	FAILED_CHECK_RETURN(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice), E_FAIL);
	FAILED_CHECK_RETURN(pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter), E_FAIL);
	FAILED_CHECK_RETURN(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory), E_FAIL);

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width						= iWinSizeX;
	SwapChainDesc.BufferDesc.Height						= iWinSizeY;
	SwapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator	= 60;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator		= 1;
	SwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferCount							= 1;
	SwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow							= hWnd;
	SwapChainDesc.Windowed								= eWinMode;
	SwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.SampleDesc.Count						= 1;
	SwapChainDesc.SampleDesc.Quality					= 0;
	SwapChainDesc.Flags									= 0;

	FAILED_CHECK_RETURN(pFactory->CreateSwapChain(m_pDevice, &SwapChainDesc, &m_pSwapChain), E_FAIL);

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	NULL_CHECK_RETURN(m_pSwapChain, E_FAIL);

	ID3D11Texture2D* pBackBuffer = nullptr;

	FAILED_CHECK_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferView), E_FAIL);

	Safe_Release(pBackBuffer);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinSizeX, _uint iWinSizeY)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.Width				= iWinSizeX;
	TextureDesc.Height				= iWinSizeY;
	TextureDesc.MipLevels			= 1;
	TextureDesc.ArraySize			= 1;
	TextureDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Count	= 1;
	TextureDesc.SampleDesc.Quality	= 0;
	TextureDesc.Usage				= D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags		= 0;
	TextureDesc.MiscFlags			= 0;

	ID3D11Texture2D* pDepthStencil = nullptr;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencil), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateDepthStencilView(pDepthStencil, nullptr, &m_pDepthStencilView), E_FAIL);

	Safe_Release(pDepthStencil);

	return S_OK;
}

void CGraphic_Device::Set_Viewport()
{
	m_Viewports[VP_FULL].TopLeftX	= 0.f;
	m_Viewports[VP_FULL].TopLeftY	= 0.f;
	m_Viewports[VP_FULL].Width		= m_fWinSizeX;
	m_Viewports[VP_FULL].Height		= m_fWinSizeY;
	m_Viewports[VP_FULL].MinDepth	= 0.f;
	m_Viewports[VP_FULL].MaxDepth	= 1.f;

	m_Viewports[VP_MAIN].TopLeftX	= m_vViewportInfo[VP_MAIN].x * m_fWinSizeX;
	m_Viewports[VP_MAIN].TopLeftY	= m_vViewportInfo[VP_MAIN].y * m_fWinSizeY;
	m_Viewports[VP_MAIN].Width		= m_vViewportInfo[VP_MAIN].z * m_fWinSizeX;
	m_Viewports[VP_MAIN].Height		= m_vViewportInfo[VP_MAIN].w * m_fWinSizeY;
	m_Viewports[VP_MAIN].MinDepth	= 0.f;
	m_Viewports[VP_MAIN].MaxDepth	= 1.f;

	m_Viewports[VP_SUB].TopLeftX	= m_vViewportInfo[VP_SUB].x * m_fWinSizeX;
	m_Viewports[VP_SUB].TopLeftY	= m_vViewportInfo[VP_SUB].y * m_fWinSizeY;
	m_Viewports[VP_SUB].Width		= m_vViewportInfo[VP_SUB].z * m_fWinSizeX;
	m_Viewports[VP_SUB].Height		= m_vViewportInfo[VP_SUB].w * m_fWinSizeY;
	m_Viewports[VP_SUB].MinDepth	= 0.f;
	m_Viewports[VP_SUB].MaxDepth	= 1.f;

	m_pDeviceContext->RSSetViewports(3, m_Viewports);
}

void CGraphic_Device::Free()
{
	m_pSwapChain->SetFullscreenState(false, nullptr);

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}