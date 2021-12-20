#include "stdafx.h"
#include "..\Public\Minigame_Countdown.h"

#include "UI_Generator.h"
#include "Minigame_GaugeCircle.h"
#include "LaserTennis_Manager.h"

CMinigame_Countdown::CMinigame_Countdown(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigame_Countdown::CMinigame_Countdown(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigame_Countdown::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigame_Countdown::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigame_Countdown::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	m_UIDesc.vScale.x = 400.f;
	m_UIDesc.vScale.y = 400.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_vSaveScale = m_UIDesc.vScale;
	m_vMinScale = { 50.f, 50.f };

	return S_OK;
}

_int CMinigame_Countdown::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CMinigame_Countdown::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (m_UIDesc.vScale.x >= m_vMinScale.x + 40.f)
	{
		m_fAlpha += (_float)TimeDelta * 3.f;
		m_UIDesc.vScale.x -= (_float)TimeDelta * 420.f;
		m_UIDesc.vScale.y -= (_float)TimeDelta * 420.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

		if (1.f <= m_fAlpha)
			m_fAlpha = 1.f;
	}
	else if (m_UIDesc.vScale.x >= m_vMinScale.x)
	{
		m_fAlpha -= (_float)TimeDelta * 5.f;
		m_UIDesc.vScale.x -= (_float)TimeDelta * 50.f;
		m_UIDesc.vScale.y -= (_float)TimeDelta * 50.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

		if (0.f >= m_fAlpha)
			m_fAlpha = 0.f;
	}
	else if(true == m_IsStart)
	{
		++m_UIDesc.iTextureRenderIndex;
		m_UIDesc.vScale = m_vSaveScale;

		m_IsStart = false;

		if (4 < m_UIDesc.iTextureRenderIndex)
			LASERTENNIS->Start_Game();
	}

	if (true == m_pMinigame_GaugeCircle->Get_NextCount() && 3 >= m_UIDesc.iTextureRenderIndex)
	{
		++m_UIDesc.iTextureRenderIndex;
		m_UIDesc.vScale = m_vSaveScale;
	}

	if (4 < m_UIDesc.iTextureRenderIndex)
	{
		m_pMinigame_GaugeCircle->Set_Dead();
		UI_Delete(Default, Minigame_Countdown);
		m_UIDesc.iTextureRenderIndex = 4;
	}

	if (1 <= m_UIDesc.iTextureRenderIndex && 3 >= m_UIDesc.iTextureRenderIndex)
		m_pMinigame_GaugeCircle->Set_Active(true);
	else
		m_pMinigame_GaugeCircle->Set_Active(false);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_Countdown::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pVIBuffer_RectCom->Render(24);

	return S_OK;
}

HRESULT CMinigame_Countdown::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CMinigame_Countdown::Ready_Layer_UI()
{
	CGameObject* pGameObject = nullptr;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Minigame_GaugeCircle"), nullptr, &pGameObject), E_FAIL);
	m_pMinigame_GaugeCircle = static_cast<CMinigame_GaugeCircle*>(pGameObject);
	m_pMinigame_GaugeCircle->Set_PlayerID(Player::Default);

	return S_OK;
}

CMinigame_Countdown * CMinigame_Countdown::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigame_Countdown* pInstance = new CMinigame_Countdown(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Minigame_Countdown Prototype, Error to CMinigame_Countdown::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigame_Countdown::Clone_GameObject(void * pArg)
{
	CMinigame_Countdown* pClone = new CMinigame_Countdown(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigame_Countdown, Error to CMinigame_Countdown::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigame_Countdown::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
