#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
private:
	explicit CGraphic_Device() = default;
	virtual ~CGraphic_Device() = default;

public: /* Enum */
	enum WINMODE		{ TYPE_FULLMODE, TYPE_WINMODE, TYPE_END };
	enum VIEWPORT_TYPE	{ VP_FULL, VP_MAIN, VP_SUB, VP_END };

public: /* Getter */
	const D3D11_VIEWPORT			Get_ViewportInfo(_uint iViewportIndex) const;
	const _float4					Get_ViewportUVInfo(_uint iViewportIndex) const;
	const _float4					Get_ViewportRadioInfo(_uint iViewportIndex) const;
	const _float					Get_ViewportAspect(_uint iViewportIndex) const;

public: /* Setter */
	void Set_ViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo); /* 뷰포트 사이즈 바로 세팅 */
	void Set_GoalViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo, _float fLerpSpeed); /* 뷰포트 사이즈 천천히 변환 */
	void Set_Viewport();

public:
	HRESULT	Ready_GraphicDevice(WINMODE eWinMode, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	HRESULT	Clear_BackBuffer(_float4 vClearColor);
	HRESULT	Clear_DepthStencilBuffer();
	HRESULT	Present();
	void	Tick(_double dTimeDelta);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	IDXGISwapChain*				m_pSwapChain = nullptr;
	ID3D11RenderTargetView*		m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	DXGI_FORMAT					m_eBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	/* For.DevideViewport */
	D3D11_VIEWPORT	m_Viewports[VP_END];
	_float4			m_vViewportInfo[VP_END];	/* x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	_float			m_fWinSizeX = 0.f;
	_float			m_fWinSizeY = 0.f;
	_float			m_fWeight = 0.f;		/* 변환 가중치 */
	_float			m_fLerpSpeed = 0.f;		/* 변환 속도 */
	_float4			m_vStartInfo[VP_END];	/* 보간용 */
	_float4			m_vGoalInfo[VP_END];	/* 보간용 */
	_bool			m_isViewportChanging = false;
private:
	HRESULT Ready_SwapChain(WINMODE eWinMode, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinSizeX, _uint iWinSizeY);

public:
	virtual void Free() override;
};

END

/*
	// 뷰포트 크기 조절 방법
	Set_ViewportInfo > 뷰포트 크기를 바로 세팅
	Set_GoalViewportInfo > 뷰포트 크기를 천천히 세팅, 스피드 조절 가능

	첫번째 인자는 세팅할 메인뷰포트 정보(x = TopLeftX, y = TopLeftY, z = Width, w = Height)
	두번째 인자는 세팅할 서브뷰포트 정보
	정보 벡터의 각 요소들의 범위는 0.f ~ 1.f.
*/