#include "stdafx.h"
#include "..\Public\PC_Enter.h"

#include "UI_Generator.h"

CPC_Enter::CPC_Enter(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CPC_Enter::CPC_Enter(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CPC_Enter::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPC_Enter::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CPC_Enter::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y , 0.f, 0.f));

	return S_OK;
}

_int CPC_Enter::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CPC_Enter::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CPC_Enter::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	return S_OK;
}


HRESULT CPC_Enter::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CPC_Enter * CPC_Enter::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPC_Enter* pInstance = new CPC_Enter(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create PC_Enter Prototype, Error to CPC_Enter::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPC_Enter::Clone_GameObject(void * pArg)
{
	CPC_Enter* pClone = new CPC_Enter(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPC_Enter, Error to CPC_Enter::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPC_Enter::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
