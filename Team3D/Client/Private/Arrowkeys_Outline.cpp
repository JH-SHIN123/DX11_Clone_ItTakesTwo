#include "stdafx.h"
#include "..\Public\Arrowkeys_Outline.h"

#include "GameInstance.h"

CArrowkeys_Outline::CArrowkeys_Outline(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CArrowkeys_Outline::CArrowkeys_Outline(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CArrowkeys_Outline::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CArrowkeys_Outline::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CArrowkeys_Outline::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CArrowkeys_Outline::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CArrowkeys_Outline::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CArrowkeys_Outline::Render()
{
	CUIObject::Render();

	if (FAILED(Set_UIVariables_Perspective1()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CArrowkeys_Outline::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

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

HRESULT CArrowkeys_Outline::Set_UIVariables_Perspective1()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	if (m_ePlayerID == Player::Player_Cody)
	{
		D3D11_VIEWPORT Viewport = m_pGameInstance->Get_ViewportInfo(1);

		SubProjMatrix = XMMatrixIdentity();

		// 뷰포트 하나로 합쳐질 때 Width가 0.f가 되버리면 직교할 때 0.f / 0.f 나누기 연산이 일어나서 XMSclarNearEqul 오류 발생 그거 임시방편 예외처리 입니다.
		if (0.1f <= Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));
	}         
	else if (m_ePlayerID == Player::Player_May)
	{
		D3D11_VIEWPORT Viewport = m_pGameInstance->Get_ViewportInfo(2);

		ProjMatrix = XMMatrixIdentity();

		if (0.1f <= Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));
	}

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));


	return S_OK;
}

HRESULT CArrowkeys_Outline::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CArrowkeys_Outline * CArrowkeys_Outline::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CArrowkeys_Outline* pInstance = new CArrowkeys_Outline(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Arrowkeys_Outline Prototype, Error to CArrowkeys_Outline::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CArrowkeys_Outline::Clone_GameObject(void * pArg)
{
	CArrowkeys_Outline* pClone = new CArrowkeys_Outline(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CArrowkeys_Outline, Error to CArrowkeys_Outline::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CArrowkeys_Outline::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
