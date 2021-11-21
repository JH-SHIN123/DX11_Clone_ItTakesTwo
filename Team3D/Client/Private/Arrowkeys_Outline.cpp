#include "stdafx.h"
#include "..\Public\Arrowkeys_Outline.h"

#include "GameInstance.h"

CArrowkeys_Outline::CArrowkeys_Outline(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CArrowkeys_Outline::CArrowkeys_Outline(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CArrowkeys_Outline::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CArrowkeys_Outline::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CArrowkeys_Outline::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CArrowkeys_Outline::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CArrowkeys_Outline::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CArrowkeys_Outline::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CArrowkeys_Outline::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CArrowkeys_Outline * CArrowkeys_Outline::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CArrowkeys_Outline* pInstance = new CArrowkeys_Outline(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Arrowkeys_Outline Prototype, Error to CArrowkeys_Outline::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CArrowkeys_Outline::Clone_GameObject(void * pArg)
{
	CArrowkeys_Outline* pClone = new CArrowkeys_Outline(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CArrowkeys_Outline, Error to CArrowkeys_Outline::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CArrowkeys_Outline::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
