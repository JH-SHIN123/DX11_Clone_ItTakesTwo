#include "stdafx.h"
#include "..\public\Return_Button.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Participation_Button.h"

CReturn_Button::CReturn_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CReturn_Button::CReturn_Button(const CReturn_Button & rhs)
	: CGameObject(rhs)
{
}

HRESULT CReturn_Button::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CReturn_Button::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(330.f, -320.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(163.f * 0.6f, 74.f * 0.6f, 0.f, 1.f));

	DATABASE->Set_ReturnButton(this);

	return S_OK;
}

_int CReturn_Button::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == ((CParticipation_Button*)(DATABASE->Get_Participation_Cody()))->Get_ReadyCody() &&
		true == ((CParticipation_Button*)(DATABASE->Get_Participation_May()))->Get_ReadyMay())
	{
		m_bReady = true;

		if(m_pGameInstance->Key_Down(DIK_RETURN))
			m_bChangeScene = true;
	}

	return NO_EVENT;
}

_int CReturn_Button::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (false == m_bReady)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CReturn_Button::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	return S_OK;
}

HRESULT CReturn_Button::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("ReturnButton"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	return S_OK;
}

HRESULT CReturn_Button::Set_UIVariables_Perspective()
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

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));
	return S_OK;
}

CReturn_Button * CReturn_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CReturn_Button* pInstance = new CReturn_Button(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CReturn_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CReturn_Button::Clone_GameObject(void * pArg)
{
	CReturn_Button* pInstance = new CReturn_Button(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CReturn_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CReturn_Button::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_RectCom);

	CGameObject::Free();
}