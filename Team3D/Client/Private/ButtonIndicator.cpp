#include "stdafx.h"
#include "..\Public\ButtonIndicator.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CButtonIndicator::CButtonIndicator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CButtonIndicator::CButtonIndicator(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CButtonIndicator::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CButtonIndicator::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CButtonIndicator::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CButtonIndicator::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	Scale_Effect(TimeDelta);

	return _int();
}

_int CButtonIndicator::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CButtonIndicator::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

void CButtonIndicator::Scale_Effect(_double TimeDelta)
{
	m_Time += TimeDelta;

	_float fChange = 1.f;

	if (1 == m_iScaleChangeCount % 2)
		fChange *= -1.f;

	if (m_Time < 0.1)
	{
		m_UIDesc.vScale.x -= 5.f * fChange;
		m_UIDesc.vScale.y -= 5.f * fChange;
		m_UIDesc.vPos.y -= 1.5f * fChange;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f , 1.f));
	}
	else if (0.2 < m_Time)
	{
		m_Time = 0;
		++m_iScaleChangeCount;
	}
}

HRESULT CButtonIndicator::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CButtonIndicator * CButtonIndicator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CButtonIndicator* pInstance = new CButtonIndicator(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create ButtonIndicator Prototype, Error to CButtonIndicator::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CButtonIndicator::Clone_GameObject(void * pArg)
{
	CButtonIndicator* pClone = new CButtonIndicator(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CButtonIndicator, Error to CButtonIndicator::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CButtonIndicator::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
