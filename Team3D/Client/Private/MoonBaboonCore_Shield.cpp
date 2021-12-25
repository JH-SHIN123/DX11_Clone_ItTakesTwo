#include "stdafx.h"
#include "MoonBaboonCore_Shield.h"
#include "MoonBaboonCore.h"

CMoonBaboonCore_Shield::CMoonBaboonCore_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore_Shield::CMoonBaboonCore_Shield(const CMoonBaboonCore_Shield& rhs)
	: CGameObject(rhs)
{
}

void CMoonBaboonCore_Shield::Set_WorldMatrix()
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixTranslation(m_vLocalPos.x + m_vOffsetPos.x, m_vLocalPos.y + m_vOffsetPos.y, m_vLocalPos.z + m_vOffsetPos.z) * m_pParentTransform->Get_WorldMatrix());
}

HRESULT CMoonBaboonCore_Shield::NativeConstruct(void* pArg)
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
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CoreShield_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	Set_WorldMatrix();
	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

_int CMoonBaboonCore_Shield::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (0 != m_pParent->Get_ActiveCore() || m_pParent->Get_IsGoUp())
	{
		OnTrigger(TimeDelta);

		Set_WorldMatrix();
		m_pStaticActorCom->Update_StaticActor();
	}

	return _int();
}

_int CMoonBaboonCore_Shield::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMoonBaboonCore_Shield::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CMoonBaboonCore_Shield::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CMoonBaboonCore_Shield::OnTrigger(_double TimeDelta)
{
	// 몇초간 내려가기
	_float fSpeed = 5.5f;
	if (m_pParent->Get_bArrived())
	{
		if(m_vLocalPos.y > -4.f)
			m_vLocalPos.y -= (_float)TimeDelta * fSpeed;
	}
	else // 몇초간 올라가기
	{
		if (m_vLocalPos.y < 0.f)
			m_vLocalPos.y += (_float)TimeDelta * fSpeed;
	}
}

CMoonBaboonCore_Shield* CMoonBaboonCore_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMoonBaboonCore_Shield* pInstance = new CMoonBaboonCore_Shield(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboonCore_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboonCore_Shield::Clone_GameObject(void* pArg)
{
	return this;
}

void CMoonBaboonCore_Shield::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
