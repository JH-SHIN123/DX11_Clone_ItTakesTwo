#include "stdafx.h"
#include "..\Public\Effect_Cody_Size_ML.h"
#include "GameInstance.h"

CEffect_Cody_Size_ML::CEffect_Cody_Size_ML(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Cody_Size_ML::CEffect_Cody_Size_ML(const CEffect_Cody_Size_ML & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Cody_Size_ML::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Cody_Size_ML::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };
	m_EffectDesc_Prototype.iInstanceCount = 2;

	__super::Ready_Component(pArg);
	Ready_Instance();

	return S_OK;
}

_int CEffect_Cody_Size_ML::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Cody_Size_ML::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Cody_Size_ML::Render()
{
	return S_OK;
}

void CEffect_Cody_Size_ML::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Size_ML::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Size_ML::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Cody_Size_ML::Ready_Instance()
{

	return S_OK;
}

CEffect_Cody_Size_ML * CEffect_Cody_Size_ML::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Cody_Size_ML*	pInstance = new CEffect_Cody_Size_ML(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Cody_Size_ML");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Cody_Size_ML::Clone_GameObject(void * pArg)
{
	CEffect_Cody_Size_ML* pInstance = new CEffect_Cody_Size_ML(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Cody_Size_ML");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Cody_Size_ML::Free()
{
	__super::Free();
}
