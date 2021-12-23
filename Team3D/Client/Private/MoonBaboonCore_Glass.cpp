#include "stdafx.h"
#include "MoonBaboonCore_Glass.h"
#include "MoonBaboonCore.h"

CMoonBaboonCore_Glass::CMoonBaboonCore_Glass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore_Glass::CMoonBaboonCore_Glass(const CMoonBaboonCore_Glass& rhs)
	: CGameObject(rhs)
{
}

void CMoonBaboonCore_Glass::Set_Broken()
{
	if (m_bBroken) return;
	m_bBroken = true;
}

void CMoonBaboonCore_Glass::Set_WorldMatrix()
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixTranslation(m_vLocalPos.x + m_vOffsetPos.x, m_vLocalPos.y + m_vOffsetPos.y, m_vLocalPos.z + m_vOffsetPos.z) * m_pParentTransform->Get_WorldMatrix());
}

HRESULT CMoonBaboonCore_Glass::NativeConstruct(void* pArg)
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
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Glass_01"), TEXT("Com_Model_Glass"), (CComponent**)&m_pModelCom_Glass), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Glass_Broken_01"), TEXT("Com_Model_GlassBroken"), (CComponent**)&m_pModelCom_GlassBroken), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Shield_01"), TEXT("Com_Model_Pillar"), (CComponent**)&m_pModelCom_Pillar), E_FAIL);
	
	m_pModelCom = m_pModelCom_Glass;

	Set_WorldMatrix();
	m_UserData.eID = GameID::eBOSSCORE;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

_int CMoonBaboonCore_Glass::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (0 != m_pParent->Get_ActiveCore())
	{
		OnTrigger(TimeDelta);

		Set_WorldMatrix();
		m_pStaticActorCom->Update_StaticActor();
	}

	return _int();
}

_int CMoonBaboonCore_Glass::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMoonBaboonCore_Glass::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom_Pillar, E_FAIL);
	m_pModelCom_Pillar->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom_Pillar->Render_Model(7);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(6);

	return S_OK;
}

void CMoonBaboonCore_Glass::OnTrigger(_double TimeDelta)
{
	// 부서진거랑 / 안부서진거
	if (m_bBroken)
	{
		if(m_pParent) m_pParent->Set_Broken();
		m_pModelCom = m_pModelCom_GlassBroken;
	}
}

CMoonBaboonCore_Glass* CMoonBaboonCore_Glass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMoonBaboonCore_Glass* pInstance = new CMoonBaboonCore_Glass(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboonCore_Glass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboonCore_Glass::Clone_GameObject(void* pArg)
{
	return this;
}

void CMoonBaboonCore_Glass::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	m_pModelCom = nullptr;
	Safe_Release(m_pModelCom_Glass);
	Safe_Release(m_pModelCom_GlassBroken);
	
	Safe_Release(m_pModelCom_Pillar);

	Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
