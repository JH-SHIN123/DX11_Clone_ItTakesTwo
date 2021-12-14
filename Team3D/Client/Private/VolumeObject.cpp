#include "stdafx.h"
#include "..\Public\VolumeObject.h"

CVolumeObject::CVolumeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CVolumeObject::CVolumeObject(const CVolumeObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CVolumeObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CVolumeObject::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(2.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GeoSphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_Scale(XMVectorSet(50.f, 50.f, 50.f,0.f));

	return S_OK;
}

_int CVolumeObject::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CVolumeObject::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 100.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_VOLUME, this);
	}

	return NO_EVENT;
}

HRESULT CVolumeObject::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	
	if(RENDER_GROUP::VOLUME_FRONT == eGroup)
		m_pModelCom->Render_Model(14);
	else
		m_pModelCom->Render_Model(15);

	return S_OK;
}

CVolumeObject* CVolumeObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVolumeObject* pInstance = new CVolumeObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CVolumeObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVolumeObject::Clone_GameObject(void* pArg)
{
	CVolumeObject* pInstance = new CVolumeObject(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CVolumeObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVolumeObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
