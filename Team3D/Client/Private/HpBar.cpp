#include "stdafx.h"
#include "..\Public\HpBar.h"

#include "HpBarFrame.h"
#include "UI_Generator.h"

CHpBar::CHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CHpBar::CHpBar(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CHpBar::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHpBar::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CHpBar::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));


	return S_OK;
}

_int CHpBar::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	if (m_pGameInstance->Key_Down(DIK_INSERT))
	{
		UI_Create(Cody, Minigame_Ready_Cody);
		UI_Create(May, Minigame_Ready_May);
		//UI_Create(Default, Minigame_Countdown);
	}
	else if (m_pGameInstance->Key_Down(DIK_J))
	{
		UI_Create(Default, Minigame_Score);
	}
	else if (m_pGameInstance->Key_Down(DIK_M))
	{
		UI_Create(Default, Minigame_Win_May);
	}

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		m_fHp += 10.f;
		m_fRatio = (m_fHp / 120.f) / 2.f;
		m_IsHit = true;
		m_fWatingTime = 0.f;
		m_fRecoveryTime = 0.f;
	}

	if (true == m_IsHit)
	{
		if (m_fDecreaseRateRatio <= m_fRatio)
		{
			m_fWatingTime += (_float)TimeDelta;

			if (1.f <= m_fWatingTime)
			{
				m_fDecreaseRateRatio = m_fRatio;
				m_fWatingTime = 0.f;
				m_IsHit = false;
			}
		}
	}
	else
	{
		m_fWatingTime += (_float)TimeDelta;

		if (1.f <= m_fWatingTime)
		{
			m_fRecoveryTime += (_float)TimeDelta;

			if (0.02f <= m_fRecoveryTime)
			{
				m_fHp -= 10.f;

				if (0 >= m_fHp)
					m_fHp = 0.f;

				m_fRatio = (m_fHp / 120.f) / 2.f;
				m_fDecreaseRateRatio = m_fRatio;
				m_fRecoveryTime = 0.f;
			}
		}
	}

	return _int();
}

_int CHpBar::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CHpBar::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fCircleRatio", &m_fRatio, sizeof(_float));
	m_pVIBuffer_RectCom->Set_Variable("g_fDecreaseRateRatio", &m_fDecreaseRateRatio, sizeof(_float));
	
	m_pVIBuffer_RectCom->Render(22);

	return S_OK;
}

void CHpBar::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;
	UI_CreateOnlyOnce(Cody, Portrait_Cody);
	UI_CreateOnlyOnce(May, Portrait_May);

	if (nullptr != m_pHpBarFrame)
	{
		m_pHpBarFrame->Set_Active(IsCheck);
		m_pHpBarFrame->Set_PlayerID(m_ePlayerID);
	}

	if (false == m_IsActive)
	{
		UI_Delete(Cody, Portrait_Cody);
		UI_Delete(May, Portrait_May);
	}
}

void CHpBar::Set_Hp(_float fHp)
{
	m_fHp += fHp;
	m_fRatio = (m_fHp / 120.f) / 2.f;
	m_IsHit = true;
	m_fWatingTime = 0.f;
	m_fRecoveryTime = 0.f;
}

HRESULT CHpBar::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CHpBar::Ready_Layer_UI()
{
	CGameObject* pGameObject = nullptr;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("CodyHpBarFrame"), nullptr, &pGameObject), E_FAIL);
	m_pHpBarFrame = static_cast<CHpBarFrame*>(pGameObject);
	m_pHpBarFrame->Set_PlayerID(m_ePlayerID);

	return S_OK;
}

CHpBar * CHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHpBar* pInstance = new CHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create HpBar Prototype, Error to CHpBar::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpBar::Clone_GameObject(void * pArg)
{
	CHpBar* pClone = new CHpBar(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CHpBar, Error to CHpBar::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CHpBar::Free()
{
	Safe_Release(m_pHpBarFrame);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
