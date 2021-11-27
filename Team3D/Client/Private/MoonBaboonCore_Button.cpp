#include "stdafx.h"
#include "MoonBaboonCore_Button.h"
#include "MoonBaboonCore.h"

CMoonBaboonCore_Button::CMoonBaboonCore_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore_Button::CMoonBaboonCore_Button(const CMoonBaboonCore_Button& rhs)
	: CGameObject(rhs)
{
}

void CMoonBaboonCore_Button::Set_WorldMatrix()
{
	// Offset : 0, -2, 0
	_matrix WorldMat = XMMatrixTranslation(m_vOffsetPosition.x + m_vLocalPosition.x, m_vOffsetPosition.y + m_vLocalPosition.y, m_vOffsetPosition.z + m_vLocalPosition.z) * m_pParentTransform->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(WorldMat);
}

HRESULT CMoonBaboonCore_Button::NativeConstruct(void* pArg)
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
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_CorePillar_Button_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	Set_WorldMatrix();

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
 	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pGeometry = new PxBoxGeometry(1.1f, 0.6f, 1.1f);
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

_int CMoonBaboonCore_Button::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (0 != m_pParent->Get_ActiveCore())
	{
		Set_WorldMatrix();
		m_pStaticActorCom->Update_StaticActor();
		m_pTriggerActorCom->Update_TriggerActor();
	}

	OnPressed(TimeDelta);

	return NO_EVENT;
}

_int CMoonBaboonCore_Button::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMoonBaboonCore_Button::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CMoonBaboonCore_Button::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CMoonBaboonCore_Button::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	if (nullptr == m_pParent) return;

	switch (eStatus)
	{
	case Engine::TriggerStatus::eFOUND:
		if (eID == GameID::eMAY || eID == GameID::eCODY) {
			m_bPressed = true;
			m_bReleased = false;

			m_pParent->Set_ActiveCore(1);
		}
		break;
	case Engine::TriggerStatus::eLOST:
		if (eID == GameID::eMAY || eID == GameID::eCODY) {
			m_bReleased = true;
			m_bPressed = false;

			m_pParent->Set_ActiveCore(-1);
		}
		break;
	}
}

void CMoonBaboonCore_Button::OnPressed(_double TimeDelta)
{
	_float fTriggerSpeed = 3.f;
	if (m_bPressed) // 아래로 내려가라
	{
		if (m_fMoveDeltaValue > 0.2)
		{
			m_bPressed = false;
		}
		else
		{
			m_fMoveDeltaValue += (_float)TimeDelta * fTriggerSpeed;
			m_vLocalPosition.y -= (_float)TimeDelta * fTriggerSpeed;
		}
	}

	if (m_bReleased) // 위로 올라와라
	{
		if (m_fMoveDeltaValue < 0)
		{
			m_bReleased = false;
		}
		else
		{
			m_fMoveDeltaValue -= (_float)TimeDelta * fTriggerSpeed;
			m_vLocalPosition.y += (_float)TimeDelta * fTriggerSpeed;
		}
	}

	if (m_bReleased || m_bPressed) {
		Set_WorldMatrix();
		m_pStaticActorCom->Update_StaticActor();
	}
}

CMoonBaboonCore_Button* CMoonBaboonCore_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMoonBaboonCore_Button* pInstance = new CMoonBaboonCore_Button(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboonCore_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboonCore_Button::Clone_GameObject(void* pArg)
{
	return this;
}

void CMoonBaboonCore_Button::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
