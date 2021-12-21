#include "..\Public\PostFX.h"
#include "Graphic_Device.h"
#include "RenderTarget_Manager.h"
#include "Pipeline.h"
#include "VIBuffer_RectRHW.h"
#include "Blur.h"
#include "ShaderCompiler.h"
#include "Textures.h"

IMPLEMENT_SINGLETON(CPostFX)

void CPostFX::Set_RadiarBlur_Main(_bool bActive, _float2& vFocusPos)
{
	m_vRadiarBlur_FocusPos_Main = vFocusPos;

	if (true == bActive)
	{
		m_bRadialBlur_Main = true;
		m_bRadialBlur_Main_Finish = false;
		m_fRadialBlur_MainRatio = 1.f;
	}
	else
	{
		m_bRadialBlur_Main_Finish = true;
		m_fRadialBlur_MainRatio = 1.f;
	}
}

void CPostFX::Set_RadiarBlur_Sub(_bool bActive, _float2& vFocusPos)
{
	m_vRadiarBlur_FocusPos_Sub = vFocusPos;

	if (true == bActive)
	{
		m_bRadialBlur_Sub = true;
		m_bRadialBlur_Sub_Finish = false;
		m_fRadialBlur_SubRatio = 1.f;
	}
	else
	{
		m_bRadialBlur_Sub_Finish = true;
		m_fRadialBlur_SubRatio = 1.f;
	}
}

HRESULT CPostFX::Ready_PostFX(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
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

	m_pVIBuffer_ToneMapping = CVIBuffer_RectRHW::Create(pDevice, pDeviceContext, 0.f, 0.f, fBufferWidth, fBufferHeight, TEXT("../Bin/ShaderFiles/Shader_PostFX.hlsl"), "DefaultTechnique");

	FAILED_CHECK_RETURN(Build_LuminanceBuffer(fBufferWidth, fBufferHeight),E_FAIL);
	FAILED_CHECK_RETURN(Build_BloomResources(fBufferWidth, fBufferHeight), E_FAIL);
	FAILED_CHECK_RETURN(Build_DOFBlurResources(fBufferWidth, fBufferHeight), E_FAIL);
	FAILED_CHECK_RETURN(Build_ComputeShaders(TEXT("../Bin/ShaderFiles/ComputeShader_PostFX.hlsl"), "DefaultTechnique"), E_FAIL);

	m_pRadiarBlur_Mask = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/PostFX/radiarblur.png"));
	m_pVignatte_Mask = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/PostFX/vignatte%d.png"),2);
	m_pVolume_Mask = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, TEXT("../Bin/Resources/Texture/PostFX/T_Smoke_01.png"));

	return S_OK;
}

HRESULT CPostFX::PostProcessing(_double TimeDelta)
{
#ifdef _DEBUG
	//FAILED_CHECK_RETURN(KeyInput_Test(TimeDelta), E_FAIL);
#endif // _DEBUG

	FAILED_CHECK_RETURN(Tick_Adaptation(TimeDelta), E_FAIL);
	FAILED_CHECK_RETURN(Tick_RadiarBlur(TimeDelta), E_FAIL);

	if (m_fVolumeTimeDelta >= 1.f) m_fVolumeTimeDelta = 0.f;
	else m_fVolumeTimeDelta += (_float)TimeDelta * 0.15f;

	FAILED_CHECK_RETURN(DownScale(TimeDelta), E_FAIL);
	FAILED_CHECK_RETURN(Bloom(), E_FAIL);
	FAILED_CHECK_RETURN(Blur(m_pShaderResourceView_Bloom_Temp, m_pUnorderedAccessView_Bloom), E_FAIL);
	FAILED_CHECK_RETURN(Blur(m_pShaderResourceView_DownScaledHDR, m_pUnorderedAccessView_DORBlur), E_FAIL);
	FAILED_CHECK_RETURN(Blur_Effects(), E_FAIL);
	FAILED_CHECK_RETURN(FinalPass(),E_FAIL);

	// Swap Cur LumAvg - Prev LumAvg
	// 두 버퍼의 값을 교체하고, 현재 프레임의 평균 휘도를 다음프레임의 "prevLumAvg"로 저장한다.
	swap(m_pHDRBuffer_PrevLumAve, m_pHDRBuffer_LumAve);
	swap(m_pUnorderedAccessView_PrevLumAve, m_pUnorderedAccessView_LumAve);
	swap(m_pShaderResourceView_PrevLumAve, m_pShaderResourceView_LumAve);

	return S_OK;
}

