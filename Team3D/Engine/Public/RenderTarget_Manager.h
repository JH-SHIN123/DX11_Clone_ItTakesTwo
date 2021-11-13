#pragma once

#include "RenderTarget.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
public:
	explicit CRenderTarget_Manager() = default;
	virtual ~CRenderTarget_Manager() = default;

public: /* Getter */
	ID3D11ShaderResourceView* Get_ShaderResourceView(const _tchar* pRenderTargetTag);

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _tchar* pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer = false);
	HRESULT Add_MRT(const _tchar* pRenderTargetTag, const _tchar* pMRTTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pDevice_Context, const _tchar* pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pDevice_Context, const _tchar* pMRTTag);

private: /* Typedef */
	typedef unordered_map<const _tchar*, CRenderTarget*>			RENDER_TARGETS;
	typedef list<CRenderTarget*>									MULTIPLE_RENDER_TARGET;
	typedef unordered_map<const _tchar*, MULTIPLE_RENDER_TARGET>	MULTIPLE_RENDER_TARGETS;
private:
	RENDER_TARGETS				m_RenderTargets;
	MULTIPLE_RENDER_TARGETS		m_MRTs;
	ID3D11RenderTargetView*		m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
private:
	CRenderTarget*			Find_RenderTarget(const _tchar* pRenderTargetTag);
	list<CRenderTarget*>*	Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;

#ifdef _DEBUG
public:
	HRESULT Ready_DebugBuffer(const _tchar* pRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_DebugBuffer(const _tchar* pMRTTag);
	void	Clear_Buffers();
#endif
};

END