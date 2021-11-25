#include "stdafx.h"
#include "..\Public\ControllerIcon.h"

#include "GameInstance.h"
#include "Pipeline.h"

CControllerIcon::CControllerIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CControllerIcon::CControllerIcon(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CControllerIcon::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CControllerIcon::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CControllerIcon::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CControllerIcon::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CControllerIcon::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CControllerIcon::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	return S_OK;
}

HRESULT CControllerIcon::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CControllerIcon * CControllerIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CControllerIcon* pInstance = new CControllerIcon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create ControllerIcon Prototype, Error to CControllerIcon::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CControllerIcon::Clone_GameObject(void * pArg)
{
	CControllerIcon* pClone = new CControllerIcon(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CControllerIcon, Error to CControllerIcon::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CControllerIcon::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