HRESULT CPostFX::DownScale(_double TimeDelta)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	// CS ---------------------------------------------------------------------------
	// For. First Pass
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();

	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_PostFX"))), E_FAIL);

	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_HDRDownScale", m_pUnorderedAccessView_DownScaledHDR), E_FAIL); // HDR 다운스케일 텍스쳐 제작

	FAILED_CHECK_RETURN(m_InputLayouts_CS[0].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	m_pDeviceContext->Dispatch(m_iDownScaleGroups, 1, 1);
	Unbind_ShaderResources(); // Reset Views

	// For. Second Pass
	/* Constant */
	FAILED_CHECK_RETURN(Set_Variable("g_Adaptation", &m_fAdaptation, sizeof(_float)), E_FAIL);
	
	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageValues1D", m_pShaderResourceView_Lum), E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_PrevAverageLum", m_pShaderResourceView_PrevLumAve), E_FAIL);

	/* Ouput */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_AverageLum", m_pUnorderedAccessView_LumAve), E_FAIL);

	// MAX_GROUPS : 64
	FAILED_CHECK_RETURN(m_InputLayouts_CS[1].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->Dispatch(1, 1, 1);
	Unbind_ShaderResources(); // Reset Views

	return S_OK;
}

HRESULT CPostFX::Bloom()
{
	// BrightRight - 다운스케일된 HDR과 SRV의 평균휘도를 계산한후, 임시로 UAV(m_pUnorderedAccessView_Bloom_Temp)로 저장한다.
	
	/* Constants */
	Set_Variable("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float));

	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_HDRDownScaleTex", m_pShaderResourceView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_AverageLum1D", m_pShaderResourceView_LumAve), E_FAIL);

	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Bloom", m_pUnorderedAccessView_Bloom_Temp), E_FAIL);

	FAILED_CHECK_RETURN(m_InputLayouts_CS[2].pPass->Apply(0, m_pDeviceContext), E_FAIL);
	m_pDeviceContext->Dispatch(m_iDownScaleGroups, 1, 1);
	Unbind_ShaderResources();

	return S_OK;
}

HRESULT CPostFX::Blur(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput)
{
	//> 2 pass - verticla & horizontal
	// 수직 / 수평 가우시안 블러 & 첫 패스에서 휘도 다운스케일에 쓰인 그룹숫자만큼의 컴퓨트셰이더를 적용한다.
	// 0 pass : 첫째 임시 텍스쳐 SRV(m_pShaderResourceView_Bloom_Temp)로부터 픽셀을 수평방향으로 필터링해 둘째 임시 텍스쳐 UAV(m_pUnorderedAccessView_Bloom)로 저장한다.

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Horizontal
	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", pInput), E_FAIL);
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", m_pUnorderedAccessView_Bloom_Temp2), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[4].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	_uint x = (_uint)ceil((m_iWinSize[0] / 4.0f) / (128.0f - 12.0f));
	_uint y = (_uint)ceil(m_iWinSize[1] / 4.0f);
	m_pDeviceContext->Dispatch(x, y, 1);
	Unbind_ShaderResources();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vertical
	/* Input */
	FAILED_CHECK_RETURN(Set_ShaderResourceView("g_Input", m_pShaderResourceView_Bloom_Temp2), E_FAIL);
	/* Output */
	FAILED_CHECK_RETURN(Set_UnorderedAccessView("g_Output", pOutput), E_FAIL);
	FAILED_CHECK_RETURN(m_InputLayouts_CS[3].pPass->Apply(0, m_pDeviceContext), E_FAIL);

	x = (_uint)ceil(m_iWinSize[0] / 4.0f);
	y = (_uint)ceil((m_iWinSize[1] / 4.0f) / (128.0f - 12.0f));
	m_pDeviceContext->Dispatch(x, y, 1);
	Unbind_ShaderResources();

	return S_OK;
}

HRESULT CPostFX::Blur_Effects()
{
	return CBlur::GetInstance()->Blur_Effect();
}

