#include "..\public\RenderTarget_Manager.h"

IMPLEMENT_SINGLETON(CRenderTarget_Manager)

ID3D11ShaderResourceView * CRenderTarget_Manager::Get_ShaderResourceView(const _tchar * pRenderTargetTag)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, nullptr);

	return pRenderTarget->Get_ShaderResourceView();	
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _tchar * pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, _bool isDepthBuffer)
{
	NOT_NULL_CHECK_RETURN(Find_RenderTarget(pRenderTargetTag), E_FAIL);

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, eFormat, vClearColor, isDepthBuffer);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const _tchar * pRenderTargetTag, const _tchar * pMRTTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	MULTIPLE_RENDER_TARGET* pMRT = Find_MRT(pMRTTag);

	if (nullptr == pMRT)
	{
		MULTIPLE_RENDER_TARGET MRT;
		MRT.emplace_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRT);
	}
	else		
		pMRT->emplace_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag, _bool isClear)
{
	MULTIPLE_RENDER_TARGET* pMRT = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRT, E_FAIL);


	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);
	m_pBackBufferView->Release();
	m_pDepthStencilView->Release();

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr };
	ID3D11DepthStencilView* DepthStencil = nullptr;

	_uint iRenderTargetIndex = 0;

	for (auto& pRenderTarget : *pMRT)
	{
		if (true == isClear)
		{
			pRenderTarget->Clear_View();
			pRenderTarget->Clear_Depth_Stencil_Buffer();
		}

		RenderTargets[iRenderTargetIndex] = pRenderTarget->Get_RenderTargetView();
		DepthStencil = pRenderTarget->Get_DepthStencilView();
		++iRenderTargetIndex;
	}

	if (nullptr == DepthStencil)
		pDeviceContext->OMSetRenderTargets((_uint)pMRT->size(), RenderTargets, m_pDepthStencilView);
	else {
		ID3D11ShaderResourceView* pSRV[8] = { nullptr };
		pDeviceContext->PSSetShaderResources(0, 8, pSRV);
		pDeviceContext->OMSetRenderTargets((_uint)pMRT->size(), RenderTargets, DepthStencil);
	}
	
	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	// Clear Textures - Max RTV Count 8
	ID3D11ShaderResourceView* pSRV[8] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 8, pSRV);

	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	return S_OK;
}

CRenderTarget * CRenderTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CRenderTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_DebugBuffer(const _tchar * pRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	return pRenderTarget->Ready_DebugBuffer(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_DebugBuffer(const _tchar * pMRTTag)
{
	MULTIPLE_RENDER_TARGET* pMRT = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRT, E_FAIL);

	for (auto& pRenderTarget : *pMRT)
		FAILED_CHECK_RETURN(pRenderTarget->Render_DebugBuffer(), E_FAIL);

	return S_OK;
}

void CRenderTarget_Manager::Clear_Buffers()
{
	for (auto& Pair : m_RenderTargets)
		Pair.second->Clear_Buffer();
}
#endif