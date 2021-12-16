#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Light_Manager.h"
#include "RenderTarget_Manager.h"
#include "VIBuffer_RectRHW.h"
#include "Graphic_Device.h"
#include "Shadow_Manager.h"
#include "Input_Device.h"
#include "PostFX.h"
#include "Blur.h"
#include "SSAO.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pRenderTarget_Manager(CRenderTarget_Manager::GetInstance())
{
	Safe_AddRef(m_pRenderTarget_Manager);
}

CRenderer::CRenderer(const CRenderer & rhs)
	: CComponent(rhs)
{
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	CComponent::NativeConstruct_Prototype();

	D3D11_VIEWPORT	ViewportDesc = CGraphic_Device::GetInstance()->Get_ViewportInfo(CGraphic_Device::VP_FULL);

	_uint iWidth	= (_uint)ViewportDesc.Width;
	_uint iHeight = (_uint)ViewportDesc.Height;

	/* MRT_Deferred */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_SNORM, _float4(1.f, 1.f, 1.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Diffuse"), TEXT("MRT_Deferred")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Normal"), TEXT("MRT_Deferred")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Depth"), TEXT("MRT_Deferred")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Shadow"), TEXT("MRT_Deferred")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular_Src"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Specular_Src"), TEXT("MRT_Deferred")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Emissive"), TEXT("MRT_Deferred")), E_FAIL);

	/* MRT_LightAcc */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Shade"), TEXT("MRT_LightAcc")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Specular"), TEXT("MRT_LightAcc")), E_FAIL);

	/* MRT_ShadowDepth */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_CascadedShadow_Depth"), SHADOWMAP_SIZE, SHADOWMAP_SIZE * MAX_CASCADES, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), true), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_CascadedShadow_Depth"), TEXT("MRT_CascadedShadow")), E_FAIL);

	/* MRT_Volume */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Volume_Front"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Volume_Front"), TEXT("MRT_Volume_Front")), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Volume_Back"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Volume_Back"), TEXT("MRT_Volume_Back")), E_FAIL);

	/* MRT_PostFX */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_PostFX"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_PostFX"), TEXT("MRT_PostFX")), E_FAIL);

	/* MRT_Effect */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Effect"), TEXT("MRT_Effect")), E_FAIL);

	m_pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_Blend.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

#ifdef _DEBUG
	_float fWidth	= ViewportDesc.Width / 7.f;
	_float fHeight	= ViewportDesc.Height / 7.f;

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Diffuse"), 0.f, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Normal"), 0.f, fHeight, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Depth"), 0.f, fHeight * 2.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Shadow"), 0.f, fHeight * 3.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Specular_Src"), 0.f, fHeight * 4.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Emissive"), 0.f, fHeight * 5.f, fWidth, fHeight), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Shade"), fWidth, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Specular"), fWidth, fHeight, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Volume_Front"), fWidth, fHeight * 2.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Volume_Back"), fWidth, fHeight * 3.f, fWidth, fHeight), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_CascadedShadow_Depth"), fWidth * 2.f, 0.f, fWidth, fHeight * MAX_CASCADES), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_PostFX"), fWidth * 3.f, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Effect"), fWidth * 3.f, fHeight, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(CSSAO::GetInstance()->Ready_DebugBuffer(fWidth * 3.f, fHeight * 2.f, fWidth, fHeight), E_FAIL);

	FAILED_CHECK_RETURN(CBlur::GetInstance()->Ready_DebugBuffer(TEXT("Target_EmissiveBlur"), fWidth * 4.f, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(CBlur::GetInstance()->Ready_DebugBuffer(TEXT("Target_EffectBlur"), fWidth * 4.f, fHeight, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(CBlur::GetInstance()->Ready_DebugBuffer(TEXT("Target_SpecularBlur"), fWidth * 4.f, fHeight * 2.f, fWidth, fHeight), E_FAIL);
#endif

	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CRenderer::Add_GameObject_ToRenderGroup(RENDER_GROUP::Enum eGroup, CGameObject * pGameObject)
{
	if (eGroup >= RENDER_GROUP::RENDER_END) { return E_FAIL; }
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_RenderObjects[eGroup].emplace_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_Renderer(_double TimeDelta)
{
	// 0 - pass
	FAILED_CHECK_RETURN(Render_ShadowsForAllCascades(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Volume(), E_FAIL);
	
	// 1- pass
	FAILED_CHECK_RETURN(Render_Priority(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonAlpha(), E_FAIL);

	FAILED_CHECK_RETURN(Compute_SSAO(),  E_FAIL); /* Calculate Occlution Ambient for Directinal Light */
	
	FAILED_CHECK_RETURN(Render_LightAcc(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Blend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Alpha(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Effect(), E_FAIL);
	FAILED_CHECK_RETURN(PostProcessing(TimeDelta), E_FAIL);

	FAILED_CHECK_RETURN(Render_Effect_No_Blur(), E_FAIL);
	FAILED_CHECK_RETURN(Render_UI(), E_FAIL);

#ifdef _DEBUG
	if (CInput_Device::GetInstance()->Key_Down(DIK_1) && CInput_Device::GetInstance()->Key_Pressing(DIK_LCONTROL))
		m_bShowDebugBuffer = !m_bShowDebugBuffer;

	if (m_bShowDebugBuffer)
	{
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_CascadedShadow"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_PostFX"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_Effect"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_Volume_Front"));
		m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_Volume_Back"));

		CSSAO::GetInstance()->Render_DebugBuffer();
		CBlur::GetInstance()->Render_DebugBuffer_Emissive(TEXT("Target_EmissiveBlur"));
		CBlur::GetInstance()->Render_DebugBuffer_Effect(TEXT("Target_EffectBlur"));
		CBlur::GetInstance()->Render_DebugBuffer_Specular(TEXT("Target_SpecularBlur"));
	}
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_PostFX"), true, true);
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_PRIORITY])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_PRIORITY), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_PRIORITY].clear();
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_PostFX"));

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"));
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_NONALPHA])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_NONALPHA), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_NONALPHA].clear();
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Deferred"));

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	//Sort_GameObjects(m_RenderObjects[RENDER_GROUP::RENDER_ALPHA]);

	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_PostFX"), false);
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_ALPHA])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_ALPHA), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_ALPHA].clear();
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_PostFX"));

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Effect"));
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_EFFECT])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_EFFECT), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_EFFECT].clear();
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Effect"));

	return S_OK;
}

