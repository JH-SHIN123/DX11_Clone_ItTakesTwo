#include "stdafx.h"
#include "..\Public\Effect_Cody_Dash.h"
#include "GameInstance.h"

CEffect_Cody_Dash::CEffect_Cody_Dash(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Cody_Dash::CEffect_Cody_Dash(const CEffect_Cody_Dash & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Cody_Dash::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Cody_Dash::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_Cody_Dash::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	return _int();
}

_int CEffect_Cody_Dash::Late_Tick(_double TimeDelta)
{

	return __super::Late_Tick(TimeDelta);
}

HRESULT CEffect_Cody_Dash::Render()
{
	__super::Render();



	return S_OK;
}

void CEffect_Cody_Dash::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Dash::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

CEffect_Cody_Dash * CEffect_Cody_Dash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_Cody_Dash*	pInstance = new CEffect_Cody_Dash(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Cody_Dash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Cody_Dash::Clone_GameObject(void * pArg)
{
	CEffect_Cody_Dash* pInstance = new CEffect_Cody_Dash(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Cody_Dash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Cody_Dash::Free()
{
	__super::Free();
}
