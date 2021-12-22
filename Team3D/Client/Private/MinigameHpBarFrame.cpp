#include "stdafx.h"
#include "..\Public\MinigameHpBarFrame.h"

CMinigameHpBarFrame::CMinigameHpBarFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigameHpBarFrame::CMinigameHpBarFrame(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigameHpBarFrame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigameHpBarFrame::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigameHpBarFrame::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y - 5.f, 0.f, 0.f));

	return S_OK;
}

_int CMinigameHpBarFrame::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CMinigameHpBarFrame::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigameHpBarFrame::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(30);

	return S_OK;
}

void CMinigameHpBarFrame::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;
}

HRESULT CMinigameHpBarFrame::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CMinigameHpBarFrame * CMinigameHpBarFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigameHpBarFrame* pInstance = new CMinigameHpBarFrame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create MinigameHpBarFrame Prototype, Error to CMinigameHpBarFrame::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigameHpBarFrame::Clone_GameObject(void * pArg)

{
	CMinigameHpBarFrame* pClone = new CMinigameHpBarFrame(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigameHpBarFrame, Error to CMinigameHpBarFrame::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigameHpBarFrame::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
