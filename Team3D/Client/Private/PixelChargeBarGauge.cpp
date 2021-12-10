#include "stdafx.h"
#include "..\Public\PixelChargeBarGauge.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"

CPixelChargeBarGauge::CPixelChargeBarGauge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelChargeBarGauge::CPixelChargeBarGauge(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelChargeBarGauge::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelChargeBarGauge::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelChargeBarGauge"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}
	return S_OK;
}

_int CPixelChargeBarGauge::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_pGameInstance->Key_Down(DIK_K))
	{
		m_fGauge += 0.5f;
	}

	if (m_fGauge > 0.f)
		m_fGauge -= TimeDelta;
	else if (m_fGauge < 0.f)
		m_fGauge = 0.f;

	return _int();
}

_int CPixelChargeBarGauge::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CPixelChargeBarGauge::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));
	m_pVIBufferCom->Set_Variable("g_fGauge", &m_fGauge, sizeof(_float));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(2);

	return S_OK;
}

CPixelChargeBarGauge * CPixelChargeBarGauge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelChargeBarGauge* pInstance = new CPixelChargeBarGauge(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelChargeBarGauge Prototype, Error to CPixelChargeBarGauge::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelChargeBarGauge::Clone_GameObject(void * pArg)
{
	CPixelChargeBarGauge* pClone = new CPixelChargeBarGauge(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelChargeBarGauge, Error to CPixelChargeBarGauge::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelChargeBarGauge::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
