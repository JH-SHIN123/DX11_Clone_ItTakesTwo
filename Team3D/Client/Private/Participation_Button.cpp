#include "stdafx.h"
#include "..\public\Participation_Button.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Return_Button.h"

CParticipation_Button::CParticipation_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CParticipation_Button::CParticipation_Button(const CParticipation_Button & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticipation_Button::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CParticipation_Button::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(323.f * 0.6f, 265.f * 0.6f, 0.f, 1.f));

	if (0 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-110.f, -230.f, 0.f, 1.f));
		m_iFrame = 0;
		DATABASE->Set_Participation_Cody(this);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(110.f, -230.f, 0.f, 1.f));
		m_iFrame = 2;
		DATABASE->Set_Participation_May(this);
	}

	return S_OK;
}

_int CParticipation_Button::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 == m_iOption && m_pGameInstance->Key_Down(DIK_RETURN))
	{
		/* Sound */
		m_pGameInstance->Stop_Sound(CHANNEL_BUTTON);
		m_pGameInstance->Play_Sound(TEXT("MainMenu_Button_In.wav"), CHANNEL_BUTTON);

		m_bReadyCody = true;
		m_iFrame = 1;
	}

	if (1 == m_iOption && (m_pGameInstance->Key_Down(DIK_SPACE) || m_pGameInstance->Pad_Key_Down(DIP_A)))
	{
		/* Sound */
		m_pGameInstance->Stop_Sound(CHANNEL_BUTTON);
		m_pGameInstance->Play_Sound(TEXT("MainMenu_Button_In.wav"), CHANNEL_BUTTON);

		m_bReadyMay = true;
		m_iFrame = 3;
	}

	return NO_EVENT;
}

_int CParticipation_Button::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CParticipation_Button::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	return S_OK;
}

HRESULT CParticipation_Button::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_LOGO, TEXT("Participation"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	return S_OK;
}

HRESULT CParticipation_Button::Set_UIVariables_Perspective()
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

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_iFrame));
	return S_OK;
}

CParticipation_Button * CParticipation_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CParticipation_Button* pInstance = new CParticipation_Button(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CParticipation_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticipation_Button::Clone_GameObject(void * pArg)
{
	CParticipation_Button* pInstance = new CParticipation_Button(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CParticipation_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticipation_Button::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_RectCom);

	CGameObject::Free();
}