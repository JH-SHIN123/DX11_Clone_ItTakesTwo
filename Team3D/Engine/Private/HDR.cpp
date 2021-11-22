#include "..\Public\HDR.h"
#include "RenderTarget_Manager.h"
#include "VIBuffer_RectRHW.h"

IMPLEMENT_SINGLETON(CHDR)

HRESULT CHDR::Ready_HDR(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(pDevice);
	Safe_AddRef(pDeviceContext);

	m_iWinSize[0] = (_uint)fBufferWidth;
	m_iWinSize[1] = (_uint)fBufferHeight;

	m_pVIBuffer_ToneMapping = CVIBuffer_RectRHW::Create(pDevice, pDeviceContext, 0.f, 0.f, fBufferWidth, fBufferHeight, TEXT("../Bin/ShaderFiles/Shader_HDR.hlsl"), "DefaultTechnique");

	FAILED_CHECK_RETURN(Build_FirstPassResources(fBufferWidth, fBufferHeight),E_FAIL);
	FAILED_CHECK_RETURN(Build_SecondPassResources(),E_FAIL);
	FAILED_CHECK_RETURN(Build_PrevLumAvgResources(), E_FAIL);
	FAILED_CHECK_RETURN(Build_BloomResources(fBufferWidth, fBufferHeight), E_FAIL);
	FAILED_CHECK_RETURN(Build_ComputeShaders(TEXT("../Bin/ShaderFiles/ComputeShader_HDR.hlsl"), "DefaultTechnique"), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Render_HDR(_double TimeDelta)
{
	FAILED_CHECK_RETURN(Calculate_LuminanceAvg(TimeDelta), E_FAIL);
	FAILED_CHECK_RETURN(Calculate_BrightPassForBloom(), E_FAIL);

	// PS ---------------------------------------------------------------------------
	// Tone Mapping
	NULL_CHECK_RETURN(m_pVIBuffer_ToneMapping, E_FAIL);
	
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_fMiddleGrey += 0.0005f;
	else if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_fMiddleGrey -= 0.0005f;

	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_fLumWhiteSqr += 0.05f;
	else if (GetAsyncKeyState(VK_F4) & 0x8000)
		m_fLumWhiteSqr -= 0.05f;

	_float fLumWhiteSqrt = powf(sqrtf(m_fLumWhiteSqr),2);

	m_pVIBuffer_ToneMapping->Set_Variable("g_MiddleGrey", &m_fMiddleGrey, sizeof(_float));
	m_pVIBuffer_ToneMapping->Set_Variable("g_LumWhiteSqr", &fLumWhiteSqrt, sizeof(_float));

	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_HDR")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_BloomTexture", m_pShaderResourceView_Bloom); // -> 이거생기니까 마크생김
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_AverageLum", m_pShaderResourceView_LumAve);

	m_pVIBuffer_ToneMapping->Render(0);

	// Swap Cur LumAvg - Prev LumAvg
	// 두 버퍼의 값을 교체하고, 현재 프레임의 평균 휘도를 다음프레임의 "prevLumAvg"로 저장한다.
	swap(m_pHDRBuffer_PrevLumAve, m_pHDRBuffer_LumAve);
	swap(m_pUnorderedAccessView_PrevLumAve, m_pUnorderedAccessView_LumAve);
	swap(m_pShaderResourceView_PrevLumAve, m_pShaderResourceView_LumAve);

	return S_OK;
}

HRESULT CHDR::Calculate_LuminanceAvg(_double TimeDelta)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);
	
	// Set Adaptation
	//m_fAdaptationDeltaT += (_float)TimeDelta;

	//_float fAdaptNorm = 0.f;
	//if (m_bAdaptationFirstTime)
	//{
	//	fAdaptNorm = 0.f;
	//	m_bAdaptationFirstTime = false;
	//}
	//else
	//{
	//	if(m_fAdaptationDeltaT > m_fAdapt)
	//	{
	//		m_fAdaptationDeltaT -= m_fAdapt;
	//	}

	//	fAdaptNorm = min(m_fAdapt < 0.0001f ? m_fAdapt : (m_fAdaptationDeltaT / m_fAdapt), m_fAdapt - 0.0001f);
	//}

	//m_fAdaptation = fAdaptNorm;

	// CS ---------------------------------------------------------------------------
	// For. First Pass
	// Set HDR Texture
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_HDR"))),E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_HDRDownScale", m_pUnorderedAccessView_DownScaledHDR), E_FAIL); // HDR 다운스케일 텍스쳐 제작
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageValues1D", m_pShaderResourceView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_Lum), E_FAIL);

	FAILED_CHECK_RETURN(m_InputLayouts_CS[0].pPass->Apply(0, m_pDeviceContext),E_FAIL);

	 _uint x = (_uint)(ceil((m_iWinSize[0] / 1024.f) * (m_iWinSize[1] / 16.f)));
	m_pDeviceContext->Dispatch(x, 1,1); 

	// Reset Views
	Unbind_ShaderResources();

	// For. Second Pass
	//FAILED_CHECK_RETURN(Set_Variable("g_Adaptation", &m_fAdaptation, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageValues1D", m_pShaderResourceView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_LumAve), E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_PrevAverageLum", m_pShaderResourceView_PrevLumAve), E_FAIL);

	// MAX_GROUPS : 64
	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->Dispatch(1, 1, 1);

	// Reset Views
	Unbind_ShaderResources();

	return S_OK;
}

