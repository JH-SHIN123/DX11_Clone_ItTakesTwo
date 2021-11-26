#include "stdafx.h"
#include "..\public\RobotParts.h"
#include "GameInstance.h"

CRobotParts::CRobotParts(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRobotParts::CRobotParts(const CRobotParts& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobotParts::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobotParts::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int CRobotParts::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CRobotParts::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CRobotParts::Render(RENDER_GROUP::Enum eGroup)
{
	return CGameObject::Render(eGroup);
}

HRESULT CRobotParts::Render_ShadowDepth()
{
	return CGameObject::Render_ShadowDepth();
}

void CRobotParts::Free()
{

	CGameObject::Free();
}