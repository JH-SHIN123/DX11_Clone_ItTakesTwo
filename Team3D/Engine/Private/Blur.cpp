#include "Blur.h"
#include "RenderTarget_Manager.h"

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
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", m_pUnorderedAccessView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	_uint x = (_uint)ceil((m_iWinSize[0] / 4.0f) / (128.0f - 12.0f));
	_uint y = (_uint)ceil(m_iWinSize[1] / 4.0f);
	m_pDeviceContext->Dispatch(x, y, 1);
	Unbind_ShaderResources();

	// Vertical
	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", m_pShaderResourceView_Bloom_Temp), E_FAIL);
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

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEmissiveTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledEffectTex), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pBloomTex_Temp, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEmissiveTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEmissive), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledEffectTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledEffect), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
	ZeroMemory(&UnorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UnorderedAccessViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UnorderedAccessViewDesc.Buffer.FirstElement = 0;
	UnorderedAccessViewDesc.Buffer.NumElements = m_iWinSize[0] * m_iWinSize[1] / 16;

	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pBloomTex_Temp, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEmissiveTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEmissive), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledEffectTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledEffect), E_FAIL);

	return S_OK;
}

HRESULT CBlur::Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pCompiledShaderCode = nullptr;
	ID3DBlob* pCompileErrorMsg = nullptr;

	FAILED_CHECK_RETURN(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCompiledShaderCode, &pCompileErrorMsg), E_FAIL);
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
	Safe_Release(pCompiledShaderCode);
	Safe_Release(pCompileErrorMsg);

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
	Safe_Release(m_pUnorderedAccessView_Bloom_Temp);
	Safe_Release(m_pShaderResourceView_Bloom_Temp);
	Safe_Release(m_pBloomTex_Temp);

	Safe_Release(m_pShaderResourceView_DownScaledEmissive);
	Safe_Release(m_pUnorderedAccessView_DownScaledEmissive);
	Safe_Release(m_pDownScaledEmissiveTex);

	Safe_Release(m_pShaderResourceView_DownScaledEffect);
	Safe_Release(m_pUnorderedAccessView_DownScaledEffect);
	Safe_Release(m_pDownScaledEffectTex);

	for (auto& InputLayout : m_InputLayouts_CS)
	{
		Safe_Release(InputLayout.pPass);
		Safe_Release(InputLayout.pLayout);
	}
	m_InputLayouts_CS.clear();
	Safe_Release(m_pEffect_CS);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
