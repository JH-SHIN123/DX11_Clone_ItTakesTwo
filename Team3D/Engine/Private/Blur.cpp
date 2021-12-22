#include "Blur.h"
#include "RenderTarget_Manager.h"
#include "SSAO.h"
#include "VIBuffer_RectRHW.h"
#include "ShaderCompiler.h"

IMPLEMENT_SINGLETON(CBlur)

HRESULT CBlur::Ready_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(pDevice);
	Safe_AddRef(pDeviceContext);

	m_iWinSize[0] = (_uint)fBufferWidth;
	m_iWinSize[1] = (_uint)fBufferHeight;
	m_iDownScaleGroups = (_uint)ceil((_float)(fBufferWidth * fBufferHeight / 16) / 1024.0f);

	FAILED_CHECK_RETURN(Build_BlurResources(fBufferWidth, fBufferHeight),E_FAIL);
	FAILED_CHECK_RETURN(Build_ComputeShaders(TEXT("../Bin/ShaderFiles/ComputeShader_Blur.hlsl"), "DefaultTechnique"), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Blur_Emissive()
{
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(DownScale(pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Emissive")),m_pUnorderedAccessView_DownScaledEmissive), E_FAIL);
	FAILED_CHECK_RETURN(BlurInPlace(m_pShaderResourceView_DownScaledEmissive, m_pUnorderedAccessView_DownScaledEmissive), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Blur_Effect()
{
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(DownScale(pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Effect")), m_pUnorderedAccessView_DownScaledEffect), E_FAIL);
	FAILED_CHECK_RETURN(BlurInPlace(m_pShaderResourceView_DownScaledEffect, m_pUnorderedAccessView_DownScaledEffect), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Blur_Specular()
{
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(DownScale(pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Specular")), m_pUnorderedAccessView_DownScaledSpecular), E_FAIL);
	FAILED_CHECK_RETURN(BlurInPlace(m_pShaderResourceView_DownScaledSpecular, m_pUnorderedAccessView_DownScaledSpecular), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Blur_Effect_Pre_CustomBlur()
{
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(DownScale(pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Effect_Pre_Custom_Blur")), m_pUnorderedAccessView_DownScaledEffectPreCustomBlur), E_FAIL);
	FAILED_CHECK_RETURN(BlurInPlace(m_pShaderResourceView_DownScaledEffectPreCustomBlur, m_pUnorderedAccessView_DownScaledEffectPreCustomBlur), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Blur_Effect_Post_CustomBlur()
{
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(DownScale(pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Effect_Post_Custom_Blur")), m_pUnorderedAccessView_DownScaledEffectPostCustomBlur), E_FAIL);
	FAILED_CHECK_RETURN(BlurInPlace(m_pShaderResourceView_DownScaledEffectPostCustomBlur, m_pUnorderedAccessView_DownScaledEffectPostCustomBlur), E_FAIL);

	return S_OK;
}

HRESULT CBlur::DownScale(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_InputTex", inputSRV), E_FAIL);
	
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_OutputDownScaleTex", inputUAV), E_FAIL); // HDR 다운스케일 텍스쳐 제작

	FAILED_CHECK_RETURN(m_InputLayouts_CS[0].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->Dispatch(m_iDownScaleGroups, 1, 1);
	Unbind_ShaderResources(); // Reset Views

	return S_OK;
}

HRESULT CBlur::BlurInPlace(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	// Horizontal
/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", inputSRV), E_FAIL);
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", m_pUnorderedAccessView_Blur_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	_uint x = (_uint)ceil((m_iWinSize[0] / 4.0f) / (128.0f - 12.0f));
	_uint y = (_uint)ceil(m_iWinSize[1] / 4.0f);
	m_pDeviceContext->Dispatch(x, y, 1);
	Unbind_ShaderResources();

	// Vertical
	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", m_pShaderResourceView_Blur_Temp), E_FAIL);
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", inputUAV), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[2].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	x = (_uint)ceil(m_iWinSize[0] / 4.0f);
	y = (_uint)ceil((m_iWinSize[1] / 4.0f) / (128.0f - 12.0f));
	m_pDeviceContext->Dispatch(x, y, 1);
	Unbind_ShaderResources();

	return S_OK;
}

HRESULT CBlur::Build_BlurResources(_float iWidth, _float iHeight)
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = (_uint)iWidth / 4;
	TextureDesc.Height = (_uint)iHeight / 4;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBlurTex_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEmissiveTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEffectTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledSpecularTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEffectPreCustomBlurTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEffectPostCustomBlurTex), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pBlurTex_Temp, &ShaderResourceViewDesc, &m_pShaderResourceView_Blur_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEmissiveTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEmissive), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEffectTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEffect), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledSpecularTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledSpecular), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEffectPreCustomBlurTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEffectPreCustomBlur), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEffectPostCustomBlurTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEffectPostCustomBlur), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
	ZeroMemory(&UnorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UnorderedAccessViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UnorderedAccessViewDesc.Buffer.FirstElement = 0;
	UnorderedAccessViewDesc.Buffer.NumElements = m_iWinSize[0] * m_iWinSize[1] / 16;

	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pBlurTex_Temp, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Blur_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEmissiveTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEmissive), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEffectTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEffect), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledSpecularTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledSpecular), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEffectPreCustomBlurTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEffectPreCustomBlur), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEffectPostCustomBlurTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEffectPostCustomBlur), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	ID3DBlob* pCompiledShaderCode = CShaderCompiler::GetInstance()->Get_CompiledCode(pShaderFilePath);
	FAILED_CHECK_RETURN(D3DX11CreateEffectFromMemory(pCompiledShaderCode->GetBufferPointer(), pCompiledShaderCode->GetBufferSize(), 0, m_pDevice, &m_pEffect_CS), E_FAIL);

	ID3DX11EffectTechnique* pTechnique = m_pEffect_CS->GetTechniqueByName(pTechniqueName);
	NULL_CHECK_RETURN(pTechnique, E_FAIL);

	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	FAILED_CHECK_RETURN(pTechnique->GetDesc(&TechniqueDesc), E_FAIL);

	m_InputLayouts_CS.reserve(TechniqueDesc.Passes);

	for (_uint iPassIndex = 0; iPassIndex < TechniqueDesc.Passes; ++iPassIndex)
	{
		INPUT_LAYOUT_DESC	InputLayoutDesc;

		InputLayoutDesc.pLayout = nullptr;
		InputLayoutDesc.pPass = pTechnique->GetPassByIndex(iPassIndex);

		m_InputLayouts_CS.emplace_back(InputLayoutDesc);
	}

	Safe_Release(pTechnique);

	return S_OK;
}

HRESULT CBlur::Set_Variable(const char* pConstantName, void* pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectVariable* pVariable = m_pEffect_CS->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CBlur::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pSrvVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pSrvVariable, E_FAIL);

	return pSrvVariable->SetResource(pResourceView);
}

HRESULT CBlur::Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUavVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	NULL_CHECK_RETURN(pUavVariable, E_FAIL);
	return pUavVariable->SetUnorderedAccessView(pResourceView);
}

HRESULT CBlur::Unbind_ShaderResources()
{
	ID3D11ShaderResourceView* pNullSRV[8] = { 0 };
	ID3D11UnorderedAccessView* pNullUAV[8] = { 0 };

	m_pDeviceContext->CSSetShaderResources(0, 8, pNullSRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, pNullUAV, 0);

	//m_pDeviceContext->CSSetShader(0, 0, 0);

	return S_OK;
}

void CBlur::Free()
{
	Safe_Release(m_pUnorderedAccessView_DownScaledEffectPostCustomBlur);
	Safe_Release(m_pShaderResourceView_DownScaledEffectPostCustomBlur);
	Safe_Release(m_pDownScaledEffectPostCustomBlurTex);

	Safe_Release(m_pUnorderedAccessView_DownScaledEffectPreCustomBlur);
	Safe_Release(m_pShaderResourceView_DownScaledEffectPreCustomBlur);
	Safe_Release(m_pDownScaledEffectPreCustomBlurTex);

	Safe_Release(m_pUnorderedAccessView_Blur_Temp);
	Safe_Release(m_pShaderResourceView_Blur_Temp);
	Safe_Release(m_pBlurTex_Temp);

	Safe_Release(m_pShaderResourceView_DownScaledEmissive);
	Safe_Release(m_pUnorderedAccessView_DownScaledEmissive);
	Safe_Release(m_pDownScaledEmissiveTex);

	Safe_Release(m_pShaderResourceView_DownScaledEffect);
	Safe_Release(m_pUnorderedAccessView_DownScaledEffect);
	Safe_Release(m_pDownScaledEffectTex);

	Safe_Release(m_pUnorderedAccessView_DownScaledSpecular);
	Safe_Release(m_pShaderResourceView_DownScaledSpecular);
	Safe_Release(m_pDownScaledSpecularTex);

	for (auto& InputLayout : m_InputLayouts_CS)
	{
		Safe_Release(InputLayout.pPass);
		Safe_Release(InputLayout.pLayout);
	}
	m_InputLayouts_CS.clear();
	Safe_Release(m_pEffect_CS);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	Clear_Buffer();
#endif
}

#ifdef _DEBUG
HRESULT CBlur::Ready_DebugBuffer(const _tchar* pTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	auto& iter = find_if(m_VIBuffers.begin(), m_VIBuffers.end(), CTagFinder(pTag));
	if (iter != m_VIBuffers.end()) return E_FAIL;

	CVIBuffer_RectRHW* pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, fX, fY, fSizeX, fSizeY, TEXT("../Bin/ShaderFiles/Shader_RenderTarget.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(pVIBuffer, E_FAIL);
	m_VIBuffers.emplace(pTag, pVIBuffer);

	return S_OK;
}

HRESULT CBlur::Render_DebugBuffer_Emissive(const _tchar* pTag)
{
	auto& iter = find_if(m_VIBuffers.begin(), m_VIBuffers.end(), CTagFinder(pTag));
	if (iter == m_VIBuffers.end()) return E_FAIL;

	(*iter).second->Set_ShaderResourceView("g_DiffuseTexture", m_pShaderResourceView_DownScaledEmissive);
	(*iter).second->Render(0);
	return S_OK;
}

HRESULT CBlur::Render_DebugBuffer_Effect(const _tchar* pTag)
{
	auto& iter = find_if(m_VIBuffers.begin(), m_VIBuffers.end(), CTagFinder(pTag));
	if (iter == m_VIBuffers.end()) return E_FAIL;

	(*iter).second->Set_ShaderResourceView("g_DiffuseTexture", m_pShaderResourceView_DownScaledEffect);
	(*iter).second->Render(0);
	return S_OK;
}

HRESULT CBlur::Render_DebugBuffer_Specular(const _tchar* pTag)
{
	auto& iter = find_if(m_VIBuffers.begin(), m_VIBuffers.end(), CTagFinder(pTag));
	if (iter == m_VIBuffers.end()) return E_FAIL;

	(*iter).second->Set_ShaderResourceView("g_DiffuseTexture", m_pShaderResourceView_DownScaledSpecular);
	(*iter).second->Render(0);
	return S_OK;
}

void CBlur::Clear_Buffer()
{
	for (auto& Pair : m_VIBuffers)
	{
		Safe_Release(Pair.second);
	}
	m_VIBuffers.clear();
}
#endif