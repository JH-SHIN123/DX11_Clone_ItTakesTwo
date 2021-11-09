#include "stdafx.h"
#include "..\Public\TestEffect.h"


CTestEffect::CTestEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CTestEffect::CTestEffect(const CTestEffect & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CTestEffect::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CTestEffect::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CTestEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	return _int();
}

_int CTestEffect::Late_Tick(_double TimeDelta)
{

	return __super::Late_Tick(TimeDelta);
}

HRESULT CTestEffect::Render()
{
	__super::Render();



	return S_OK;
}

HRESULT CTestEffect::Set_ShaderConstant_Default()
{
	return S_OK;
}

HRESULT CTestEffect::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	return S_OK;
}

CTestEffect * CTestEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CTestEffect*	pInstance = new CTestEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CTestEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTestEffect::Clone_GameObject(void * pArg)
{
	CTestEffect* pInstance = new CTestEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTestEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestEffect::Free()
{
	__super::Free();
}