HRESULT CRenderer::Render_Effect_No_Blur()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_EFFECT_NO_BLUR])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_EFFECT_NO_BLUR), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_EFFECT_NO_BLUR].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	Sort_GameObjects(m_RenderObjects[RENDER_GROUP::RENDER_UI]);

	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_UI])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_UI), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowsForAllCascades()
{
	NULL_CHECK_RETURN(m_pRenderTarget_Manager, E_FAIL);

	// FullScreen Render pass (Main / Sub)

	// if directional light is not exit, not render shadow
	CShadow_Manager* pShadowManager = CShadow_Manager::GetInstance();
	if (nullptr == pShadowManager) return E_FAIL;

	// Set Each Cascade Shadow viewports
	FAILED_CHECK_RETURN(pShadowManager->RSSet_CascadedViewports(), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_CascadedShadow"),true,true), E_FAIL);
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_NONALPHA])
	{
		FAILED_CHECK_RETURN(pGameObject->Render_ShadowDepth(), E_FAIL);
	}
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_CascadedShadow")), E_FAIL);

	// Setup origin Viewports
	CGraphic_Device::GetInstance()->Set_Viewport();

	return S_OK;
}

HRESULT CRenderer::Render_Volume()
{
	/* 1 pass - Render to Front */
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Volume_Front"), true, true);
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_VOLUME])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_VOLUME_FRONT), E_FAIL);;
	}
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Volume_Front"));

	/* 2 pass - Render to Back */
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Volume_Back"), true, true);
	for (auto& pGameObject : m_RenderObjects[RENDER_GROUP::RENDER_VOLUME])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(RENDER_GROUP::RENDER_VOLUME_BACK), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_GROUP::RENDER_VOLUME].clear();
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Volume_Back"));

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	NULL_CHECK_RETURN(m_pRenderTarget_Manager, E_FAIL);

	CLight_Manager*	pLight_Manager = CLight_Manager::GetInstance();

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc")), E_FAIL);
	FAILED_CHECK_RETURN(pLight_Manager->Render_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_LightAcc")), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	CBlur* pBlur = CBlur::GetInstance();
	FAILED_CHECK_RETURN(pBlur->Blur_Emissive(), E_FAIL);
	FAILED_CHECK_RETURN(pBlur->Blur_Specular(), E_FAIL);

	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_PostFX"), false);

	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Diffuse")));
	m_pVIBuffer->Set_ShaderResourceView("g_ShadeTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Shade")));
	m_pVIBuffer->Set_ShaderResourceView("g_ShadowTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Shadow")));
	
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Specular")));
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularBlurTexture", pBlur->Get_ShaderResourceView_BlurSpecular());

	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Emissive")));
	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveBlurTexture", pBlur->Get_ShaderResourceView_BlurEmissive());

	m_pVIBuffer->Render(0);
	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_PostFX"));

	return S_OK;
}

HRESULT CRenderer::PostProcessing(_double TimeDelta)
{
	CPostFX* pHDR = CPostFX::GetInstance();
	FAILED_CHECK_RETURN(pHDR->PostProcessing(TimeDelta),E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Compute_SSAO()
{
	FAILED_CHECK_RETURN(CSSAO::GetInstance()->Compute_SSAO(),E_FAIL);

	return S_OK;
}

void CRenderer::Sort_GameObjects(RENDER_OBJECTS & GameObjects)
{
	GameObjects.sort([](CGameObject* pFirst, CGameObject* pSecond) { return pFirst->Get_DistanceFromCamera() > pSecond->Get_DistanceFromCamera(); });
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone_Component(void * pArg)
{
	Safe_AddRef(this);

	return this;
}

void CRenderer::Free()
{
	for (_uint iRenderGroupIndex = 0; iRenderGroupIndex < RENDER_GROUP::RENDER_END; ++iRenderGroupIndex)
	{
		for (auto& pGameObject : m_RenderObjects[iRenderGroupIndex])
			Safe_Release(pGameObject);
		m_RenderObjects[iRenderGroupIndex].clear();
	}

	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pVIBuffer);

	CComponent::Free();
}
