#include "stdafx.h"
#include "..\Public\Arrowkeys_Fill.h"

CArrowkeys_Fill::CArrowkeys_Fill(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CArrowkeys_Fill::CArrowkeys_Fill(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CArrowkeys_Fill::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CArrowkeys_Fill::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CArrowkeys_Fill::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CArrowkeys_Fill::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	UpDown(TimeDelta);

	return _int();
}

_int CArrowkeys_Fill::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CArrowkeys_Fill::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(3);

	return S_OK;
}

void CArrowkeys_Fill::UpDown(_double TimeDelta)
{
	m_UpDownTime += TimeDelta;

	_float fChange = 1.f;

	if (1 == m_iUpDownCount % 2)
		fChange *= -1.f;

	if (m_UpDownTime < 0.1)
	{
		m_UIDesc.vPos.y -= 0.5f * fChange;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	}
	else if (0.5 < m_UpDownTime)
	{
		m_UpDownTime = 0;
		++m_iUpDownCount;
	}
}

HRESULT CArrowkeys_Fill::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CArrowkeys_Fill * CArrowkeys_Fill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CArrowkeys_Fill* pInstance = new CArrowkeys_Fill(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Arrowkeys_Fill Prototype, Error to CArrowkeys_Fill::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CArrowkeys_Fill::Clone_GameObject(void * pArg)
{
	CArrowkeys_Fill* pClone = new CArrowkeys_Fill(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CArrowkeys_Fill, Error to CArrowkeys_Fill::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CArrowkeys_Fill::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