HRESULT CHDR::Calculate_BrightPassForBloom()
{
	// Downscale & BrightRight
	// 다운스케일된 HDR과 SRV의 평균휘도를 계산한후, 임시로 UAV(m_pUnorderedAccessView_Bloom_Temp)로 저장한다.
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_HDRDownScaleTex", m_pShaderResourceView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_LumAve), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Bloom", m_pUnorderedAccessView_Bloom_Temp), E_FAIL);

	FAILED_CHECK_RETURN(m_InputLayouts_CS[2].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->Dispatch(1, 1, 1);

	Unbind_ShaderResources();

	//> 2 pass - verticla & horizontal
	// 수직 / 수평 가우시안 블러
	// 첫 패스에서 휘도 다운스케일에 쓰인 그룹숫자만큼의 컴퓨트셰이더를 적용한다.
	// 0 pass : 첫째 임시 텍스쳐 SRV(m_pShaderResourceView_Bloom_Temp)로부터 픽셀을 수평방향으로 필터링해 둘째 임시 텍스쳐 UAV(m_pUnorderedAccessView_Bloom)로 저장한다.

	// Vertical
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", m_pShaderResourceView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", m_pUnorderedAccessView_Bloom), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[3].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	_uint x = (_uint)(ceil(m_iWinSize[0] / 4.f));
	_uint y = (_uint)(ceil((m_iWinSize[1] / 4.f) / ((128 - 12) + 1)));

	m_pDeviceContext->Dispatch(x, y, 1);

	Unbind_ShaderResources();

	// Horizontal
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", m_pShaderResourceView_Bloom), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", m_pUnorderedAccessView_Bloom), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[4].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	// GroupThread : 128
	x = (_uint)(ceil(m_iWinSize[1] / 4.f) / ((128 - 12) + 1));
	y = (_uint)(ceil(m_iWinSize[1] / 4.f));
	m_pDeviceContext->Dispatch(x, y, 1);

	// Reset Views
	Unbind_ShaderResources();

	return S_OK;
}

void CHDR::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer_ToneMapping);
}

HRESULT CHDR::Unbind_ShaderResources()
{
	ID3D11ShaderResourceView* pNullSRV[8] = { 0 };
	ID3D11UnorderedAccessView* pNullUAV[8] = { 0 };

	m_pDeviceContext->CSSetShaderResources(0, 8, pNullSRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, pNullUAV, 0);

	m_pDeviceContext->CSSetShader(0, 0, 0);

	return S_OK;
}

