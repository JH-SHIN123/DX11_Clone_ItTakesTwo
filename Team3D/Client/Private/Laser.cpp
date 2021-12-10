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

	m_vStartPoint = _float4(0.f, 0.f, 0.f, 1.f);
	m_vEndPoint = _float4(0.f, 0.f, 0.f, 1.f);
	m_vLaserDir = _float4(0.f, 0.f, 1.f, 0.f);
	m_fLaserSizeX = 0.f;
	m_fLaserSizeY = 0.f;
	m_fLaserMaxY = 0.f;
	m_fShootSpeed = 0.f;
	m_fOutsideAlpha = 0.f;
	m_isCollided = false;

	m_isHitCody = false;
	m_isHitMay = false;
	m_dDamagingDelay_Cody = 0.0;
	m_dDamagingDelay_May = 0.0;
	m_dCreateEffectCycle = 0.1;
	m_dCreateEffectDelay = 0.1;

	m_pLaserMatrices = new VTXMATRIX_CUSTOM_ST[4];
	ZeroMemory(m_pLaserMatrices, sizeof(VTXMATRIX_CUSTOM_ST));

	m_pLaserMatrices[0].vUp = m_vLaserDir;
	m_pLaserMatrices[0].vPosition = m_vStartPoint;
	m_pLaserMatrices[0].vSize = _float2(m_fLaserSizeX, m_fLaserSizeY);
	m_pLaserMatrices[0].vTextureUV = _float4(0.f, 0.f, 2.f, 1.f);

	m_pLaserMatrices[1].vUp = m_vLaserDir;
	m_pLaserMatrices[1].vPosition = m_vStartPoint;
	m_pLaserMatrices[1].vSize = _float2(m_fLaserSizeX, m_fLaserSizeX);
	m_pLaserMatrices[1].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);

	m_pLaserMatrices[2].vUp = m_vLaserDir;
	m_pLaserMatrices[2].vPosition = m_vStartPoint;
	m_pLaserMatrices[2].vSize = _float2(m_fLaserSizeX, m_fLaserSizeX);
	m_pLaserMatrices[2].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);

	m_pLaserMatrices[3].vUp = m_vLaserDir;
	m_pLaserMatrices[3].vPosition = m_vStartPoint;
	m_pLaserMatrices[3].vSize = _float2(m_fLaserSizeX * 2.f, m_fLaserSizeY);
	m_pLaserMatrices[3].vTextureUV = _float4(0.5f, 0.f, 1.5f, 1.f);

	return S_OK;
}

_int CLaser::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CLaser::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (m_dCreateEffectDelay > 0.0)
		m_dCreateEffectDelay -= dTimeDelta;

	if (m_dDamagingDelay_Cody > 0.0)
		m_dDamagingDelay_Cody -= dTimeDelta;

	if (m_dDamagingDelay_May > 0.0)
		m_dDamagingDelay_May -= dTimeDelta;

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

void CLaser::Set_Dead()
{
	m_isDead = true;
}


void CLaser::Set_LaserMatices()
{
	m_pLaserMatrices[0].vUp = m_vLaserDir;
	m_pLaserMatrices[0].vPosition = m_vStartPoint;
	m_pLaserMatrices[0].vSize = _float2(m_fLaserSizeX, m_fLaserSizeY);

	m_pLaserMatrices[1].vUp = m_vLaserDir;
	m_pLaserMatrices[1].vPosition = m_vStartPoint;
	m_pLaserMatrices[1].vSize = _float2(m_fLaserSizeX, m_fLaserSizeX);

	m_pLaserMatrices[2].vUp = m_vLaserDir;
	m_pLaserMatrices[2].vPosition = m_vEndPoint;
	m_pLaserMatrices[2].vSize = _float2(m_fLaserSizeX, m_fLaserSizeX);

	m_pLaserMatrices[3].vUp = m_vLaserDir;
	m_pLaserMatrices[3].vPosition = m_vStartPoint;
	m_pLaserMatrices[3].vSize = _float2(m_fLaserSizeX * 2.f, m_fLaserSizeY);
}

void CLaser::Adjust_OutsideAlpha(_double dTimeDelta)
{
	m_fSineGrpah += (_float)dTimeDelta * 100.f;

	if (360.f <= m_fSineGrpah)
		m_fSineGrpah -= 360.f;

	m_fOutsideAlpha = (sinf(XMConvertToRadians(m_fSineGrpah)) + 1.f) * 0.35f;
	m_fOutsideAlpha = powf(m_fOutsideAlpha, 3);
}

void CLaser::Free()
{
	Safe_Delete_Array(m_pLaserMatrices);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
