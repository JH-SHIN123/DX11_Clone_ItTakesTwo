#include "..\public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_RectRHW.h"
#include "Graphic_Device.h"
#include "Pipeline.h"
#include "RenderTarget_Manager.h"
#include "Shadow_Manager.h"
#include "SSAO.h"
#include "Blur.h"

IMPLEMENT_SINGLETON(CLight_Manager)

LIGHT_DESC * CLight_Manager::Get_LightDescPtr(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return nullptr;

	const auto& iter = find_if(m_Lights.begin(), m_Lights.end(), CTagFinder(pLightTag));
	if (iter == m_Lights.end()) return nullptr;

	return iter->second->Get_LightDescPtr();
}

HRESULT CLight_Manager::TurnOn_Light(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return E_FAIL;

	const auto& iter = find_if(m_Lights.begin(), m_Lights.end(), CTagFinder(pLightTag));
	if (iter == m_Lights.end()) return E_FAIL;

	iter->second->TurnOn_Light();

	return S_OK;
}

HRESULT CLight_Manager::TurnOff_Light(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return E_FAIL;

	const auto& iter = find_if(m_Lights.begin(), m_Lights.end(), CTagFinder(pLightTag));
	if (iter == m_Lights.end()) return E_FAIL;

	iter->second->TurnOff_Light();

	return S_OK;
}

HRESULT CLight_Manager::Ready_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	m_pVIBuffer = CVIBuffer_RectRHW::Create(pDevice, pDeviceContext, 0.f, 0.f, fBufferWidth, fBufferHeight, TEXT("../Bin/ShaderFiles/Shader_LightAcc.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

HRESULT CLight_Manager::Reserve_Container(_uint iCount)
{
	m_Lights.reserve(iCount);

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const _tchar* pLightTag, const LIGHT_DESC & LightDesc, _bool isActive)
{
	if (nullptr == pLightTag) return E_FAIL;

	const auto& iter = find_if(m_Lights.begin(), m_Lights.end(), CTagFinder(pLightTag));
	if (iter != m_Lights.end()) {
		MSG_BOX("Already Exist Light");
		return E_FAIL;
	}

	CLight* pLight = nullptr;
	if (LIGHT_DESC::TYPE_DIRECTIONAL == LightDesc.eType)
	{
		if (nullptr == m_pDirectionalLight) 
		{
			pLight = CLight::Create(LightDesc, isActive);
			m_pDirectionalLight = pLight;
			Safe_AddRef(m_pDirectionalLight);
		}
	}
	else
	{
		pLight = CLight::Create(LightDesc, isActive);
	}

	NULL_CHECK_RETURN(pLight, E_FAIL);

	//m_Lights.emplace(pLightTag, pLight);

	m_Lights[pLightTag] = pLight;

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights()
{
	ID3D11ShaderResourceView* pNormalShaderResourceView = CRenderTarget_Manager::GetInstance()->Get_ShaderResourceView(TEXT("Target_Normal"));
	NULL_CHECK_RETURN(pNormalShaderResourceView, E_FAIL);
	ID3D11ShaderResourceView* pDepthShaderResourceView = CRenderTarget_Manager::GetInstance()->Get_ShaderResourceView(TEXT("Target_Depth"));
	NULL_CHECK_RETURN(pDepthShaderResourceView, E_FAIL);
	ID3D11ShaderResourceView* pSpecSrcShaderResourceView = CRenderTarget_Manager::GetInstance()->Get_ShaderResourceView(TEXT("Target_Specular_Src"));
	NULL_CHECK_RETURN(pNormalShaderResourceView, E_FAIL);

	FAILED_CHECK_RETURN(m_pVIBuffer->Set_ShaderResourceView("g_NormalTexture", pNormalShaderResourceView), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", pDepthShaderResourceView), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_ShaderResourceView("g_SpecularSrcTexture", pSpecSrcShaderResourceView), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_ShaderResourceView("g_SSAOTexture", CSSAO::GetInstance()->Get_ShaderResourceView()), E_FAIL);

	_float	fCamFar;
	_vector vCamPosition;
	_matrix	ProjMatrixInverse;
	_matrix ViewMatrixInverse;
	_float4	vViewportUVInfo;

	CGraphic_Device* pGraphicDevice = CGraphic_Device::GetInstance();
	CPipeline* pPipeline = CPipeline::GetInstance();

	/* For.MainView */
	vCamPosition		= pPipeline->Get_MainCamPosition();
	fCamFar				= pPipeline->Get_MainCamFar();
	ProjMatrixInverse	= pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	ViewMatrixInverse	= pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	vViewportUVInfo		= pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_MAIN);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_fMainCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vMainCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_MainProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_MainViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);
	
	/* For.SubView */
	vCamPosition		= pPipeline->Get_SubCamPosition();
	fCamFar				= pPipeline->Get_SubCamFar();
	ProjMatrixInverse	= pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	ViewMatrixInverse	= pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	vViewportUVInfo		= pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_SUB);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_fSubCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vSubCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_SubProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_SubViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	for (auto& pLight : m_Lights)
		FAILED_CHECK_RETURN(pLight.second->Render_Light(m_pVIBuffer), E_FAIL);

	return S_OK;
}

void CLight_Manager::Clear_Lights()
{
	Safe_Release(m_pDirectionalLight);

	for (auto& pLight : m_Lights)
		Safe_Release(pLight.second);
	m_Lights.clear();
}

void CLight_Manager::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer);
}

void CLight_Manager::Free()
{
	Safe_Release(m_pVIBuffer);

	Clear_Lights();
}