HRESULT CPostFX::FinalPass()
{
	// PS - Tone Mapping
	NULL_CHECK_RETURN(m_pVIBuffer_ToneMapping, E_FAIL);

	CGraphic_Device* pGraphicDevice = CGraphic_Device::GetInstance();
	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();
	CPipeline* pPipeline = CPipeline::GetInstance();
	CBlur* pBlur = CBlur::GetInstance();

	/* HDR */
	_float fMiddleGrey = m_fMiddleGrey;
	_float fLumWhiteSqr = m_fLumWhiteSqr;
	fLumWhiteSqr *= fMiddleGrey;
	fLumWhiteSqr *= fLumWhiteSqr;
	m_pVIBuffer_ToneMapping->Set_Variable("g_MiddleGrey", &fMiddleGrey, sizeof(_float));
	m_pVIBuffer_ToneMapping->Set_Variable("g_LumWhiteSqr", &fLumWhiteSqr, sizeof(_float));
	m_pVIBuffer_ToneMapping->Set_Variable("g_BloomScale", &m_fBloomScale, sizeof(_float));

	/* Vignatte */
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_VignatteTex", m_pVignatte_Mask->Get_ShaderResourceView(0));

	/* Fog */
	m_pVIBuffer_ToneMapping->Set_Variable("g_bFog", &m_bMainFog, sizeof(m_bMainFog));

	/* Radiar Blur */
	m_pVIBuffer_ToneMapping->Set_Variable("g_bRadiarBlur_Main", &m_bRadialBlur_Main, sizeof(m_bRadialBlur_Main)); 
	m_pVIBuffer_ToneMapping->Set_Variable("g_bRadiarBlur_Sub", &m_bRadialBlur_Sub, sizeof(m_bRadialBlur_Sub));
	m_pVIBuffer_ToneMapping->Set_Variable("g_RadiarBlur_FocusPos_Main", &m_vRadiarBlur_FocusPos_Main, sizeof(m_vRadiarBlur_FocusPos_Main));
	m_pVIBuffer_ToneMapping->Set_Variable("g_RadiarBlur_FocusPos_Sub", &m_vRadiarBlur_FocusPos_Sub, sizeof(m_vRadiarBlur_FocusPos_Sub));
	m_pVIBuffer_ToneMapping->Set_Variable("g_fRadiarBlurRatio_Main", &m_fRadialBlur_MainRatio, sizeof(m_fRadialBlur_MainRatio));
	m_pVIBuffer_ToneMapping->Set_Variable("g_fRadiarBlurRatio_Sub", &m_fRadialBlur_SubRatio, sizeof(m_fRadialBlur_SubRatio));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_RadiarBlurMaskTex", m_pRadiarBlur_Mask->Get_ShaderResourceView(0));

	/* Volume */
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_VolumeTex_Front", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Volume_Front")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_VolumeTex_Back", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Volume_Back")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_VolumeMaskTex", m_pVolume_Mask->Get_ShaderResourceView(0));
	m_pVIBuffer_ToneMapping->Set_Variable("g_fTime", &m_fVolumeTimeDelta, sizeof(_float));

	_float	fCamFar;
	_vector vCamPosition;
	_matrix	ProjMatrixInverse;
	_matrix	ViewMatrixInverse;
	_float4	vViewportUVInfo;

	/* For.MainView */
	vCamPosition = pPipeline->Get_MainCamPosition();
	fCamFar = pPipeline->Get_MainCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	ViewMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_MAIN);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_fMainCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_vMainCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_MainProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_MainViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	/* For.SubView */
	vCamPosition = pPipeline->Get_SubCamPosition();
	fCamFar = pPipeline->Get_SubCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	ViewMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_SUB);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_fSubCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_vSubCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_SubProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_SubViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer_ToneMapping->Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_HDRTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_PostFX")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_BloomTexture", m_pShaderResourceView_Bloom);
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_DOFBlurTex", m_pShaderResourceView_DORBlur);
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_DepthTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Depth")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_EffectTex", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_Effect")));
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_EffectBlurTex", pBlur->Get_ShaderResourceView_BlurEffect());
	m_pVIBuffer_ToneMapping->Set_ShaderResourceView("g_AverageLum", m_pShaderResourceView_LumAve);

	m_pVIBuffer_ToneMapping->Render(0);

	/* Unbind PS */
	ID3D11ShaderResourceView* pSRV[16] = { nullptr };
	m_pDeviceContext->PSSetShaderResources(0, 16, pSRV);

	return S_OK;
}

