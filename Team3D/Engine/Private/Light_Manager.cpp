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

CLight* CLight_Manager::Get_Light(LightStatus::Enum eState, const _tchar* pLightTag, _bool bAddRef)
{
	CLight* pLight = nullptr;

	if (LightStatus::eDIRECTIONAL == eState)
	{
		pLight = m_pDirectionalLight;
	}
	else
	{
		NULL_CHECK_RETURN(pLightTag, nullptr);

		if (LightStatus::eSTATIC == eState)
		{
			for (auto iter = m_StaticLights.begin(); iter != m_StaticLights.end(); ++iter)
			{
				if (nullptr == (*iter)) continue;

				else if (false == lstrcmp((*iter)->Get_LightTag(), pLightTag))
				{
					pLight = (*iter);
					break;
				}
			}
		}
		else if (LightStatus::eDYNAMIC == eState)
		{
			for (auto iter = m_DynamicLights.begin(); iter != m_DynamicLights.end(); ++iter)
			{
				if (nullptr == (*iter)) continue;

				else if (false == lstrcmp((*iter)->Get_LightTag(), pLightTag))
				{
					pLight = (*iter);
					break;
				}
			}
		}
	}

	if (bAddRef) 
	{
#ifdef _DEBUG
		if (nullptr == pLight) MSG_BOX("Failed to Get_Light");
#endif
		Safe_AddRef(pLight);
	}

	return pLight;
}

HRESULT CLight_Manager::Ready_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	NULL_CHECK_RETURN(pDevice, E_FAIL);
	NULL_CHECK_RETURN(pDeviceContext, E_FAIL);

	m_pVIBuffer = CVIBuffer_RectRHW::Create(pDevice, pDeviceContext, 0.f, 0.f, fBufferWidth, fBufferHeight, TEXT("../Bin/ShaderFiles/Shader_LightAcc.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

_int CLight_Manager::Tick_LightManager(_double dTimeDelta)
{
	/* Tick Dynamic Light */
	_int iProgress = 0;
	CLight* pDeleteLight = nullptr;
	for (auto iter = m_DynamicLights.begin(); iter != m_DynamicLights.end();)
	{
		if (nullptr == (*iter)) {
			//++iter;
			continue;
		}

		iProgress = (*iter)->Tick_Light(dTimeDelta);

		if (EVENT_DEAD == iProgress)
		{
			pDeleteLight = (*iter);
			iter = m_DynamicLights.erase(iter);
			Safe_Release(pDeleteLight);
		}
		else ++iter;
	}

	return iProgress;
}

HRESULT CLight_Manager::Render_LightManager()
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
	vCamPosition = pPipeline->Get_MainCamPosition();
	fCamFar = pPipeline->Get_MainCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	ViewMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_MAIN);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_fMainCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vMainCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_MainProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_MainViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	/* For.SubView */
	vCamPosition = pPipeline->Get_SubCamPosition();
	fCamFar = pPipeline->Get_SubCamFar();
	ProjMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	ViewMatrixInverse = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	vViewportUVInfo = pGraphicDevice->Get_ViewportUVInfo(CGraphic_Device::VP_SUB);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_fSubCamFar", &fCamFar, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vSubCamPosition", &vCamPosition, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_SubProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_SubViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_matrix)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVIBuffer->Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4)), E_FAIL);

	/* Render Directional Light */
	if(m_pDirectionalLight)  FAILED_CHECK_RETURN(m_pDirectionalLight->Render_Light(m_pVIBuffer), E_FAIL);

	/* Render Static Light */
	for (auto& pLight : m_StaticLights)
		FAILED_CHECK_RETURN(pLight->Render_Light(m_pVIBuffer), E_FAIL);

	/* Render Dynamic Light */
	for (auto& pLight : m_DynamicLights)
		FAILED_CHECK_RETURN(pLight->Render_Light(m_pVIBuffer), E_FAIL);

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(LightStatus::Enum eState, CLight* pLight)
{
	NULL_CHECK_RETURN(pLight, E_FAIL);

	switch (eState)
	{
	case LightStatus::eDIRECTIONAL:
		if (nullptr == m_pDirectionalLight)
		{
			m_pDirectionalLight = pLight;
		}
		else
		{
			Safe_Release(m_pDirectionalLight);
			m_pDirectionalLight = pLight;
		}
		break;
	case LightStatus::eSTATIC:
		m_StaticLights.push_back(pLight);
		break;
	case LightStatus::eDYNAMIC:
		m_DynamicLights.push_back(pLight);
		break;
	}

	return S_OK;
}

HRESULT CLight_Manager::Remove_Light(const _tchar* pLightTag)
{
	if (nullptr == pLightTag) return E_FAIL;

	for (auto iter = m_DynamicLights.begin(); iter != m_DynamicLights.end(); ++iter)
	{
		if (nullptr == (*iter)) continue;

		else if (false == lstrcmp((*iter)->Get_LightTag(), pLightTag))
		{
			Safe_Release(*iter);
			m_DynamicLights.erase(iter);
			return S_OK;
		}
	}

#ifdef _DEBUG
	MSG_BOX("Failed to Remove Light - Wrong Tag");
#endif

	return S_OK;
}

void CLight_Manager::Clear_Lights(LightStatus::Enum eState)
{
	switch (eState)
	{
	case LightStatus::eDIRECTIONAL:
		Safe_Release(m_pDirectionalLight);
		break;
	case LightStatus::eSTATIC:
		for (auto& pLight : m_StaticLights)
			Safe_Release(pLight);
		m_StaticLights.clear();
		break;
	case LightStatus::eDYNAMIC:
		for (auto& pLight : m_DynamicLights)
			Safe_Release(pLight);
		m_DynamicLights.clear();
		break;
	}
}

void CLight_Manager::Free()
{
}

void CLight_Manager::Clear_All()
{
	for (_uint i = 0; i < LightStatus::eEND; ++i)
	{
		Clear_Lights(LightStatus::Enum(i));
	}

	Safe_Release(m_pVIBuffer);
}