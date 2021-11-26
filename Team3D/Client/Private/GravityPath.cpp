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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Bend_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

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

CGravityPath * CGravityPath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGravityPath* pInstance = new CGravityPath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CGravityPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGravityPath::Clone_GameObject(void * pArg)
{
	CGravityPath* pInstance = new CGravityPath(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CGravityPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGravityPath::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}