#include "stdafx.h"
#include "..\Public\Effect_Hit_BossLaser.h"

CEffect_Hit_BossLaser::CEffect_Hit_BossLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Hit_BossLaser::CEffect_Hit_BossLaser(const CEffect_Hit_BossLaser & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Hit_BossLaser::NativeConstruct_Prototype(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CEffect_Hit_BossLaser::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

_int CEffect_Hit_BossLaser::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_Hit_BossLaser::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Hit_BossLaser::Render(RENDER_GROUP::Enum eGroup)
{
	return E_NOTIMPL;
}

void CEffect_Hit_BossLaser::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Hit_BossLaser::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Hit_BossLaser::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Hit_BossLaser * CEffect_Hit_BossLaser::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	return nullptr;
}

CGameObject * CEffect_Hit_BossLaser::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CEffect_Hit_BossLaser::Free()
{
}
