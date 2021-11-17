#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

public: /* Getter */
	ID3D11RenderTargetView*		Get_RenderTargetView() const { return m_pRenderTargetView; }
	ID3D11ShaderResourceView*	Get_ShaderResourceView() const { return m_pShaderResourceView; }
	ID3D11DepthStencilView*		Get_DepthStencilView() const { return m_pDepthStencilView; }

public:
	HRESULT NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer = false);
	HRESULT Clear_View();
	HRESULT Clear_Depth_Stencil_Buffer();

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	ID3D11Texture2D*			m_pTargetTexture = nullptr;
	ID3D11RenderTargetView*		m_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView = nullptr;
	
private:
	_bool						m_IsDepthStencil = false;
	ID3D11Texture2D*			m_pDepthTargetTexture = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

private:
	_float4						m_vClearColor;

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer);
	virtual void Free() override;

#ifdef _DEBUG
public:
	HRESULT Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_DebugBuffer();
	void	Clear_Buffer();
private:
	class CVIBuffer_RectRHW* m_pVIBuffer = nullptr;
#endif

};

END