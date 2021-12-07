#include "stdafx.h"
#include "..\Public\Effect_Pinball_Move.h"

CEffect_Pinball_Move::CEffect_Pinball_Move(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Pinball_Move::CEffect_Pinball_Move(const CEffect_Pinball_Move & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Pinball_Move::NativeConstruct_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CEffect_Pinball_Move::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CEffect_Pinball_Move::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Pinball_Move::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Pinball_Move::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

void CEffect_Pinball_Move::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Move::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Move::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Pinball_Move * CEffect_Pinball_Move::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Pinball_Move*	pInstance = new CEffect_Pinball_Move(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Pinball_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Pinball_Move::Clone_GameObject(void * pArg)
{
	CEffect_Pinball_Move* pInstance = new CEffect_Pinball_Move(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Pinball_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Pinball_Move::Free()
{
	__super::Free();
}
