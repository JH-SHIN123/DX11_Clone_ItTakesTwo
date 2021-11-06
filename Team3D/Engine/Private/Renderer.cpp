#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Light_Manager.h"
#include "RenderTarget_Manager.h"
#include "VIBuffer_RectRHW.h"
#include "Graphic_Device.h"
#include "Transform.h"

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
	_uint iHeight	= (_uint)ViewportDesc.Height;

	/* Target_Diffuse */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_SNORM, _float4(1.f, 1.f, 1.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Diffuse"), TEXT("MRT_Deferred")), E_FAIL);
	/* Target_Normal */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Normal"), TEXT("MRT_Deferred")), E_FAIL);
	/* Target_Depth */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Depth"), TEXT("MRT_Deferred")), E_FAIL);

	/* Target_Shade */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Shade"), TEXT("MRT_LightAcc")), E_FAIL);
	/* Target_Specular */
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_Specular"), TEXT("MRT_LightAcc")), E_FAIL);


	/* Target_DOF*/
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DOF"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_DOF"), TEXT("MRT_DOF")), E_FAIL);


	/* Target_ShadowMap*/
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ShadowMap"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Add_MRT(TEXT("Target_ShadowMap"), TEXT("MRT_ShadowMap")), E_FAIL);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_Blend.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	m_pDOFBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width / 2.f, ViewportDesc.Height / 2.f, TEXT("../Bin/ShaderFiles/Shader_DOF.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pDOFBuffer, E_FAIL);

	m_pShadowMapBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_Shadow.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pShadowMapBuffer, E_FAIL);


#ifdef _DEBUG
	_float fWidth	= ViewportDesc.Width / 10.f;
	_float fHeight	= ViewportDesc.Height / 10.f;

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Diffuse"), 0.f, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Normal"), 0.f, fHeight, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Depth"), 0.f, fHeight * 2.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Shade"), fWidth, 0.f, fWidth, fHeight), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_Specular"), fWidth, fHeight, fWidth, fHeight), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Ready_DebugBuffer(TEXT("Target_ShadowMap"), fWidth, fHeight, fWidth, fHeight), E_FAIL);
#endif

	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CRenderer::Add_GameObject_ToRenderGroup(RENDER_GROUP eGroup, CGameObject * pGameObject)
{
	if (eGroup >= RENDER_END) { return E_FAIL; }
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_RenderObjects[eGroup].emplace_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_Renderer()
{
	FAILED_CHECK_RETURN(Render_Priority(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonAlpha(), E_FAIL);
	FAILED_CHECK_RETURN(Render_ShadowMap(), E_FAIL);
	FAILED_CHECK_RETURN(Render_LightAcc(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Blend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_DOF(), E_FAIL);
	FAILED_CHECK_RETURN(Render_Alpha(), E_FAIL);
	FAILED_CHECK_RETURN(Render_UI(), E_FAIL);

#ifdef _DEBUG
	m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"));
	m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"));
	m_pRenderTarget_Manager->Render_DebugBuffer(TEXT("MRT_ShadowMap"));
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"));

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_NONALPHA].clear();

	m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_Deferred"));

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	Sort_GameObjects(m_RenderObjects[RENDER_ALPHA]);

	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHA])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		FAILED_CHECK_RETURN(pGameObject->Render(), E_FAIL);
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI].clear();

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

	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Diffuse")));
	m_pVIBuffer->Set_ShaderResourceView("g_ShadeTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Shade")));
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Specular")));
	m_pVIBuffer->Set_ShaderResourceView("g_DOFTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_DOF")));
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Depth")));

	m_pVIBuffer->Render(0);

	return S_OK;
}

HRESULT CRenderer::Render_ShadowMap()
{
	//  카메라위치를 광원위치로 바꿔치기 하고 
	// -> 그 위치에서 바라본 오브젝트들의 깊이값을 쉐도우맵 텍스쳐에 그린다. ( 오브젝트들이 Shader_Mesh.hlsl 써서 렌더하고 있는데 
	// -> Shader_Shadow.hlsl 써서 깊이만 렌더하게 할 수 있는지. )
	// -> 그 쉐도우맵 텍스쳐랑 디퍼드에서 뽑아낸 Depth 텍스쳐의 깊이 값을 비교 해서 그림자 그릴지 말지를 판별!
	/////////////////////////////////////////////////////////////////////
	// 쉐도우맵 렌더타겟에 광원의 위치에서 본 오브젝트들의 깊이값을 그려라! //
	////////////////////////////////////////////////////////////////////
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_ShadowMap")), E_FAIL);
	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOWTARGET])
	{
		_matrix			WorldMatrix, LightViewMatrix, LightProjMatrix = XMMatrixIdentity();

		WorldMatrix = ((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
		_vector vLightPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		LightViewMatrix = XMMatrixLookAtLH(vLightPos, XMVectorSet(-1.f, 1.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
		LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), 1.f, 0.2f, 300.f);

		m_pShadowMapBuffer->Set_Variable("WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pShadowMapBuffer->Set_Variable("LightPos", &vLightPos, sizeof(_vector));
		m_pShadowMapBuffer->Set_Variable("LightViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
		m_pShadowMapBuffer->Set_Variable("LightProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

		// 렌더 ?????????????????????????????????????

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_SHADOWTARGET].clear();
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_ShadowMap")), E_FAIL);
	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DOF")), E_FAIL);

	m_pDOFBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pRenderTarget_Manager->Get_ShaderResourceView(TEXT("Target_Diffuse")));
	m_pDOFBuffer->Render(0);

	FAILED_CHECK_RETURN(m_pRenderTarget_Manager->End_MRT(m_pDeviceContext, TEXT("MRT_DOF")), E_FAIL);
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
	for (_uint iRenderGroupIndex = 0; iRenderGroupIndex < RENDER_END; ++iRenderGroupIndex)
	{
		for (auto& pGameObject : m_RenderObjects[iRenderGroupIndex])
			Safe_Release(pGameObject);
		m_RenderObjects[iRenderGroupIndex].clear();
	}

	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pDOFBuffer);
	Safe_Release(m_pVIBuffer);

	CComponent::Free();
}
