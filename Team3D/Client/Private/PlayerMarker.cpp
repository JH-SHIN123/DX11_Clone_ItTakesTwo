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

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CPlayerMarker::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	D3D11_VIEWPORT Viewport;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
	}

	m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));


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
