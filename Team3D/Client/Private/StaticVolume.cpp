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
    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tVolumeDesc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_tVolumeDesc.Instancing_Arg), E_FAIL);

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
        // -> Inner랑 Outter넣은 매트릭스 행렬을 미리 만들어서 렌더시에 교체만해주자.
        _matrix WorldMatrixTranspose = XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix());
        memcpy(&WorldMatrixTranspose.r[3], &m_tVolumeDesc.vInnerColor, sizeof(_float3));
        m_pModelCom->Set_Variable("g_WorldMatrix", &WorldMatrixTranspose, sizeof(_matrix));

        m_pModelCom->Render_Model(4);
    }
    else if (RENDER_GROUP::RENDER_VOLUME_BACK == eGroup)
    {
        /* Outer : Front */
        _matrix WorldMatrixTranspose = XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix());
        memcpy(&WorldMatrixTranspose.r[3], &m_tVolumeDesc.vOuterColor, sizeof(_float3));
        m_pModelCom->Set_Variable("g_WorldMatrix", &WorldMatrixTranspose, sizeof(_matrix));

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

    CGameObject::Free();
}
