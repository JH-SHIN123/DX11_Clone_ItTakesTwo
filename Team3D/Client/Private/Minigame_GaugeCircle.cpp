#include "stdafx.h"
#include "..\Public\Minigame_GaugeCircle.h"

#include "GameInstance.h"

CMinigame_GaugeCircle::CMinigame_GaugeCircle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigame_GaugeCircle::CMinigame_GaugeCircle(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigame_GaugeCircle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigame_GaugeCircle::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigame_GaugeCircle::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_UIDesc.vScale.x = 300.f;
	m_UIDesc.vScale.y = 300.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CMinigame_GaugeCircle::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	m_fAngle -= (_float)TimeDelta * 190.f;

	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fAngle));

	return _int();
}

_int CMinigame_GaugeCircle::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_GaugeCircle::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(25);

	return S_OK;
}

HRESULT CMinigame_GaugeCircle::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CMinigame_GaugeCircle * CMinigame_GaugeCircle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigame_GaugeCircle* pInstance = new CMinigame_GaugeCircle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Minigame_GaugeCircle Prototype, Error to CMinigame_GaugeCircle::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigame_GaugeCircle::Clone_GameObject(void * pArg)
{
	CMinigame_GaugeCircle* pClone = new CMinigame_GaugeCircle(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigame_GaugeCircle, Error to CMinigame_GaugeCircle::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigame_GaugeCircle::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