HRESULT CPostFX::Tick_Adaptation(_double TimeDelta)
{
	// Set Adaptation
	m_fAdaptationDeltaT += (_float)TimeDelta;

	static _bool bAdaptationFirstTime = true;
	_float fAdaptNorm = 0.f;
	if (bAdaptationFirstTime)
	{
		fAdaptNorm = 0.f;
		bAdaptationFirstTime = false;
	}
	else
	{
		fAdaptNorm = min(m_fAdaptTime < 0.0001f ? 1.f : (m_fAdaptationDeltaT / m_fAdaptTime), 0.9999f);
	}

	m_fAdaptation = fAdaptNorm;

	return S_OK;
}

HRESULT CPostFX::Tick_RadiarBlur(_double TimeDelta)
{
	_float fSpeed = 5.f;

	if (m_bRadialBlur_Main_Finish)
	{
		if (m_fRadialBlur_MainRatio < 0)
		{
			m_fRadialBlur_MainRatio = 1.0;
			m_bRadialBlur_Main_Finish = false;
			m_bRadialBlur_Main = false;
		}
		else
		{
			m_fRadialBlur_MainRatio -= (_float)TimeDelta * fSpeed;
		}
	}

	if (m_bRadialBlur_Sub_Finish)
	{
		if (m_fRadialBlur_SubRatio < 0)
		{
			m_fRadialBlur_SubRatio = 1.0;
			m_bRadialBlur_Sub_Finish = false;
			m_bRadialBlur_Sub = false;
		}
		else
		{
			m_fRadialBlur_SubRatio -= (_float)TimeDelta * fSpeed;
		}
	}

	return S_OK;
}

HRESULT CPostFX::Build_LuminanceBuffer(_float iWidth, _float iHeight)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	/* 휘도의 중간값을 저장하기 위한 리소스들 */
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.StructureByteStride = sizeof(_float);
	BufferDesc.ByteWidth = m_iDownScaleGroups * BufferDesc.StructureByteStride;
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_Lum), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_iDownScaleGroups;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_Lum, &UAVDesc, &m_pUnorderedAccessView_Lum), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_iDownScaleGroups;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_Lum, &SRVDesc, &m_pShaderResourceView_Lum), E_FAIL);

	/* 부동소수점 형태로 평균 휘도 값을 저장 (현재, 이전) */
	BufferDesc.ByteWidth = sizeof(_float);
	BufferDesc.StructureByteStride = sizeof(_float);
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_LumAve), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pHDRBuffer_PrevLumAve), E_FAIL);

	UAVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_LumAve, &UAVDesc, &m_pUnorderedAccessView_LumAve), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pHDRBuffer_PrevLumAve, &UAVDesc, &m_pUnorderedAccessView_PrevLumAve), E_FAIL);

	SRVDesc.Buffer.NumElements = 1;
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_LumAve, &SRVDesc, &m_pShaderResourceView_LumAve), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pHDRBuffer_PrevLumAve, &SRVDesc, &m_pShaderResourceView_PrevLumAve), E_FAIL);

	return S_OK;
}

HRESULT CPostFX::Build_BloomResources(_float iWidth, _float iHeight)
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

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDownScaledHDRTex), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex_Temp2), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pBloomTex), E_FAIL);
	

	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDownScaledHDRTex, &ShaderResourceViewDesc, &m_pShaderResourceView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pBloomTex_Temp, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pBloomTex_Temp2, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom_Temp2), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pBloomTex, &ShaderResourceViewDesc, &m_pShaderResourceView_Bloom), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
	ZeroMemory(&UnorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UnorderedAccessViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UnorderedAccessViewDesc.Buffer.FirstElement = 0;
	UnorderedAccessViewDesc.Buffer.NumElements = m_iWinSize[0] * m_iWinSize[1] / 16;

	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDownScaledHDRTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DownScaledHDR), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pBloomTex_Temp, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom_Temp), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pBloomTex_Temp2, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom_Temp2), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pBloomTex, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_Bloom), E_FAIL);

	return S_OK;
}

HRESULT CPostFX::Build_DOFBlurResources(_float iWidth, _float iHeight)
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
	
	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDORBlur), E_FAIL);

	D3D11_SHADER_RESOURCE_VIEW_DESC	 ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pDORBlur, &ShaderResourceViewDesc, &m_pShaderResourceView_DORBlur), E_FAIL);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
	ZeroMemory(&UnorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UnorderedAccessViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UnorderedAccessViewDesc.Buffer.FirstElement = 0;
	UnorderedAccessViewDesc.Buffer.NumElements = m_iWinSize[0] * m_iWinSize[1] / 16;

	FAILED_CHECK_RETURN(m_pDevice->CreateUnorderedAccessView(m_pDORBlur, &UnorderedAccessViewDesc, &m_pUnorderedAccessView_DORBlur), E_FAIL);

	return S_OK;
}

