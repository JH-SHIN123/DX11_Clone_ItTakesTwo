#include "stdafx.h"
#include "..\Public\PC_MouseButton.h"

#include "UI_Generator.h"

CPC_MouseButton::CPC_MouseButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CPC_MouseButton::CPC_MouseButton(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CPC_MouseButton::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPC_MouseButton::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	if (!lstrcmp(m_UIDesc.szUITag, L"PC_Mouse_Reduction"))
		m_iShaderMouseOption = 0;
	else
		m_iShaderMouseOption = 1;

	return S_OK;
}

HRESULT CPC_MouseButton::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x - 20.f, m_UIDesc.vScale.y - 10.f , 0.f, 0.f));

	return S_OK;
}

_int CPC_MouseButton::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CPC_MouseButton::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CPC_MouseButton::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	// 얜 자기꺼 써야함
	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(2);

	Render_Font();


	return S_OK;
}

HRESULT CPC_MouseButton::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;
	_int iGsOption;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	m_pVIBuffer_RectCom->Set_Variable("g_iShaderMouseOption", &m_iShaderMouseOption, sizeof(_uint));

	D3D11_VIEWPORT Viewport;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		iGsOption = 0;

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		iGsOption = 1;

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
	}

	m_pVIBuffer_RectCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

	m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	if (1 <= m_UIDesc.iSubTextureNum)
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_SubTexture", m_pTextureCom->Get_ShaderResourceView(0));

	return S_OK;
}

void CPC_MouseButton::Render_Font()
{
	CUI_Generator::FONTDESC		tFontDesc;
	tFontDesc.vPosition = { m_UIDesc.vPos.x - 7.f , m_UIDesc.vPos.y - 50.f };
	tFontDesc.vScale = { 25.f, 25.f };

	if (!lstrcmp(m_UIDesc.szUITag, TEXT("PC_Mouse_Reduction")))
		UI_Generator->Render_Font(TEXT("축소"), tFontDesc, m_ePlayerID);
	else
		UI_Generator->Render_Font(TEXT("확대"), tFontDesc, m_ePlayerID);
}

HRESULT CPC_MouseButton::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CPC_MouseButton * CPC_MouseButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPC_MouseButton* pInstance = new CPC_MouseButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create PC_MouseButton Prototype, Error to CPC_MouseButton::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPC_MouseButton::Clone_GameObject(void * pArg)
{
	CPC_MouseButton* pClone = new CPC_MouseButton(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPC_MouseButton, Error to CPC_MouseButton::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPC_MouseButton::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
