#include "stdafx.h"
#include "..\Public\DynamicVolume.h"

CDynamicVolume::CDynamicVolume(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDynamicVolume::CDynamicVolume(const CDynamicVolume& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDynamicVolume::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDynamicVolume::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tVolumeDesc, pArg, sizeof(m_tVolumeDesc));

	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tVolumeDesc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tVolumeDesc.WorldMatrix));
		
	return S_OK;
}

_int CDynamicVolume::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CDynamicVolume::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_tVolumeDesc.fCullRadius)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_VOLUME, this);
	}

	return NO_EVENT;
}

HRESULT CDynamicVolume::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	if (RENDER_GROUP::RENDER_VOLUME_FRONT == eGroup) 
	{
		/* Inner : Front */
		_matrix WorldMatrixTranspose = XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix());
		memcpy(&WorldMatrixTranspose.r[3], &m_tVolumeDesc.vInnerColor, sizeof(_float3));
		m_pModelCom->Set_Variable("g_WorldMatrix", &WorldMatrixTranspose, sizeof(_matrix));
		m_pModelCom->Render_Model(21);
	}
	else if (RENDER_GROUP::RENDER_VOLUME_BACK == eGroup) 
	{
		/* Outer : Front */
		_matrix WorldMatrixTranspose = XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix());
		memcpy(&WorldMatrixTranspose.r[3], &m_tVolumeDesc.vOuterColor, sizeof(_float3));
		m_pModelCom->Set_Variable("g_WorldMatrix", &WorldMatrixTranspose, sizeof(_matrix));
		m_pModelCom->Render_Model(22);
	}

	return S_OK;
}

CDynamicVolume* CDynamicVolume::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDynamicVolume* pInstance = new CDynamicVolume(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDynamicVolume");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDynamicVolume::Clone_GameObject(void* pArg)
{
	CDynamicVolume* pInstance = new CDynamicVolume(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDynamicVolume");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicVolume::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
