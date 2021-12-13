#include "stdafx.h"
#include "..\Public\PixelLaser.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"

CPixelLaser::CPixelLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelLaser::CPixelLaser(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelLaser::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelLaser::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelLaser"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}
	return S_OK;
}

_int CPixelLaser::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return _int();
}

_int CPixelLaser::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CPixelLaser::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

CPixelLaser * CPixelLaser::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelLaser* pInstance = new CPixelLaser(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelLaser Prototype, Error to CPixelLaser::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelLaser::Clone_GameObject(void * pArg)
{
	CPixelLaser* pClone = new CPixelLaser(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelLaser, Error to CPixelLaser::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelLaser::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
