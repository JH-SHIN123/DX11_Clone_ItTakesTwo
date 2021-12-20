#include "stdafx.h"
#include "..\Public\Minigame_Score.h"

CMinigame_Score::CMinigame_Score(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigame_Score::CMinigame_Score(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigame_Score::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigame_Score::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigame_Score::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_UIDesc.iTextureRenderIndex, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CMinigame_Score::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CMinigame_Score::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_Score::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(26);

	return S_OK;
}


HRESULT CMinigame_Score::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CMinigame_Score * CMinigame_Score::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigame_Score* pInstance = new CMinigame_Score(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Minigame_Score Prototype, Error to CMinigame_Score::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigame_Score::Clone_GameObject(void * pArg)
{
	CMinigame_Score* pClone = new CMinigame_Score(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigame_Score, Error to CMinigame_Score::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigame_Score::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
