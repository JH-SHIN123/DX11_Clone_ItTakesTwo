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
	FAILED_CHECK_RETURN(Build_ComputeShaders(TEXT("../Bin/ShaderFiles/ComputeShader_HDR.hlsl"), "DefaultTechnique"), E_FAIL);

	return S_OK;
}

HRESULT CHDR::Render_HDR()
{
	Dispatch();

	// PS ---------------------------------------------------------------------------
	// Tone Mapping
	NULL_CHECK_RETURN(m_pVIBuffer_ToneMapping, E_FAIL);
	
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_fMiddleGrey += 0.01f;
	else if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_fMiddleGrey -= 0.01f;

	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_fLumWhiteSqr += 0.01f;
	else if (GetAsyncKeyState(VK_F4) & 0x8000)
		m_fLumWhiteSqr -= 0.01f;

	m_pVIBuffer_ToneMapping->Set_Variable("g_MiddleGrey", &m_fMiddleGrey, sizeof(_float));
	m_pVIBuffer_ToneMapping->Set_Variable("g_LumWhiteSqr", &m_fLumWhiteSqr, sizeof(_float));

	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Blend")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_AverageLum", m_pShaderResourceView_LumAve);

	m_pVIBuffer_ToneMapping->Render(0);

	return S_OK;
}

HRESULT CHDR::Dispatch()
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	// CS ---------------------------------------------------------------------------
	// Set Constant Buffer
	//_uint iRes[2] = { m_iWinSize[0] / 4, m_iWinSize[1] / 4 };
	//FAILED_CHECK_RETURN(Set_Variable("g_Res", iRes, sizeof(_uint)  * 2), E_FAIL);
	//_uint iDomain = m_iWinSize[0] * m_iWinSize[1] / 16;
	//FAILED_CHECK_RETURN(Set_Variable("g_Domain", &iDomain, sizeof(_uint)), E_FAIL);
	//_uint iGroupSize = m_iWinSize[0] * m_iWinSize[1] / 16 * 1024;
	//FAILED_CHECK_RETURN(Set_Variable("g_GroupSize", &iGroupSize, sizeof(_uint)), E_FAIL);

	// For. First Pass
	// Set HDR Texture
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Blend"))),E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageValues1D", m_pShaderResourceView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_Lum), E_FAIL);
	
	FAILED_CHECK_RETURN(m_InputLayouts_CS[0].pPass->Apply(0, m_pDeviceContext),E_FAIL);
	m_pDeviceContext->Dispatch(1024, 1,1);

	// For. Second Pass
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageValues1D", m_pShaderResourceView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_LumAve), E_FAIL);

	// MAX_GROUPS : 64
	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->Dispatch(MAX_GROUPS_THREAD, 1, 1);

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
	ID3D11ShaderResourceView* pNullSRV[1] = { 0 };
	ID3D11UnorderedAccessView* pNullUAV[1] = { 0 };

	m_pDeviceContext->CSSetShaderResources(0, 1, pNullSRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, pNullUAV, 0);
	//m_pDeviceContext->CSSetShader(0, 0, 0);

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
	Safe_Release(m_pUnorderedAccessView_Lum);
	Safe_Release(m_pShaderResourceView_Lum);
	Safe_Release(m_pHDRBuffer_Lum);

	Safe_Release(m_pUnorderedAccessView_LumAve);
	Safe_Release(m_pShaderResourceView_LumAve);
	Safe_Release(m_pHDRBuffer_LumAve);

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

#ifdef _DEBUG
HRESULT CHDR::Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_pVIBuffer_Debug = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, fX, fY, fSizeX, fSizeY, TEXT("../Bin/ShaderFiles/Shader_RenderTarget.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer_Debug, E_FAIL);
	return S_OK;
}
HRESULT CHDR::Render_DebugBuffer()
{
	m_pVIBuffer_Debug->Set_ShaderResourceView("g_HDRDebugBuffer", m_pShaderResourceView_LumAve);
	m_pVIBuffer_Debug->Render(1);

	return S_OK;
}
void CHDR::Clear_DebugBuffers()
{
	Safe_Release(m_pVIBuffer_Debug);
}
#endif