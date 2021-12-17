#include "stdafx.h"
#include "..\Public\BossHpBar.h"

#include "BossHpBarFrame.h"

CBossHpBar::CBossHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CBossHpBar::CBossHpBar(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CBossHpBar::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBossHpBar::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CBossHpBar::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CBossHpBar::Tick(_double TimeDelta)
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

_int CBossHpBar::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;

	if (m_fRatio < m_fDecreaseRateRatio)
	{
		m_fWaitingTime += (_float)TimeDelta;

		if(1.f <= m_fWaitingTime)
			m_fDecreaseRateRatio -= (_float)TimeDelta / 2.f;

		if(m_fRatio >= m_fDecreaseRateRatio)
			m_fDecreaseRateRatio = m_fRatio;
	}
	else
		m_fWaitingTime = 0.f;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CBossHpBar::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fRatio", &m_fRatio, sizeof(_float));
	m_pVIBuffer_RectCom->Set_Variable("g_fDecreaseRateRatio", &m_fDecreaseRateRatio, sizeof(_float));

	m_pVIBuffer_RectCom->Render(20);

	return S_OK;
}

void CBossHpBar::Set_Ratio(_float fRatio)
{
	m_fDecreaseRateRatio = m_fRatio;
	m_fRatio -= fRatio;
}

void CBossHpBar::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;
	m_pBossHpBarFrame->Set_Active(IsCheck);

	if (m_IsActive == false)
	{
		m_fScaleX = 1.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_fScaleX, m_UIDesc.vScale.y, 0.f, 0.f));
	}
}

HRESULT CBossHpBar::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CBossHpBar::Ready_Layer_UI()
{
	CGameObject* pGameObject = nullptr;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("BossHpBarFrame"), nullptr, &pGameObject), E_FAIL);
	m_pBossHpBarFrame = static_cast<CBossHpBarFrame*>(pGameObject);

	return S_OK;
}

CBossHpBar * CBossHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CBossHpBar* pInstance = new CBossHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create BossHpBar Prototype, Error to CBossHpBar::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossHpBar::Clone_GameObject(void * pArg)
{
	CBossHpBar* pClone = new CBossHpBar(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CBossHpBar, Error to CBossHpBar::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBossHpBar::Free()
{
	Safe_Release(m_pBossHpBarFrame);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
