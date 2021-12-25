#include "..\public\Light.h"
#include "VIBuffer_RectRHW.h"
#include "RenderTarget_Manager.h"

HRESULT CLight::NativeConstruct(const _tchar* pLightTag, void* pArgs)
{
	lstrcpy(m_szLightTag, pLightTag);

	if (nullptr != pArgs)
		memcpy(&m_LightDesc, pArgs, sizeof(m_LightDesc));

	return S_OK;
}

_int CLight::Tick_Light(_double dTimeDelta)
{
	if (m_bDead) return EVENT_DEAD;

	return _int();
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
		if (0.f == m_LightDesc.fRange) return S_OK;

		iPassIndex = 1;
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightPos", &m_LightDesc.vPosition, sizeof(XMFLOAT3)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fRange", &m_LightDesc.fRange, sizeof(_float)), E_FAIL);
	}
	else if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
	{
		if (0.f == m_LightDesc.fRange) return S_OK;

		iPassIndex = 2;

		_float fCosInnerAngle = cosf(m_LightDesc.fInnerAngle);
		_float fSinOuterAngle = sinf(m_LightDesc.fOuterAngle);
		_float fCosOuterAngle = cosf(m_LightDesc.fOuterAngle);

		_float fSpotConeAttRange = fCosInnerAngle - fCosOuterAngle;
		_float fSpotRangeRcp = 1.f / m_LightDesc.fRange;

		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightPos", &m_LightDesc.vPosition, sizeof(XMFLOAT3)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_vLightDir", &m_LightDesc.vDirection, sizeof(XMFLOAT3)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fSpotCosOuterCone", &fCosOuterAngle, sizeof(_float)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fSpotConeAttRange", &fSpotConeAttRange, sizeof(_float)), E_FAIL);
		FAILED_CHECK_RETURN(pVIBuffer->Set_Variable("g_fSpotRangeRcp", &fSpotRangeRcp, sizeof(_float)), E_FAIL);
	}

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}

CLight * CLight::Create(const _tchar* pLightTag, void* pArgs)
{
	CLight* pInstance = new CLight;

	if (FAILED(pInstance->NativeConstruct(pLightTag, pArgs)))
	{
		MSG_BOX("Failed to Create Instance - CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}
