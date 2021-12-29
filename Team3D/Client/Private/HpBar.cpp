#include "stdafx.h"
#include "..\Public\HpBar.h"

#include "HpBarFrame.h"
#include "UI_Generator.h"
#include "Portrait.h"

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

	/* Option 0 : 메인 Hp바 / Option 1 : 서브 Hp바 */
	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_fMaxHp = 120.f;
	m_fHp = m_fMaxHp;

	D3D11_VIEWPORT MainViewport = m_pGameInstance->Get_ViewportInfo(1);
	D3D11_VIEWPORT SubViewPort = m_pGameInstance->Get_ViewportInfo(2);

	m_fSaveMainViewPortWidth = MainViewport.Width;
	m_fSaveSubViewPortWidth = SubViewPort.Width;

	return S_OK;
}

_int CHpBar::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	D3D11_VIEWPORT MainViewport = m_pGameInstance->Get_ViewportInfo(1);
	D3D11_VIEWPORT SubViewPort = m_pGameInstance->Get_ViewportInfo(2);

	if (m_fSaveMainViewPortWidth < MainViewport.Width)
	{
		_float fDecreaseWidth = fabs(m_fSaveMainViewPortWidth - MainViewport.Width);

		m_fSaveMainViewPortWidth += (_float)TimeDelta * fDecreaseWidth;
		m_UIDesc.vPos.x -= (_float)TimeDelta * fDecreaseWidth / 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));

		if(nullptr != m_pPortrait)
			m_pPortrait->Set_Position((_float)TimeDelta * fDecreaseWidth / 2.f);

		m_pHpBarFrame->Set_Position((_float)TimeDelta * fDecreaseWidth / 2.f);
	}

	switch (m_ePlayerID)
	{
	case Player::Cody:
		CodyHpBar_Boss(TimeDelta);
		break;
	case Player::May:
		MayHpBar_Boss(TimeDelta);
		break;
	}

	return _int();
}

_int CHpBar::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

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
	m_pVIBuffer_RectCom->Set_Variable("g_IsRecovery", &m_IsRecovery, sizeof(_bool));
	m_pVIBuffer_RectCom->Set_Variable("g_iShaderOption", &m_iShaderOption, sizeof(_int));

	m_pVIBuffer_RectCom->Render(22);

	return S_OK;
}

void CHpBar::Reset()
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

void CHpBar::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;
	UI_CreateOnlyOnce(Cody, Portrait_Cody);
	UI_CreateOnlyOnce(May, Portrait_May);

	if (nullptr != m_pPortrait)
		m_pPortrait->Set_Active(IsCheck);

	if (nullptr != m_pHpBarFrame)
	{
		m_pHpBarFrame->Set_Active(IsCheck);
		m_pHpBarFrame->Set_PlayerID(m_ePlayerID);
	}

	if (false == m_IsActive && 0 == m_iOption)
	{
		UI_Delete(Cody, Portrait_Cody);
		UI_Delete(May, Portrait_May);
	}
}

void CHpBar::Set_Hp(_float fHp)
{
	m_fHp -= fHp;
	m_IsHit = true;
	m_fWatingTime = 0.f;
	m_fRecoveryTime = 0.f;

	if (m_ePlayerID == Player::Cody)
		m_fRatio = 0.5f - (m_fHp / m_fMaxHp) / 2.f;
	else if (m_ePlayerID == Player::May)
		m_fRatio = (m_fHp / m_fMaxHp) / 2.f;

	if (0.f >= m_fHp)
		m_fHp = 0.f;
}

void CHpBar::Set_ShaderOption(_int iOption)
{
	m_iShaderOption = iOption;
}

void CHpBar::Set_Stage(CHpBar::STAGE eStage)
{
	m_eStage = eStage;
}

HRESULT CHpBar::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CHpBar::Ready_Layer_UI()
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 0;

	if (0 == m_iOption)
	{
		if (!lstrcmp(m_UIDesc.szUITag, L"CodyHpBar"))
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("CodyHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pHpBarFrame = static_cast<CHpBarFrame*>(pGameObject);
			m_pHpBarFrame->Set_PlayerID(m_ePlayerID);

			iOption = 0;
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Portrait_Cody"), &iOption, &pGameObject), E_FAIL);
			m_pPortrait = static_cast<CPortrait*>(pGameObject);
			m_pPortrait->Set_PlayerID(Player::Cody);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MayHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pHpBarFrame = static_cast<CHpBarFrame*>(pGameObject);
			m_pHpBarFrame->Set_PlayerID(m_ePlayerID);

			iOption = 0;
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Portrait_May"), &iOption, &pGameObject), E_FAIL);
			m_pPortrait = static_cast<CPortrait*>(pGameObject);
			m_pPortrait->Set_PlayerID(Player::May);

			m_fDecreaseRateRatio = 0.5f;
			m_fRatio = 0.5f;
		}
	}
	else
	{
		if (!lstrcmp(m_UIDesc.szUITag, L"CodySubHpBar"))
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("CodySubHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pHpBarFrame = static_cast<CHpBarFrame*>(pGameObject);
			m_pHpBarFrame->Set_PlayerID(m_ePlayerID);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("MaySubHpBarFrame"), nullptr, &pGameObject), E_FAIL);
			m_pHpBarFrame = static_cast<CHpBarFrame*>(pGameObject);
			m_pHpBarFrame->Set_PlayerID(m_ePlayerID);

			m_fDecreaseRateRatio = 0.5f;
			m_fRatio = 0.5f;
		}

	}

	return S_OK;
}

void CHpBar::Shake_Effect(_double TimeDelta)
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

void CHpBar::CodyHpBar_Boss(_double TimeDelta)
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
			m_pHpBarFrame->Set_Active(false);
		}

		m_fWatingTime += (_float)TimeDelta;

		if (1.f <= m_fWatingTime)
		{
			m_fRecoveryTime += (_float)TimeDelta;

			if (0.02f <= m_fRecoveryTime)
			{
				m_IsRecovery = true;
				m_fHp += 10.f;

				if (120.f <= m_fHp)
				{
					m_fHp = 120.f;
					m_IsRecovery = false;
				}

				m_fRatio = 0.5f - (m_fHp / m_fMaxHp) / 2.f;
				m_fDecreaseRateRatio = m_fRatio;
				m_fRecoveryTime = 0.f;
			}
		}
	}
}

void CHpBar::MayHpBar_Boss(_double TimeDelta)
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
			m_pHpBarFrame->Set_Active(false);
		}

		m_fWatingTime += (_float)TimeDelta;

		if (1.f <= m_fWatingTime)
		{
			m_fRecoveryTime += (_float)TimeDelta;

			if (0.02f <= m_fRecoveryTime)
			{
				m_IsRecovery = true;
				m_fHp += 10.f;

				if (120.f <= m_fHp)
				{
					m_fHp = 120.f;
					m_IsRecovery = false;
				}

				m_fRatio = (m_fHp / 120.f) / 2.f;
				m_fDecreaseRateRatio = m_fRatio;
				m_fRecoveryTime = 0.f;
			}
		}
	}
}

void CHpBar::CodyHpBar_Minigame(_double TimeDelta)
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
}

void CHpBar::MayHpBar_Minigame(_double TimeDelta)
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
	Safe_Release(m_pPortrait);
	Safe_Release(m_pHpBarFrame);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
