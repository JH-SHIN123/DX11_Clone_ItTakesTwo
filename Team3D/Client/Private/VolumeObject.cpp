#include "stdafx.h"
#include "VolumeObject.h"

CVolumeObject::CVolumeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CVolumeObject::CVolumeObject(const CVolumeObject& rhs)
	: CGameObject(rhs)
{
}

void CVolumeObject::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	NULL_CHECK(m_pTransformCom);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CVolumeObject::Set_Color(_float3& vInner, _float3& vOuter)
{
	m_tVolumeDesc.vInnerColor = vInner;
	m_tVolumeDesc.vOuterColor = vOuter;
}

HRESULT CVolumeObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CVolumeObject::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tVolumeDesc, pArg, sizeof(m_tVolumeDesc));

	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_VolumePattern"), TEXT("Com_Texture"), (CComponent**)&m_pPatternTexCom), E_FAIL);

	const _tchar* pModelTag = nullptr;
	switch (m_tVolumeDesc.eVolumeType)
	{
	case TYPE_CUBE:
		pModelTag = TEXT("Component_Model_GeoCube");
		break;
	case TYPE_SPHERE:
		pModelTag = TEXT("Component_Model_GeoSphere");
		break;
	case TYPE_CONE:
		pModelTag = TEXT("Component_Model_GeoCone");
		break;
	case TYPE_CYLINDER:
		pModelTag = TEXT("Component_Model_GeoCylinder");
		break;
	}
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, pModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tVolumeDesc.WorldMatrix));
		
	return S_OK;
}

_int CVolumeObject::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_fPatternDeltaT >= 1.f)
		m_fPatternDeltaT = 0.f;
	else
		m_fPatternDeltaT += (_float)TimeDelta * 0.2f;

	return NO_EVENT;
}

_int CVolumeObject::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_tVolumeDesc.fCullRadius)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_VOLUME, this);
	}

	return NO_EVENT;
}

HRESULT CVolumeObject::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Set_Variable("g_fTime", &m_fPatternDeltaT, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", m_pPatternTexCom->Get_ShaderResourceView(0));

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
	Safe_Release(m_pPatternTexCom);

	CGameObject::Free();
}
