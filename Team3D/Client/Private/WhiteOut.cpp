#include "stdafx.h"
#include "..\public\WhiteOut.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Level_Loading.h"

CWhiteOut::CWhiteOut(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWhiteOut::CWhiteOut(const CWhiteOut & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWhiteOut::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CWhiteOut::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 1.f));
	return S_OK;
}

_int CWhiteOut::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_fAlpha += (_float)dTimeDelta * 0.3f;

	if (1.f <= m_fAlpha)
	{
		m_fAlpha = 1.f;

		m_pGameInstance->Go_NextLevelStep();
	}

	return NO_EVENT;
}

_int CWhiteOut::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CWhiteOut::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(32);

	return S_OK;
}

HRESULT CWhiteOut::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CWhiteOut::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();
	ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	m_pVIBuffer_RectCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	return S_OK;
}

CWhiteOut * CWhiteOut::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWhiteOut* pInstance = new CWhiteOut(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWhiteOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWhiteOut::Clone_GameObject(void * pArg)
{
	CWhiteOut* pInstance = new CWhiteOut(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWhiteOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWhiteOut::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBuffer_RectCom);

	CGameObject::Free();
}