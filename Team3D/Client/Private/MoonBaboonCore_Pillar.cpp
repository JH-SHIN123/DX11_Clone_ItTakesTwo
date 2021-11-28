#include "stdafx.h"
#include "MoonBaboonCore_Pillar.h"
#include "MoonBaboonCore.h"

CMoonBaboonCore_Pillar::CMoonBaboonCore_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore_Pillar::CMoonBaboonCore_Pillar(const CMoonBaboonCore_Pillar& rhs)
    : CGameObject(rhs)
{
}

void CMoonBaboonCore_Pillar::Set_WorldMatrix()
{
    m_pTransformCom->Set_WorldMatrix(m_pParentTransform->Get_WorldMatrix());
}

HRESULT CMoonBaboonCore_Pillar::NativeConstruct(void* pArg)
{
    if (nullptr != pArg)
        m_pParent = (CMoonBaboonCore*)pArg;
    else
        return E_FAIL;

    m_pParentTransform = m_pParent->Get_Transform();
    if (nullptr == m_pParentTransform) return E_FAIL;

    CGameObject::NativeConstruct(nullptr);

    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(2.f, 0.f)), E_FAIL);
    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

    m_pTransformCom->Set_WorldMatrix(m_pParentTransform->Get_WorldMatrix());
    m_UserData.eID = GameID::eENVIRONMENT;
    m_UserData.pGameObject = this;

    CStaticActor::ARG_DESC tArg;
    tArg.pModel = m_pModelCom;
    tArg.pTransform = m_pTransformCom;
    tArg.pUserData = &m_UserData;

    FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

    return S_OK;
}

_int CMoonBaboonCore_Pillar::Tick(_double TimeDelta)
{
    CGameObject::Tick(TimeDelta);

    if (0 != m_pParent->Get_ActiveCore())
    {
        Set_WorldMatrix();
        m_pStaticActorCom->Update_StaticActor();
    }

    return NO_EVENT;
}

_int CMoonBaboonCore_Pillar::Late_Tick(_double TimeDelta)
{
    CGameObject::Late_Tick(TimeDelta);

    if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
        m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
    }

    return NO_EVENT;
}

HRESULT CMoonBaboonCore_Pillar::Render(RENDER_GROUP::Enum eGroup)
{
    CGameObject::Render(eGroup);
    NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
    m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
    m_pModelCom->Set_DefaultVariables_Shadow();
    m_pModelCom->Render_Model(1);

    return S_OK;
}

HRESULT CMoonBaboonCore_Pillar::Render_ShadowDepth()
{
    NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

    m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

    // Skinned: 2 / Normal: 3
    m_pModelCom->Render_Model(3, 0, true);

    return S_OK;
}

CMoonBaboonCore_Pillar* CMoonBaboonCore_Pillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CMoonBaboonCore_Pillar* pInstance = new CMoonBaboonCore_Pillar(pDevice, pDeviceContext);

    if (FAILED(pInstance->NativeConstruct(pArg)))
    {
        MSG_BOX("Failed to Create Instance - CMoonBaboonCore_Pillar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMoonBaboonCore_Pillar::Clone_GameObject(void* pArg)
{
    return this;
}

void CMoonBaboonCore_Pillar::Free()
{
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pStaticActorCom);

    CGameObject::Free();
}
