#include "stdafx.h"
#include "..\Public\MinigameHpBar.h"

#include "MinigameHpBarFrame.h"
#include "UI_Generator.h"
#include "Portrait.h"

#include "Cody.h"
#include "May.h"

CMinigameHpBar::CMinigameHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigameHpBar::CMinigameHpBar(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigameHpBar::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigameHpBar::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigameHpBar::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	/* Option 0 : 메인 Hp바 / Option 1 : 서브 Hp바 */
	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_fMaxHp = 120.f;
	m_fHp = m_fMaxHp;

	return S_OK;
}

_int CMinigameHpBar::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	switch (m_ePlayerID)
	{
	case Player::Cody:
		CodyMinigameHpBar(TimeDelta);
		break;
	case Player::May:
		MayMinigameHpBar(TimeDelta);
		break;
	}

	return _int();
}

_int CMinigameHpBar::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigameHpBar::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fCircleRatio", &m_fRatio, sizeof(_float));
	m_pVIBuffer_RectCom->Set_Variable("g_fDecreaseRateRatio", &m_fDecreaseRateRatio, sizeof(_float));
	m_pVIBuffer_RectCom->Set_Variable("g_iShaderOption", &m_iShaderOption, sizeof(_int));

	m_pVIBuffer_RectCom->Render(29);

	return S_OK;
}

void CMinigameHpBar::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;

	if(nullptr != m_pMinigamePortrait)
		m_pMinigamePortrait->Set_Active(IsCheck);

	if (nullptr != m_pMinigameHpBarFrame)
	{
		m_pMinigameHpBarFrame->Set_Active(IsCheck);
		m_pMinigameHpBarFrame->Set_PlayerID(m_ePlayerID);
	}
}

void CMinigameHpBar::Set_Hp(_float fHp)
{
	m_fHp -= fHp;
	m_IsHit = true;
	m_fWatingTime = 0.f;

	if (m_ePlayerID == Player::Cody)
		m_fRatio = 0.5f - (m_fHp / m_fMaxHp) / 2.f;
	else if (m_ePlayerID == Player::May)
		m_fRatio = (m_fHp / m_fMaxHp) / 2.f;

	if (0.f >= m_fHp)
		m_fHp = 0.f;
}

void CMinigameHpBar::Set_ShaderOption(_int iOption)
{
	m_iShaderOption = iOption;
}

void CMinigameHpBar::Set_ResetHp()
{
	m_fHp = m_fMaxHp;

	if (m_ePlayerID == Player::Cody)
	{
		m_fRatio = 0.f;
		m_fDecreaseRateRatio = 0.f;
	}
	else
	{
		m_fRatio = 0.5f;
		m_fDecreaseRateRatio = 0.5f;
	}
}

HRESULT CMinigameHpBar::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CMinigameHpBar::Ready_Layer_UI()
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 0;

	if (0 == m_iOption)
	{
		if (!lstrcmp(m_UIDesc.szUITag, L"MinigameCodyHpBar"))
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameCodyHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pMinigameHpBarFrame = static_cast<CMinigameHpBarFrame*>(pGameObject);
			m_pMinigameHpBarFrame->Set_PlayerID(Player::Cody);

			iOption = 1;
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigamePortrait_Cody"), &iOption, &pGameObject), E_FAIL);
			m_pMinigamePortrait = static_cast<CPortrait*>(pGameObject);
			m_pMinigamePortrait->Set_PlayerID(Player::Cody);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameMayHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pMinigameHpBarFrame = static_cast<CMinigameHpBarFrame*>(pGameObject);
			m_pMinigameHpBarFrame->Set_PlayerID(Player::May);

			iOption = 1;
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigamePortrait_May"), &iOption, &pGameObject), E_FAIL);
			m_pMinigamePortrait = static_cast<CPortrait*>(pGameObject);
			m_pMinigamePortrait->Set_PlayerID(Player::May);

			m_fDecreaseRateRatio = 0.5f;
			m_fRatio = 0.5f;
		}
	}
	else
	{
		if (!lstrcmp(m_UIDesc.szUITag, L"MinigameCodySubHpBar"))
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameCodySubHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pMinigameHpBarFrame = static_cast<CMinigameHpBarFrame*>(pGameObject);
			m_pMinigameHpBarFrame->Set_PlayerID(Player::Cody);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MinigameMaySubHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pMinigameHpBarFrame = static_cast<CMinigameHpBarFrame*>(pGameObject);
			m_pMinigameHpBarFrame->Set_PlayerID(Player::May);

			m_fDecreaseRateRatio = 0.5f;
			m_fRatio = 0.5f;
		}
	}

	return S_OK;
}

void CMinigameHpBar::Shake_Effect(_double TimeDelta)
{
	if (2.5f >= m_fAngle && false == m_IsChangeRotate)
	{
		m_fAngle += (_float)TimeDelta * 150.f;
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fAngle));
	}
	else
		m_IsChangeRotate = true;
	
	if (0.f <= m_fAngle && true == m_IsChangeRotate)
	{
		m_fAngle -= (_float)TimeDelta * 150.f;
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fAngle));
	}
	else
		m_IsChangeRotate = false;
}

void CMinigameHpBar::CodyMinigameHpBar(_double TimeDelta)
{
	if (true == m_IsHit)
	{
		if (m_fDecreaseRateRatio <= m_fRatio)
		{
			Shake_Effect(TimeDelta);

			m_fWatingTime += (_float)TimeDelta;

			if (1.f <= m_fWatingTime)
			{
				m_fDecreaseRateRatio = m_fRatio;
				m_fWatingTime = 0.f;
				m_IsHit = false;
			}
		}
	}
	else if (false == m_IsHit)
	{
		if (1 == m_iOption)
		{
			m_IsActive = false;
			m_pMinigameHpBarFrame->Set_Active(false);
		}
	}
}

void CMinigameHpBar::MayMinigameHpBar(_double TimeDelta)
{
	if (true == m_IsHit)
	{
		if (m_fDecreaseRateRatio >= m_fRatio)
		{
			Shake_Effect(TimeDelta);

			m_fWatingTime += (_float)TimeDelta;

			if (1.f <= m_fWatingTime)
			{
				m_fDecreaseRateRatio = m_fRatio;
				m_fWatingTime = 0.f;
				m_IsHit = false;
			}
		}
	}
	else if (false == m_IsHit)
	{
		if (1 == m_iOption)
		{
			m_IsActive = false;
			m_pMinigameHpBarFrame->Set_Active(false);
		}
	}
}

CMinigameHpBar * CMinigameHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigameHpBar* pInstance = new CMinigameHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create MinigameHpBar Prototype, Error to CMinigameHpBar::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigameHpBar::Clone_GameObject(void * pArg)
{
	CMinigameHpBar* pClone = new CMinigameHpBar(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigameHpBar, Error to CMinigameHpBar::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigameHpBar::Free()
{
	Safe_Release(m_pMinigamePortrait);
	Safe_Release(m_pMinigameHpBarFrame);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
