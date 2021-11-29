#include "stdafx.h"
#include "..\Public\TestEffect.h"
#include "GameInstance.h"

CTestEffect::CTestEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CTestEffect::CTestEffect(const CTestEffect & rhs)
	: CInGameEffect(rhs)
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

	return _int();
}

_int CTestEffect::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CTestEffect::Render(RENDER_GROUP::Enum eGroup)
{
	__super::Render(eGroup);



	return S_OK;
}

void CTestEffect::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CTestEffect::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CTestEffect::Instance_UV(_float TimeDelta, _int iIndex)
{
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
