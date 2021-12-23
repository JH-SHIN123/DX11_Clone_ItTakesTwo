#include "stdafx.h"
#include "..\Public\Minigame_Win.h"

#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"

CMinigame_Win::CMinigame_Win(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigame_Win::CMinigame_Win(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigame_Win::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigame_Win::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigame_Win::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_UIDesc.iTextureRenderIndex, pArg, sizeof(_uint));

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vSaveScale = m_UIDesc.vScale;

	m_UIDesc.vScale.x = 1200.f;
	m_UIDesc.vScale.y = 1000.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CMinigame_Win::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	if (m_vSaveScale.x <= m_UIDesc.vScale.x)
	{
		m_fAlpha += (_float)TimeDelta * 3.f;
		m_UIDesc.vScale.x -= (_float)TimeDelta * 2000.f;
		m_UIDesc.vScale.y -= (_float)TimeDelta * 2000.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
	}

	m_fDeadTime += (_float)TimeDelta;

	if (3.f <= m_fDeadTime)
	{
		UI_Delete(Default, Minigame_Win_Cody);
		UI_Delete(Default, Minigame_Win_May);
		UI_CreateOnlyOnce(Default, Minigame_Score);
	}

	return _int();
}

_int CMinigame_Win::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_Win::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pVIBuffer_RectCom->Render(26);

	return S_OK;
}


HRESULT CMinigame_Win::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CMinigame_Win * CMinigame_Win::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigame_Win* pInstance = new CMinigame_Win(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Minigame_Win Prototype, Error to CMinigame_Win::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigame_Win::Clone_GameObject(void * pArg)
{
	CMinigame_Win* pClone = new CMinigame_Win(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigame_Win, Error to CMinigame_Win::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigame_Win::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
