#include "stdafx.h"
#include "..\Public\ContextIcon.h"

#include "GameInstance.h"

CContextIcon::CContextIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CContextIcon::CContextIcon(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CContextIcon::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CContextIcon::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CContextIcon::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x - 10.f, m_UIDesc.vScale.y - 10.f, 0.f, 0.f));

	m_fAlpha = 1.f;

	return S_OK;
}

_int CContextIcon::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CContextIcon::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	if (false == m_IsActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CContextIcon::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_InterActiveVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pVIBuffer_RectCom->Render(18);

	return S_OK;
}

void CContextIcon::Set_Active(_bool IsCheck)
{
	m_IsActive = IsCheck;
}

void CContextIcon::Set_SwingPointPlayerID(Player::ID ePlayerID)
{
	m_ePlayerID = ePlayerID;
}

void CContextIcon::Set_Alpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
}

HRESULT CContextIcon::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CContextIcon * CContextIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CContextIcon* pInstance = new CContextIcon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create ContextIcon Prototype, Error to CContextIcon::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CContextIcon::Clone_GameObject(void * pArg)
{
	CContextIcon* pClone = new CContextIcon(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CContextIcon, Error to CContextIcon::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CContextIcon::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
