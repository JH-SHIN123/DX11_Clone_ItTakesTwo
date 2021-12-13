#include "stdafx.h"
#include "..\Public\PixelHeart.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"

CPixelHeart::CPixelHeart(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelHeart::CPixelHeart(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelHeart::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelHeart::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelHeart"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}
	return S_OK;
}

_int CPixelHeart::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return _int();
}

_int CPixelHeart::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CPixelHeart::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

CPixelHeart * CPixelHeart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelHeart* pInstance = new CPixelHeart(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelHeart Prototype, Error to CPixelHeart::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelHeart::Clone_GameObject(void * pArg)
{
	CPixelHeart* pClone = new CPixelHeart(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelHeart, Error to CPixelHeart::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelHeart::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
