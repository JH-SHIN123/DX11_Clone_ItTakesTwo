#include "stdafx.h"
#include "..\Public\Space_Valve_Star.h"

CSpace_Valve_Star::CSpace_Valve_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpace_Valve_Star::CSpace_Valve_Star(const CSpace_Valve_Star & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpace_Valve_Star::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSpace_Valve_Star::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CSpace_Valve_Star::Tick(_double TimeDelta)
{
	return _int();
}

_int CSpace_Valve_Star::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CSpace_Valve_Star::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

void CSpace_Valve_Star::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CSpace_Valve_Star::Render_ShadowDepth()
{
	return S_OK;
}

HRESULT CSpace_Valve_Star::Ready_Component()
{
	return S_OK;
}

CSpace_Valve_Star * CSpace_Valve_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	return nullptr;
}

CGameObject * CSpace_Valve_Star::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CSpace_Valve_Star::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
