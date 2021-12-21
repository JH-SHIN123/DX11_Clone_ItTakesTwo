#include "stdafx.h"
#include "..\Public\BossHpBarFrame.h"

CBossHpBarFrame::CBossHpBarFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CBossHpBarFrame::CBossHpBarFrame(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CBossHpBarFrame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBossHpBarFrame::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CBossHpBarFrame::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CBossHpBarFrame::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	if (true == m_IsActive)
	{
		if (m_fScaleX <= m_UIDesc.vScale.x)
		{
			m_fScaleX += (_float)TimeDelta * 1200.f;

			if (m_fScaleX >= m_UIDesc.vScale.x)
				m_fScaleX = m_UIDesc.vScale.x;

			m_pTransformCom->Set_Scale(XMVectorSet(m_fScaleX, m_UIDesc.vScale.y, 0.f, 0.f));
		}
	}

	return _int();
}

_int CBossHpBarFrame::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CBossHpBarFrame::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(19);

	return S_OK;
}

void CBossHpBarFrame::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;

	if (m_IsActive == false)
	{
		m_fScaleX = 1.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_fScaleX, m_UIDesc.vScale.y, 0.f, 0.f));
	}
}


HRESULT CBossHpBarFrame::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CBossHpBarFrame * CBossHpBarFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CBossHpBarFrame* pInstance = new CBossHpBarFrame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create BossHpBarFrame Prototype, Error to CBossHpBarFrame::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossHpBarFrame::Clone_GameObject(void * pArg)
{
	CBossHpBarFrame* pClone = new CBossHpBarFrame(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CBossHpBarFrame, Error to CBossHpBarFrame::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBossHpBarFrame::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
