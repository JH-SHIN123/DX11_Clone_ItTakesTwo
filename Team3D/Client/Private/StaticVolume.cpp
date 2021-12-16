#include "stdafx.h"
#include "StaticVolume.h"

CStaticVolume::CStaticVolume(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject(pDevice, pDeviceContext)
{
}

CStaticVolume::CStaticVolume(const CStaticVolume& rhs)
    : CGameObject(rhs)
{
}

HRESULT CStaticVolume::NativeConstruct_Prototype()
{
    CGameObject::NativeConstruct_Prototype();

    return S_OK;
}

HRESULT CStaticVolume::NativeConstruct(void* pArg)
{
    CGameObject::NativeConstruct(pArg);

    if (nullptr != pArg)
        memcpy(&m_tVolumeDesc, pArg, sizeof(m_tVolumeDesc));

    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
   
    const _tchar* pModelTag = nullptr;
    switch (m_tVolumeDesc.eVolumeType)
    {
    case TYPE_CUBE:
        pModelTag = TEXT("Component_Model_Instance_GeoCube");
        break;
    case TYPE_SPHERE:
        pModelTag = TEXT("Component_Model_Instance_GeoSphere");
        break;
    case TYPE_CONE:
        pModelTag = TEXT("Component_Model_Instance_GeoCone");
        break;
    case TYPE_CYLINDER:
        pModelTag = TEXT("Component_Model_Instance_GeoCylinder");
        break;
    }
    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, pModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_tVolumeDesc.Instancing_Arg), E_FAIL);

    NULL_CHECK_RETURN(m_tVolumeDesc.Instancing_Arg.pWorldMatrices, E_FAIL);
    NULL_CHECK_RETURN(m_tVolumeDesc.arrInnerColor, E_FAIL);
    NULL_CHECK_RETURN(m_tVolumeDesc.arrOuterColor, E_FAIL);

    _uint iInstanceCnt = m_tVolumeDesc.Instancing_Arg.iInstanceCount;
    m_arrWorldMatrices_InnerColor = new _float4x4[iInstanceCnt];
    m_arrWorldMatrices_OuterColor = new _float4x4[iInstanceCnt];

    _float4x4 WorldMatrix = MH_XMFloat4x4Identity();
    for (_uint i = 0; i < iInstanceCnt; ++i)
    {
        WorldMatrix = m_tVolumeDesc.Instancing_Arg.pWorldMatrices[i];
        WorldMatrix._14 = m_tVolumeDesc.arrInnerColor[i].x;
        WorldMatrix._24 = m_tVolumeDesc.arrInnerColor[i].y;
        WorldMatrix._34 = m_tVolumeDesc.arrInnerColor[i].z;
        m_arrWorldMatrices_InnerColor[i] = WorldMatrix;
        
        WorldMatrix._14 = m_tVolumeDesc.arrOuterColor[i].x;
        WorldMatrix._24 = m_tVolumeDesc.arrOuterColor[i].y;
        WorldMatrix._34 = m_tVolumeDesc.arrOuterColor[i].z;
        m_arrWorldMatrices_OuterColor[i] = WorldMatrix;
    }

    return S_OK;
}

_int CStaticVolume::Tick(_double TimeDelta)
{
    CGameObject::Tick(TimeDelta);

    return NO_EVENT;
}

_int CStaticVolume::Late_Tick(_double TimeDelta)
{
    CGameObject::Late_Tick(TimeDelta);

    if (0 < m_pModelCom->Culling())
        m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_VOLUME, this);

    return NO_EVENT;
}

HRESULT CStaticVolume::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective();

    if (RENDER_GROUP::RENDER_VOLUME_FRONT == eGroup)
    {
        /* Inner : Front */
        m_pModelCom->Set_RealTimeMatrices(m_arrWorldMatrices_InnerColor);
        m_pModelCom->Render_Model(4);
    }
    else if (RENDER_GROUP::RENDER_VOLUME_BACK == eGroup)
    {
        /* Outer : Front */
        m_pModelCom->Set_RealTimeMatrices(m_arrWorldMatrices_OuterColor);
        m_pModelCom->Render_Model(5);
    }

	return S_OK;
}

CStaticVolume* CStaticVolume::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CStaticVolume* pInstance = new CStaticVolume(pDevice, pDeviceContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSG_BOX("Failed to Create Instance - CStaticVolume");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStaticVolume::Clone_GameObject(void* pArg)
{
    CStaticVolume* pInstance = new CStaticVolume(*this);

    if (FAILED(pInstance->NativeConstruct(pArg)))
    {
        MSG_BOX("Failed to Clone Instance - CStaticVolume");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStaticVolume::Free()
{
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pModelCom);

    // InnerColor, Outer Color Release
    SAFE_DELETE_ARRAY(m_tVolumeDesc.arrInnerColor);
    SAFE_DELETE_ARRAY(m_tVolumeDesc.arrOuterColor);

    SAFE_DELETE_ARRAY(m_arrWorldMatrices_InnerColor);
    SAFE_DELETE_ARRAY(m_arrWorldMatrices_OuterColor);

    CGameObject::Free();
}