HRESULT CHDR::Build_FirstPassResources(_float iWidth, _float iHeight)
{
	/* 휘도의 중간값을 저장하기 위한 리소스들 */
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.StructureByteStride = sizeof(float);
	BufferDesc.ByteWidth = (_uint)(4 * iWidth * iHeight / 16 * 1024);
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_Lum), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = (_uint)(iWidth * iHeight / 16 * 1024);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_Lum, &UAVDesc, &m_pUnorderedAccessView_Lum), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = (_uint)(iWidth * iHeight / 16 * 1024);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_Lum, &SRVDesc, &m_pShaderResourceView_Lum), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Build_SecondPassResources()
{
	/* 부동소수점 형태로 평균 휘도 값을 저장 */
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.StructureByteStride = sizeof(float);
	BufferDesc.ByteWidth = 4;
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_LumAve), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_LumAve, &UAVDesc, &m_pUnorderedAccessView_LumAve), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_LumAve, &SRVDesc, &m_pShaderResourceView_LumAve), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Build_PrevLumAvgResources()
{
	/* 부동소수점 형태로 평균 휘도 값을 저장 */
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.StructureByteStride = sizeof(float);
	BufferDesc.ByteWidth = 4;
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_PrevLumAve), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_PrevLumAve, &UAVDesc, &m_pUnorderedAccessView_PrevLumAve), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_PrevLumAve, &SRVDesc, &m_pShaderResourceView_PrevLumAve), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Build_BloomResources(_float iWidth, _float iHeight)
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = (_uint)iWidth / 4;
	TextureDesc.Height = (_uint)iHeight / 4;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledHDRTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex), E_FAIL);


	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledHDRTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledHDRTex, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledHDRTex, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
	ZeroMemory(&UnorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UnorderedAccessViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledHDRTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledHDRTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledHDRTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pCompiledShaderCode = nullptr;
	ID3DBlob* pCompileErrorMsg = nullptr;

	FAILED_CHECK_RETURN(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,  nullptr, "fx_5_0", iFlag, 0, &pCompiledShaderCode, &pCompileErrorMsg), E_FAIL);
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

HRESULT CHDR::Set_Variable(const char* pConstantName, void* pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectVariable* pVariable = m_pEffect_CS->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CHDR::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pSrvVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pSrvVariable, E_FAIL);

	return pSrvVariable->SetResource(pResourceView);
}

HRESULT CHDR::Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUavVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	NULL_CHECK_RETURN(pUavVariable, E_FAIL);
	return pUavVariable->SetUnorderedAccessView(pResourceView);
}

void CHDR::Free()
{
	Safe_Release(m_pShaderResourceView_DownScaledHDR);
	Safe_Release(m_pUnorderedAccessView_DownScaledHDR);
	Safe_Release(m_pDownScaledHDRTex);

	Safe_Release(m_pUnorderedAccessView_Bloom_Temp);
	Safe_Release(m_pShaderResourceView_Bloom_Temp);
	Safe_Release(m_pBloomTex_Temp);

	Safe_Release(m_pUnorderedAccessView_Bloom);
	Safe_Release(m_pShaderResourceView_Bloom);
	Safe_Release(m_pBloomTex);

	Safe_Release(m_pUnorderedAccessView_Lum);
	Safe_Release(m_pShaderResourceView_Lum);
	Safe_Release(m_pHDRBuffer_Lum);

	Safe_Release(m_pUnorderedAccessView_LumAve);
	Safe_Release(m_pShaderResourceView_LumAve);
	Safe_Release(m_pHDRBuffer_LumAve);

	Safe_Release(m_pUnorderedAccessView_PrevLumAve);
	Safe_Release(m_pShaderResourceView_PrevLumAve);
	Safe_Release(m_pHDRBuffer_PrevLumAve);

	for (auto& InputLayout : m_InputLayouts_CS_Bloom)
	{
		Safe_Release(InputLayout.pPass);
		Safe_Release(InputLayout.pLayout);
	}
	m_InputLayouts_CS_Bloom.clear();
	Safe_Release(m_pEffect_CS_Bloom);

	for (auto& InputLayout : m_InputLayouts_CS)
	{
		Safe_Release(InputLayout.pPass);
		Safe_Release(InputLayout.pLayout);
	}
	m_InputLayouts_CS.clear();
	Safe_Release(m_pEffect_CS);

	Safe_Release(m_pVIBuffer_ToneMapping);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}

