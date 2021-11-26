#include "..\public\Light.h"
#include "VIBuffer_RectRHW.h"
#include "RenderTarget_Manager.h"

HRESULT CLight::NativeConstruct(const LIGHT_DESC & LightDesc, _bool isActive)
{
	m_LightDesc = LightDesc;
	m_isActive	= isActive;

	return S_OK;
}

HRESULT CLight::Render_Light(CVIBuffer_RectRHW * pVIBuffer)
{
	NULL_CHECK_RETURN(pVIBuffer, E_FAIL);

	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)), E_FAIL);

	_uint iPassIndex = 0;

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 0;
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightDir", &m_LightDesc.vDirection, sizeof(XMFLOAT3)), E_FAIL);
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		iPassIndex = 1;
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightPos", &m_LightDesc.vPosition, sizeof(XMFLOAT3)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fRange", &m_LightDesc.fRange, sizeof(_float)), E_FAIL);
	}
	//else if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
	//{
	//	iPassIndex = 2;
	//	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightPos", &m_LightDesc.vPosition, sizeof(XMFLOAT3)), E_FAIL);
	//	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightDir", &m_LightDesc.vDirection, sizeof(XMFLOAT3)), E_FAIL);
	//	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fRange", &m_LightDesc.fRange, sizeof(_float)), E_FAIL);
	//	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fAngleOuterCone", &m_LightDesc.fAngleOuterCone, sizeof(_float)), E_FAIL);
	//	FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fAngleInnerCone", &m_LightDesc.fAngleInnerCone, sizeof(_float)), E_FAIL);
	//}

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}

CLight * CLight::Create(const LIGHT_DESC & LightDesc, _bool isActive)
{
	CLight* pInstance = new CLight;

	if (FAILED(pInstance->NativeConstruct(LightDesc, isActive)))
	{
		MSG_BOX("Failed to Create Instance - CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}
