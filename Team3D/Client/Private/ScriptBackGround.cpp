#include "stdafx.h"
#include "..\public\ScriptBackGround.h"
#include "GameInstance.h"
#include "UI_Generator.h"

CScriptBackGound::CScriptBackGound(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CScriptBackGound::CScriptBackGound(const CScriptBackGound & rhs)
	: CGameObject(rhs)
{
}

HRESULT CScriptBackGound::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CScriptBackGound::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	_vector vPosition = XMVectorZero();
	if (nullptr != pArg)
		memcpy(&vPosition, pArg, sizeof(_vector));

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(512.f * 0.7f, 64.f * 0.5f, 0.f, 1.f));

	return S_OK;
}

_int CScriptBackGound::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CScriptBackGound::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

HRESULT CScriptBackGound::Render(RENDER_GROUP::Enum eGroup, _float2 fScale)
{
	CGameObject::Render(eGroup);

	m_pTransformCom->Set_Scale(XMVectorSet(fScale.x, 64.f * 0.5f * fScale.y, 0.f, 1.f));

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	return S_OK;
}

HRESULT CScriptBackGound::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ScriptBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	return S_OK;
}

HRESULT CScriptBackGound::Set_UIVariables_Perspective()
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

CScriptBackGound * CScriptBackGound::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScriptBackGound* pInstance = new CScriptBackGound(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CScriptBackGound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScriptBackGound::Clone_GameObject(void * pArg)
{
	CScriptBackGound* pInstance = new CScriptBackGound(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CScriptBackGound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScriptBackGound::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_RectCom);

	CGameObject::Free();
}