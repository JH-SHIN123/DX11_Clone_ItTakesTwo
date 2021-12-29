#include "stdafx.h"
#include "..\Public\Portrait.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CPortrait::CPortrait(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CPortrait::CPortrait(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CPortrait::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPortrait::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CPortrait::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	if (0 == m_iOption)
		m_iShaderPassNum = 0;
	else
		m_iShaderPassNum = 11;

	return S_OK;
}

_int CPortrait::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CPortrait::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	if (false == m_IsActive)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CPortrait::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (0 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}

	m_pVIBuffer_RectCom->Render(m_iShaderPassNum);

	return S_OK;
}

void CPortrait::Set_Active(_bool IsActive)
{
	m_IsActive = IsActive;
}

void CPortrait::Set_Position(_float fTime)
{
	m_UIDesc.vPos.x -= fTime;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
}

HRESULT CPortrait::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CPortrait * CPortrait::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPortrait* pInstance = new CPortrait(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Portrait Prototype, Error to CPortrait::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPortrait::Clone_GameObject(void * pArg)
{
	CPortrait* pClone = new CPortrait(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPortrait, Error to CPortrait::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPortrait::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
