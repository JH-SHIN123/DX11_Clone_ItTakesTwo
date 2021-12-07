#include "stdafx.h"
#include "..\Public\Gauge_Circle.h"

#include "ContextIcon.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"

CGauge_Circle::CGauge_Circle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CGauge_Circle::CGauge_Circle(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CGauge_Circle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGauge_Circle::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CGauge_Circle::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	/* Option 0 : HookUFO / Option 1 : SpaceRail */
	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(Ready_Layer_SwingPoint(TEXT("Layer_ContextIcon_SwingPoint")), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_fAlpha = 0.5f;

	return S_OK;
}

_int CGauge_Circle::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CGauge_Circle::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	FindDistanceRatio();

	if (1.f <= m_fDistance)
	{
		m_fAlpha -= (_float)TimeDelta;
		m_pContextIcon->Set_Alpha(m_fAlpha);

		if (0.f >= m_fAlpha)
			m_IsUICreateOn = true;
	}

	if (true == m_IsUICreateOn && m_ePlayerID == GameID::eCODY)
	{
		m_IsActive = false;
		UI_CreateOnlyOnce(Cody, InputButton_InterActive_Rail);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive_Rail, XMLoadFloat4(&m_vTargetPos));
	}

	if (true == m_IsUICreateOn && m_ePlayerID == GameID::eMAY)
	{
		m_IsActive = false;
		UI_CreateOnlyOnce(May, InputButton_InterActive_Rail);
		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive_Rail, XMLoadFloat4(&m_vTargetPos));
	}

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CGauge_Circle::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_InterActiveVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	if(1 == m_iOption)
		m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pVIBuffer_RectCom->Set_Variable("g_fDistance", &m_fDistance, sizeof(_float));
	m_pVIBuffer_RectCom->Render(15);

	return S_OK;
}

void CGauge_Circle::Set_Active(_bool IsActive)
{
	m_IsActive = IsActive;
	
	if (nullptr != m_pContextIcon)
		m_pContextIcon->Set_Active(IsActive);
}

void CGauge_Circle::Set_TargetPos(_vector vPos)
{
	XMStoreFloat4(&m_vTargetPos, vPos);

	if (nullptr != m_pContextIcon)
		m_pContextIcon->Set_TargetPos(vPos);
}

void CGauge_Circle::Set_SwingPointPlayerID(Player::ID ePlayerID)
{
	m_ePlayerID = ePlayerID;

	if (nullptr != m_pContextIcon)
		m_pContextIcon->Set_SwingPointPlayerID(ePlayerID);
}

void CGauge_Circle::Set_Range(_float fRange)
{
	m_fRange = fRange;
}

void CGauge_Circle::Set_Position(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CGauge_Circle::Set_DefaultSetting()
{
	m_fAlpha = 0.5f;
	m_IsActive = false;
	m_IsUICreateOn = false;
	m_fDistance = 0.f;
	
	if (nullptr != m_pContextIcon)
		m_pContextIcon->Set_Alpha(1.f);
}

void CGauge_Circle::FindDistanceRatio()
{
	if (0 == m_iOption)
	{
		if (m_ePlayerID == Player::Cody)
		{
			_vector vCodyPos = ((CCody*)DATABASE->GetCody())->Get_Position();
			_vector vDistancePos = XMLoadFloat4(&m_vTargetPos) - vCodyPos;

			_vector vLength = XMVector3Length(vDistancePos);
			_float fDistance = fabs(XMVectorGetX(vLength));

			fDistance -= 15.f;
			m_fRange = 25.f;

			m_fDistance = 1.f - (fDistance / m_fRange);
		}
		else if (m_ePlayerID == Player::May)
		{
			_vector vMayPos = ((CMay*)DATABASE->GetMay())->Get_Position();
			_vector vDistancePos = XMLoadFloat4(&m_vTargetPos) - vMayPos;

			_vector vLength = XMVector3Length(vDistancePos);
			_float fDistance = fabs(XMVectorGetX(vLength));

			fDistance -= 15.f;
			m_fRange = 25.f;

			m_fDistance = 1.f - (fDistance / m_fRange);
		}
	}
	else if (1 == m_iOption)
	{
		if (m_ePlayerID == Player::Cody)
		{
			_vector vCodyPos = ((CCody*)DATABASE->GetCody())->Get_Position();
			_vector vDistancePos = XMLoadFloat4(&m_vTargetPos) - vCodyPos;

			_vector vLength = XMVector3Length(vDistancePos);
			_float fDistance = fabs(XMVectorGetX(vLength));

			m_fDistance = 1.f - (fDistance / m_fRange) + 0.3f;
		}
		else if (m_ePlayerID == Player::May)
		{
			_vector vMayPos = ((CMay*)DATABASE->GetMay())->Get_Position();
			_vector vDistancePos = XMLoadFloat4(&m_vTargetPos) - vMayPos;

			_vector vLength = XMVector3Length(vDistancePos);
			_float fDistance = fabs(XMVectorGetX(vLength));

			m_fDistance = 1.f - (fDistance / m_fRange) + 0.3f;
		}
	}
}

HRESULT CGauge_Circle::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CGauge_Circle::Ready_Layer_SwingPoint(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("ContextIcon_SwingPoint"), nullptr, &pGameObject), E_FAIL);
	m_pContextIcon = static_cast<CContextIcon*>(pGameObject);

	return S_OK;
}

CGauge_Circle * CGauge_Circle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGauge_Circle* pInstance = new CGauge_Circle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Gauge_Circle Prototype, Error to CGauge_Circle::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGauge_Circle::Clone_GameObject(void * pArg)
{
	CGauge_Circle* pClone = new CGauge_Circle(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CGauge_Circle, Error to CGauge_Circle::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGauge_Circle::Free()
{
	Safe_Release(m_pContextIcon);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
