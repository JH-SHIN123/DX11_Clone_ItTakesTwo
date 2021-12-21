#include "stdafx.h"
#include "..\Public\EjectionButton.h"
#include "Cody.h"
#include "UFO.h"

CEjectionButton::CEjectionButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CEjectionButton::CEjectionButton(const CEjectionButton & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CEjectionButton::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CEjectionButton::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eEJECTIONBUTTON;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_fMaxY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) - 0.06f;
	m_pTransformCom->Set_Speed(1.f, 0.f);

	return S_OK;
}

_int CEjectionButton::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if (true == m_bCollision)
	{
		_float fDist = (_float)dTimeDelta * 0.4f;
		m_fDistance += fDist;

		m_pTransformCom->Go_Down(dTimeDelta * 0.4f);

		if (0.06f <= m_fDistance)
		{
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, m_fMaxY));
			m_fDistance = 0.f;
			m_bPressed = true;
			m_bCollision = false;
			((CUFO*)DATABASE->Get_BossUFO())->Set_CutScene();
		}
	}

	return NO_EVENT;
}

_int CEjectionButton::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CEjectionButton::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CEjectionButton::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CEjectionButton::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);
}

void CEjectionButton::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);

	if (eStatus == ContactStatus::eFOUND && eID == GameID::Enum::eCODY && false == m_bPressed)
	{
		if (((((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Falling || (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land_Exit))
			m_bCollision = true;
	}
}

HRESULT CEjectionButton::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CEjectionButton * CEjectionButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEjectionButton* pInstance = new CEjectionButton(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CEjectionButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEjectionButton::Clone_GameObject(void * pArg)
{
	CEjectionButton* pInstance = new CEjectionButton(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CEjectionButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEjectionButton::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
