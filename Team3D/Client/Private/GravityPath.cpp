#include "stdafx.h"
#include "..\Public\GravityPath.h"

CGravityPath::CGravityPath(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGravityPath::CGravityPath(const CGravityPath & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravityPath::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CGravityPath::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Static_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB);
	m_pRendererCom;

	return S_OK;
}

_int CGravityPath::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CGravityPath::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return NO_EVENT;
}

HRESULT CGravityPath::Render(RENDER_GROUP::Enum eRender)
{
	CGameObject::Render(eRender);

	return S_OK;
}

HRESULT CGravityPath::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	return S_OK;
}

void CGravityPath::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}