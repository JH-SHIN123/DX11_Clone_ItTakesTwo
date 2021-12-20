#include "stdafx.h"
#include "..\public\TutorialDoor.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "Effect_FireDoor.h"

CTutorialDoor::CTutorialDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTutorialDoor::CTutorialDoor(const CTutorialDoor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTutorialDoor::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTutorialDoor::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(0.3f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_DoorWay"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform_Trigger"), (CComponent**)&m_pTransformCom_Trigger, &CTransform::TRANSFORM_DESC(0.15f, XMConvertToRadians(90.f))), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.7f, 36.15f, 1.f)); // º¯°æµÈ Pos
	_vector vTriggerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vTriggerPos.m128_f32[1] += 11.f;// +2.;
	m_pTransformCom_Trigger->Set_State(CTransform::STATE_POSITION, vTriggerPos);
	CTriggerActor::ARG_DESC ArgDesc;


	m_UserData = USERDATA(GameID::eVERTICALDOOR, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom_Trigger;
	ArgDesc.pGeometry = new PxSphereGeometry(3.f);//1

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);

	CStaticActor::ARG_DESC ArgDesc_Static;
	ArgDesc_Static.pModel = m_pModelCom;
	ArgDesc_Static.pTransform = m_pTransformCom;
	ArgDesc_Static.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &ArgDesc_Static), E_FAIL);


	EFFECT_DESC_CLONE Effect_Data;
	XMStoreFloat4x4(&Effect_Data.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_FireDoor"), Level::LEVEL_STAGE, TEXT("GameObject_2D_FireDoor"), &Effect_Data, (CGameObject**)&m_pEffectFireDoor), E_FAIL);
	m_pEffectFireDoor->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	Safe_Delete(ArgDesc.pGeometry);
	return S_OK;
}

_int CTutorialDoor::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	//

	if (m_pGameInstance->Pad_Key_Down(DIP_RB) && m_IsCollide
		|| m_pGameInstance->Key_Down(DIK_O) && m_IsCollide)
	{
		m_bPull = true;
		m_IsNoGrab = false;
		UI_Delete(May, InputButton_InterActive);
	}


	if (m_pGameInstance->Pad_Key_Down(DIP_LB) || m_pGameInstance->Key_Down(DIK_I))
	{
		m_bPull = false;
		m_IsNoGrab = true;
	}

	if (true == m_IsCollide)
	{
		if (m_bPull == true)
		{
			m_IsPullMax = false;

			m_fMoveDist -= (_float)dTimeDelta;
			_float fMyPos_Y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
			if (1.5f >= fMyPos_Y)
			{
				m_pTransformCom->Go_Up(dTimeDelta);
				m_pTransformCom_Trigger->Go_Up(dTimeDelta);
				m_IsPullMax_Once = true;
			}
			else
			{
				if (true == m_IsPullMax_Once)
				{
					m_IsPullMax_Once = false;
					m_pEffectFireDoor->Update_Trigger_Position();
					m_pStaticActorCom->Update_StaticActor();
				}
			}
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			static_cast<CMay*>(CDataStorage::GetInstance()->GetMay())->Update_Tirgger_Pos(vPos);
		}		
	}

	if (m_IsNoGrab == true)
	{
		m_IsPullMax_Once = false;

		m_fMoveDist += (_float)dTimeDelta;
		_float fMyPos_Y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];

		if (0.7f <= fMyPos_Y)
		{
			m_pTransformCom->Go_Down(dTimeDelta);
			m_pTransformCom_Trigger->Go_Down(dTimeDelta);
			m_IsPullMax = true;
		}
		else
		{
			if (true == m_IsPullMax)
			{
				m_pEffectFireDoor->Update_Trigger_Position();
				m_pStaticActorCom->Update_StaticActor();
				m_IsPullMax = false;
				m_IsNoGrab = false;
			}
		}
	}

	m_pEffectFireDoor->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//UI_Generator->CreateInterActiveUI_AccordingRange(Player::May, UI::TutorialDoor, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f, m_IsCollide);

	return NO_EVENT;
}


_int CTutorialDoor::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 12.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CTutorialDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CTutorialDoor::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eVERTICALDOOR, true, vPos);
		//UI_Create(May, InputButton_InterActive);
		//UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, vPos);
		m_IsCollide = true;
		m_IsPullMax = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eVERTICALDOOR, false, vPos);
		m_IsCollide = false;
		/*UI_Delete(May, InputButton_InterActive);*/
	}
}

HRESULT CTutorialDoor::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CTutorialDoor::Pull_TutorialDoor(_double dTimeDelta)
{

}


CTutorialDoor * CTutorialDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTutorialDoor* pInstance = new CTutorialDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTutorialDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTutorialDoor::Clone_GameObject(void * pArg)
{
	CTutorialDoor* pInstance = new CTutorialDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTutorialDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorialDoor::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pEffectFireDoor);
	Safe_Release(m_pTransformCom_Trigger);

	CGameObject::Free();
}