#include "stdafx.h"
#include "..\public\StarBuddy.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"
#include "Effect_Generator.h"
#include "Effect_StarBuddy_Move.h"

CStarBuddy::CStarBuddy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStarBuddy::CStarBuddy(const CStarBuddy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStarBuddy::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStarBuddy::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	ROBOTDESC StarDesc;
	if (nullptr != pArg)
		memcpy(&StarDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	if (0 == StarDesc.iStageNum)
		m_eInterActiveID = UI::StarBuddy0;
	else
		m_eInterActiveID = UI::StarBuddy1;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, StarDesc.vPosition);

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eSTARBUDDY, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxSphereGeometry(1.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	CStaticActor::ARG_DESC StaticActorDesc;
	StaticActorDesc.pModel = m_pModelCom;
	StaticActorDesc.pTransform = m_pTransformCom;
	StaticActorDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &StaticActorDesc), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddyEffect"), Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy_Move"), nullptr, (CGameObject**)&m_pMoveEffect), E_FAIL);

	return S_OK;
}

_int CStarBuddy::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
	m_pTransformCom->RotatePitch(dTimeDelta * 0.2f);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCodyCollide ||
		m_pGameInstance->Key_Down(DIK_O) && m_IsMayCollide ||
		m_pGameInstance->Pad_Key_Down(DIP_Y) && m_IsMayCollide)
	{
		m_bLaunch = true;
		UI_Delete(May, InputButton_PS_InterActive);
		UI_Delete(Cody, InputButton_InterActive);
	}

	else if (m_bLaunch == true)
	{
		m_fLifeTime += (_float)dTimeDelta;

		if (m_fLifeTime > 0.71f)
		{
			Launch_StarBuddy(dTimeDelta);
			m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
			m_pTransformCom->RotatePitch(dTimeDelta * 1.2f);
			if (m_bSoundOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_INTERACTIVE_STAR_EXPLODE, m_fStarBuddy_Volume);
				m_pGameInstance->Play_Sound(TEXT("Interactive_Start_Explode.wav"), CHANNEL_INTERACTIVE_STAR_EXPLODE, m_fStarBuddy_Volume);
				m_bSoundOnce = true;
			}	
			m_pMoveEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
		}
		if (m_fLifeTime > 1.9f)
		{
			EFFECT->Add_Effect(Effect_Value::StarBuddy_Explosion_Pillar, m_pTransformCom->Get_WorldMatrix());
			m_pMoveEffect->Set_Dead();
			return EVENT_DEAD;
		}
	}

	UI_Generator->CreateInterActiveUI_AccordingRange(Player::Cody, m_eInterActiveID,
		m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f, m_IsCodyCollide, m_bLaunch);

	UI_Generator->CreateInterActiveUI_AccordingRange(Player::May, m_eInterActiveID,
		m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f, m_IsMayCollide, m_bLaunch);


	return NO_EVENT;
}

_int CStarBuddy::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CStarBuddy::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CStarBuddy::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eSTARBUDDY , true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCodyCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCodyCollide = false;
		m_PlayerID = GameID::eSTARBUDDY;
	}

	// May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eSTARBUDDY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsMayCollide = true;
		m_PlayerID = GameID::eMAY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsMayCollide = false;
		m_PlayerID = GameID::eSTARBUDDY;
	}
}

HRESULT CStarBuddy::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CStarBuddy::Launch_StarBuddy(_double dTimeDelta)
{
	// 실제로 상호작용 할땐 Player -> StarBuddy Dir 방향으로 이동
	if (m_bSaveOnce == false)
	{
		m_vSavedPlayerPos = XMVectorZero();
		if (m_PlayerID == GameID::eCODY)
			m_vSavedPlayerPos = XMVectorSetY(((CCody*)DATABASE->GetCody())->Get_Position(), 0.f);
		else if (m_PlayerID == GameID::eMAY)
			m_vSavedPlayerPos = XMVectorSetY(((CMay*)DATABASE->GetMay())->Get_Position(), 0.f);

		_vector vStarPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
		m_vMoveDirection = XMVectorSetY(XMVector3Normalize(vStarPos - m_vSavedPlayerPos), 0.f);

		m_bSaveOnce = true;
	}


	m_pTransformCom->MoveToDir(m_vMoveDirection, dTimeDelta * 15.f);

	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), dTimeDelta * 4.f);
	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * 4.f);
}

CStarBuddy * CStarBuddy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStarBuddy* pInstance = new CStarBuddy(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStarBuddy::Clone_GameObject(void * pArg)
{
	CStarBuddy* pInstance = new CStarBuddy(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStarBuddy::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMoveEffect);

	CGameObject::Free();
}