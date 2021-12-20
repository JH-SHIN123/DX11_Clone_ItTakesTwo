#include "stdafx.h"
#include "..\Public\Effect_StarBuddy_Move.h"
#include "Effect_StarBuddy_Move_Particle.h"

CEffect_StarBuddy_Move::CEffect_StarBuddy_Move(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_StarBuddy_Move::CEffect_StarBuddy_Move(const CEffect_StarBuddy_Move & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_StarBuddy_Move::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();
	return S_OK;
}

HRESULT CEffect_StarBuddy_Move::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	/* ¸¸µé¾î */
	return S_OK;
}

_int CEffect_StarBuddy_Move::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_StarBuddy_Move::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_StarBuddy_Move::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

CEffect_StarBuddy_Move * CEffect_StarBuddy_Move::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	return nullptr;
}

CGameObject * CEffect_StarBuddy_Move::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CEffect_StarBuddy_Move::Free()
{
	__super::Free();
}
