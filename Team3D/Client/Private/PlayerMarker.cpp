#include "stdafx.h"
#include "..\Public\PlayerMarker.h"

#include "GameInstance.h"

CPlayerMarker::CPlayerMarker(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CPlayerMarker::CPlayerMarker(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CPlayerMarker::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPlayerMarker::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CPlayerMarker::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CPlayerMarker::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CPlayerMarker::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayerMarker::Render()
{
	CUIObject::Render();

	if (FAILED(CUIObject::Set_InterActiveVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CPlayerMarker::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CPlayerMarker * CPlayerMarker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPlayerMarker* pInstance = new CPlayerMarker(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create PlayerMarker Prototype, Error to CPlayerMarker::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerMarker::Clone_GameObject(void * pArg)
{
	CPlayerMarker* pClone = new CPlayerMarker(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPlayerMarker, Error to CPlayerMarker::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayerMarker::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
