#include "stdafx.h"
#include "..\Public\WallLaserTrap.h"


CWallLaserTrap::CWallLaserTrap(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWallLaserTrap::CWallLaserTrap(const CWallLaserTrap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWallLaserTrap::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWallLaserTrap::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CWallLaserTrap::Tick(_double TimeDelta)
{
	return _int();
}

_int CWallLaserTrap::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CWallLaserTrap::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

void CWallLaserTrap::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CWallLaserTrap::Render_ShadowDepth()
{
	return S_OK;
}

HRESULT CWallLaserTrap::Ready_Component()
{
	return S_OK;
}

void CWallLaserTrap::Check_Stage_Value()
{
}

CWallLaserTrap * CWallLaserTrap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	return nullptr;
}

CGameObject * CWallLaserTrap::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CWallLaserTrap::Free()
{
	__super::Free();
}
