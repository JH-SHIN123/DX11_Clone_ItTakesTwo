#include "stdafx.h"
#include "..\Public\InputButton_Frame.h"

#include "GameInstance.h"

CInputButton_Frame::CInputButton_Frame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: COrtho_UIObject(pDevice, pDeviceContext)
{
}

CInputButton_Frame::CInputButton_Frame(const COrtho_UIObject & rhs)
	: COrtho_UIObject(rhs)
{
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct(void * pArg)
{
	COrtho_UIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CInputButton_Frame::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	COrtho_UIObject::Tick(TimeDelta);

	return _int();
}

_int CInputButton_Frame::Late_Tick(_double TimeDelta)
{
	COrtho_UIObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInputButton_Frame::Render()
{
	COrtho_UIObject::Render();

	if (FAILED(Set_ConstantTable()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CInputButton_Frame::Set_ConstantTable()
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

HRESULT CInputButton_Frame::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CInputButton_Frame * CInputButton_Frame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CInputButton_Frame* pInstance = new CInputButton_Frame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create InputButton_Frame Prototype, Error to CInputButton_Frame::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInputButton_Frame::Clone_GameObject(void * pArg)
{
	CInputButton_Frame* pClone = new CInputButton_Frame(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CInputButton_Frame, Error to CInputButton_Frame::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CInputButton_Frame::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	COrtho_UIObject::Free();
}
