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
    //if (nullptr != pArg)
    //    memcpy(&m_tVolumeDesc, pArg, sizeof(m_tVolumeDesc));

    //CGameObject::NativeConstruct(nullptr);

    //FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
    //FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
    //FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tVolumeDesc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

    //m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tVolumeDesc.WorldMatrix));

    return S_OK;
}

_int CStaticVolume::Tick(_double TimeDelta)
{
    CGameObject::Tick(TimeDelta);

    return NO_EVENT;
}

_int CStaticVolume::Late_Tick(_double TimeDelta)
{
    return _int();
}

HRESULT CStaticVolume::Render(RENDER_GROUP::Enum eGroup)
{
    return E_NOTIMPL;
}

CStaticVolume* CStaticVolume::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    return nullptr;
}

CGameObject* CStaticVolume::Clone_GameObject(void* pArg)
{
    return nullptr;
}

void CStaticVolume::Free()
{
}
