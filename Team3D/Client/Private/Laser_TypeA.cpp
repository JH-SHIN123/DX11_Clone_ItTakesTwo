#include "stdafx.h"
#include "..\Public\Laser_TypeA.h"

CLaser_TypeA::CLaser_TypeA(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLaser(pDevice, pDeviceContext)
{
}

CLaser_TypeA::CLaser_TypeA(const CLaser_TypeA & rhs)
	: CLaser(rhs)
{
}

HRESULT CLaser_TypeA::NativeConstruct_Prototype()
{
	CLaser::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_TypeA::NativeConstruct(void * pArg)
{
	CLaser::NativeConstruct(pArg);

	return S_OK;
}

_int CLaser_TypeA::Tick(_double TimeDelta)
{
	CLaser::Tick(TimeDelta);

	return NO_EVENT;
}

_int CLaser_TypeA::Late_Tick(_double TimeDelta)
{
	CLaser::Late_Tick(TimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CLaser_TypeA::Render(RENDER_GROUP::Enum eRender)
{
	CLaser::Render(eRender);

	m_pVIBufferCom->Set_DefaultVariables_Perspective(XMMatrixIdentity());
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(3));

	m_pVIBufferCom->Render(11, m_pInstanceMatrices, 1);

	return NO_EVENT;
}

HRESULT CLaser_TypeA::Render_ShadowDepth()
{
	CLaser::Render_ShadowDepth();

	return S_OK;
}

CLaser_TypeA * CLaser_TypeA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLaser_TypeA::Clone_GameObject(void * pArg)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_TypeA::Free()
{
	CLaser::Free();
}