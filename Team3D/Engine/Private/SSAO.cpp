#include "SSAO.h"
#include "RenderTarget_Manager.h"
#include "Graphic_Device.h"
#include "Pipeline.h"

#ifdef _DEBUG
#include "VIBuffer_RectRHW.h"
#endif

IMPLEMENT_SINGLETON(CSSAO)

HRESULT CSSAO::Ready_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(pDevice);
	Safe_AddRef(pDeviceContext);

	m_iWinSize[0] = (_uint)fBufferWidth;
	m_iWinSize[1] = (_uint)fBufferHeight;
	
	m_iDownScaledWidth = (_uint)(m_iWinSize[0] * 0.5f);
	m_iDownScaledHeight = (_uint)(m_iWinSize[1] * 0.5f);

	FAILED_CHECK_RETURN(Build_DownScaledDepthBuffer(), E_FAIL);
	FAILED_CHECK_RETURN(Build_SSAOResources(), E_FAIL);
	FAILED_CHECK_RETURN(Build_ComputeShaders(TEXT("../Bin/ShaderFiles/ComputeShader_SSAO.hlsl"), "DefaultTechnique"), E_FAIL);

	return S_OK;
}

HRESULT CSSAO::Compute_SSAO()
{
	/* Constants */
	_float	fCamFar;
	_matrix	ProjMatrixInverse;
	_matrix ViewMatrix;
	_float4	vViewportUVInfo;

	CGraphic_Device* pGraphicDevice = CGraphic_Device::GetInstance();
	CPipeline* pPipeline = CPipeline::GetInstance();

	/* For.MainView */
	fCamFar = pPipeline->Get_MainCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	ViewMatrix = pPipeline->Get_Transform(CPipeline::TS_MAINVIEW);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_MAIN);
	FAILED_CHECK_RETURN(Set_Variable("g_fMainCamFar", &fCamFar, sizeof(_float)), E_FAIL);

	FAILED_CHECK_RETURN(Set_Variable("g_MainProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	/* For.SubView */
	fCamFar = pPipeline->Get_SubCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	ViewMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_SUB);
	FAILED_CHECK_RETURN(Set_Variable("g_fSubCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(Set_Variable("g_SubProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	FAILED_CHECK_RETURN(DownScaleDepth(), E_FAIL);
	FAILED_CHECK_RETURN(SSAO(), E_FAIL);

	return S_OK;
}

HRESULT CSSAO::DownScaleDepth()
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	// Nomap Map / Depth Map
	CRenderTarget_Manager* pRenderTargerManager =  CRenderTarget_Manager::GetInstance();

	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_NormalsTexture", pRenderTargerManager->Get_ShaderResourceView(TEXT("Target_Normal"))), E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_DepthTexture", pRenderTargerManager->Get_ShaderResourceView(TEXT("Target_Depth"))), E_FAIL);
	
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_RWDownScaledDepth", m_pUAV_DownScaledDepth), E_FAIL);
	
	FAILED_CHECK_RETURN(m_InputLayouts_CS[0].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->Dispatch((UINT)ceil((float)(m_iDownScaledWidth * m_iDownScaledHeight) / 1024.0f), 1, 1);
	Unbind_ShaderResources(); // Reset Views

	return S_OK;
}

HRESULT CSSAO::SSAO()
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_DownScaledDepth", m_pSRV_DownScaledDepth), E_FAIL);

	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_RWSSAO", m_pUAV_SSAO), E_FAIL);

	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->Dispatch((UINT)ceil((float)(m_iDownScaledWidth * m_iDownScaledHeight) / 1024.0f), 1, 1);
	Unbind_ShaderResources(); // Reset Views

	return S_OK;
}

HRESULT CSSAO::Build_DownScaledDepthBuffer()
{
	// Allocate down scaled depth buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = 4 * sizeof(_float);
	bufferDesc.ByteWidth = m_iDownScaledWidth * m_iDownScaledHeight * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pDownScaledDepthBuffer), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_iDownScaledWidth * m_iDownScaledHeight;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledDepthBuffer, &UAVDesc, &m_pUAV_DownScaledDepth), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_iDownScaledWidth * m_iDownScaledHeight;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledDepthBuffer, &SRVDesc, &m_pSRV_DownScaledDepth), E_FAIL);

	return S_OK;
}

HRESULT CSSAO::Build_SSAOResources()
{
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_iDownScaledWidth, //UINT Width;
		m_iDownScaledHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R32_TYPELESS,//DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&t2dDesc, NULL, &m_pSSAOTex),E_FAIL);

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pSSAOTex, &UAVDesc, &m_pUAV_SSAO), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pSSAOTex, &SRVDesc, &m_pSRV_SSAO), E_FAIL);

	return S_OK;
}

HRESULT CSSAO::Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName)
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

HRESULT CSSAO::Set_Variable(const char* pConstantName, void* pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectVariable* pVariable = m_pEffect_CS->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CSSAO::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pSrvVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pSrvVariable, E_FAIL);

	return pSrvVariable->SetResource(pResourceView);
}

HRESULT CSSAO::Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUavVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	NULL_CHECK_RETURN(pUavVariable, E_FAIL);
	return pUavVariable->SetUnorderedAccessView(pResourceView);
}

HRESULT CSSAO::Unbind_ShaderResources()
{
	ID3D11ShaderResourceView* pNullSRV[8] = { 0 };
	ID3D11UnorderedAccessView* pNullUAV[8] = { 0 };

	m_pDeviceContext->CSSetShaderResources(0, 8, pNullSRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, pNullUAV, 0);

	//m_pDeviceContext->CSSetShader(0, 0, 0);

	return S_OK;
}

void CSSAO::Free()
{
	Safe_Release(m_pUAV_SSAO);
	Safe_Release(m_pSRV_SSAO);
	Safe_Release(m_pSSAOTex);

	Safe_Release(m_pUAV_DownScaledDepth);
	Safe_Release(m_pSRV_DownScaledDepth);
	Safe_Release(m_pDownScaledDepthBuffer);

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
	Safe_Release(m_pVIBuffer);
#endif
}

#ifdef _DEBUG
HRESULT CSSAO::Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, fX, fY, fSizeX, fSizeY, TEXT("../Bin/ShaderFiles/Shader_RenderTarget.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

HRESULT CSSAO::Render_DebugBuffer()
{
	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pSRV_SSAO);

	m_pVIBuffer->Render(0);

	return S_OK;
}

void CSSAO::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer);
}
#endif