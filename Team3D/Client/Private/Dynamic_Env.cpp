#include "stdafx.h"
#include "..\Public\Dynamic_Env.h"

CDynamic_Env::CDynamic_Env(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDynamic_Env::CDynamic_Env(const CDynamic_Env & rhs)
	: CGameObject(rhs)
{
}

_fvector CDynamic_Env::Get_Position()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CDynamic_Env::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDynamic_Env::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CDynamic_Env::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CDynamic_Env::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

HRESULT CDynamic_Env::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	return S_OK;
}

HRESULT CDynamic_Env::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	return S_OK;
}

void CDynamic_Env::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);
}

HRESULT CDynamic_Env::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDynamic_Env_Desc, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tDynamic_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tDynamic_Env_Desc.WorldMatrix));

	return S_OK;
}

void CDynamic_Env::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