HRESULT CPostFX::Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName)
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

HRESULT CPostFX::Set_Variable(const char* pConstantName, void* pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectVariable* pVariable = m_pEffect_CS->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CPostFX::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect_CS, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pSrvVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pSrvVariable, E_FAIL);

	return pSrvVariable->SetResource(pResourceView);
}

HRESULT CPostFX::Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUavVariable = m_pEffect_CS->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	NULL_CHECK_RETURN(pUavVariable, E_FAIL);
	return pUavVariable->SetUnorderedAccessView(pResourceView);
}

HRESULT CPostFX::Unbind_ShaderResources()
{
	ID3D11ShaderResourceView* pNullSRV[8] = { 0 };
	ID3D11UnorderedAccessView* pNullUAV[8] = { 0 };

	m_pDeviceContext->CSSetShaderResources(0, 8, pNullSRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, pNullUAV, 0);

	//m_pDeviceContext->CSSetShader(0, 0, 0);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPostFX::KeyInput_Test(_double TimeDelta)
{
	//if (GetAsyncKeyState(VK_F1) & 0x8000)
	//	m_fMiddleGrey += 0.005f; // 0.0005
	//else if (GetAsyncKeyState(VK_F2) & 0x8000)
	//	m_fMiddleGrey -= 0.005f;

	//if (GetAsyncKeyState(VK_F3) & 0x8000)
	//	m_fLumWhiteSqr += 0.05f;
	//else if (GetAsyncKeyState(VK_F4) & 0x8000)
	//	m_fLumWhiteSqr -= 0.05f;

	//if (GetAsyncKeyState(VK_F5) & 0x8000)
	//	m_fBloomScale += TimeDelta;
	//else if (GetAsyncKeyState(VK_F6) & 0x8000)
	//	m_fBloomScale -= TimeDelta;

#ifdef _DEBUG
	TCHAR szBuff[256] = L"";
	GetPrivateProfileString(L"Section_1", L"Key_1", L"0", szBuff, 256, L"../test.ini");;
	_float a = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_1", L"Key_2", L"0", szBuff, 256, L"../test.ini");
	_float b = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_1", L"Key_3", L"0", szBuff, 256, L"../test.ini");
	_float c = (_float)_wtof(szBuff);
	GetPrivateProfileString(L"Section_1", L"Key_4", L"0", szBuff, 256, L"../test.ini");
	_float d = (_float)_wtof(szBuff);
	
	m_fMiddleGrey = a;
	m_fLumWhiteSqr = b;
	m_fBloomScale = c;
	m_fBloomThreshold = d;

	//GetPrivateProfileString(L"Section_1", L"Key_5", L"0", szBuff, 256, L"../test.ini");
	//_float e = (_float)_wtof(szBuff);
	//GetPrivateProfileString(L"Section_1", L"Key_6", L"0", szBuff, 256, L"../test.ini");
	//_float f = (_float)_wtof(szBuff);
	//
	//m_fRadiarDist = e;
	//m_fRadiarStr = f;
#endif // _DEBUG

	return S_OK;
}
#endif // _DEBUG

void CPostFX::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer_ToneMapping);
	Safe_Release(m_pRadiarBlur_Mask);
	Safe_Release(m_pVignatte_Mask);
	Safe_Release(m_pVolume_Mask);
}

void CPostFX::Free()
{
	Safe_Release(m_pShaderResourceView_DownScaledHDR);
	Safe_Release(m_pUnorderedAccessView_DownScaledHDR);
	Safe_Release(m_pDownScaledHDRTex);

	Safe_Release(m_pShaderResourceView_DORBlur);
	Safe_Release(m_pUnorderedAccessView_DORBlur);
	Safe_Release(m_pDORBlur);

	Safe_Release(m_pUnorderedAccessView_Bloom_Temp);
	Safe_Release(m_pShaderResourceView_Bloom_Temp);
	Safe_Release(m_pBloomTex_Temp);

	Safe_Release(m_pUnorderedAccessView_Bloom_Temp2);
	Safe_Release(m_pShaderResourceView_Bloom_Temp2);
	Safe_Release(m_pBloomTex_Temp2);

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

