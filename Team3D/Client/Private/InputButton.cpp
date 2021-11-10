#include "stdafx.h"
#include "..\Public\InputButton.h"

#include "GameInstance.h"

CInputButton::CInputButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CInputButton::CInputButton(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CInputButton::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInputButton::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CInputButton::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CInputButton::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CInputButton::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	return _int();
}

HRESULT CInputButton::Render()
{
	CUIObject::Render();

	return S_OK;
}

HRESULT CInputButton::Set_ConstantTable()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	CPipeline* pPipeline = CPipeline::GetInstance();

	_matrix WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();
	ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	m_pVIBuffer_RectCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

HRESULT CInputButton::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CInputButton * CInputButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CInputButton* pInstance = new CInputButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create InputButton Prototype, Error to CInputButton::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInputButton::Clone_GameObject(void * pArg)
{
	CInputButton* pClone = new CInputButton(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CInputButton, Error to CInputButton::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CInputButton::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
