#include "stdafx.h"
#include "..\Public\Minigame_Countdown.h"

#include "GameInstance.h"

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

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

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
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_Countdown::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(24);

	return S_OK;
}

HRESULT CMinigame_Countdown::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

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
