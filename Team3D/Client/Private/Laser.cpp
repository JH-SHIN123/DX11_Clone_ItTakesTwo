#include "stdafx.h"
#include "..\Public\Laser.h"

CLaser::CLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLaser::CLaser(const CLaser & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLaser::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL); // 3¹ø
	
	m_pInstanceMatrices = new VTXMATRIX_CUSTOM_ST;
	ZeroMemory(m_pInstanceMatrices, sizeof(VTXMATRIX_CUSTOM_ST));

	m_pInstanceMatrices[0].vRight = _float4(1.f, 0.f, 0.f, 0.f);
	m_pInstanceMatrices[0].vUp = _float4(0.f, 1.f, 0.f, 0.f);
	m_pInstanceMatrices[0].vLook = _float4(0.f, 0.f, 1.f, 0.f);
	m_pInstanceMatrices[0].vPosition = _float4(64.f, 0.f, 30.f, 1.f);
	m_pInstanceMatrices[0].vSize = _float2(3.f, 10.f);
	m_pInstanceMatrices[0].vTextureUV = _float4(0.f, 0.f, 2.f, 1.f);

	m_pCircleMatrice = new VTXMATRIX_CUSTOM_ST;
	ZeroMemory(m_pCircleMatrice, sizeof(VTXMATRIX_CUSTOM_ST));

	m_pCircleMatrice[0].vRight = _float4(1.f, 0.f, 0.f, 0.f);
	m_pCircleMatrice[0].vUp = _float4(0.f, 1.f, 0.f, 0.f);
	m_pCircleMatrice[0].vLook = _float4(0.f, 0.f, 1.f, 0.f);
	m_pCircleMatrice[0].vPosition = _float4(64.f, 0.f, 30.f, 1.f);
	m_pCircleMatrice[0].vSize = _float2(6.f, 6.f);
	m_pCircleMatrice[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);

	return S_OK;
}

_int CLaser::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CLaser::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return NO_EVENT;
}

HRESULT CLaser::Render(RENDER_GROUP::Enum eRender)
{
	CGameObject::Render(eRender);

	return S_OK;
}

HRESULT CLaser::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	return S_OK;
}

void CLaser::Free()
{
	Safe_Delete_Array(m_pInstanceMatrices);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
