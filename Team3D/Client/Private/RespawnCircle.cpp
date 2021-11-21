#include "stdafx.h"
#include "..\Public\RespawnCircle.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CRespawnCircle::CRespawnCircle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CRespawnCircle::CRespawnCircle(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CRespawnCircle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CRespawnCircle::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CRespawnCircle::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CRespawnCircle::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CRespawnCircle::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CRespawnCircle::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(5);

	return S_OK;
}

HRESULT CRespawnCircle::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CRespawnCircle * CRespawnCircle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRespawnCircle* pInstance = new CRespawnCircle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create RespawnCircle Prototype, Error to CRespawnCircle::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRespawnCircle::Clone_GameObject(void * pArg)
{
	CRespawnCircle* pClone = new CRespawnCircle(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CRespawnCircle, Error to CRespawnCircle::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CRespawnCircle::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
