#include "stdafx.h"
#include "..\Public\Gauge_Circle.h"

#include "GameInstance.h"

CGauge_Circle::CGauge_Circle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CGauge_Circle::CGauge_Circle(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CGauge_Circle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGauge_Circle::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CGauge_Circle::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CGauge_Circle::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CGauge_Circle::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CGauge_Circle::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CGauge_Circle::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CGauge_Circle * CGauge_Circle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGauge_Circle* pInstance = new CGauge_Circle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Gauge_Circle Prototype, Error to CGauge_Circle::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGauge_Circle::Clone_GameObject(void * pArg)
{
	CGauge_Circle* pClone = new CGauge_Circle(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CGauge_Circle, Error to CGauge_Circle::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGauge_Circle::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
