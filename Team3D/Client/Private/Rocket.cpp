#include "stdafx.h"
#include "..\public\Rocket.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CRocket::CRocket(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRocket::CRocket(const CRocket & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRocket::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRocket::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	ROBOTDESC RocketDesc;
	if (nullptr != pArg)
		memcpy(&RocketDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, RocketDesc.vPosition);

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eROCKET, this);
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

	return S_OK;
}

_int CRocket::Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide == true)
	{
		m_bLaunch = true;
		UI_Delete(May, InputButton_InterActive);
		UI_Delete(Cody, InputButton_InterActive);
	}
	if (m_bLaunch == true)
	{
		m_fLifeTime += (_float)dTimeDelta;

		if(m_fLifeTime > 0.65f)
			Launch_Rocket(dTimeDelta);

		if (m_fLifeTime > 3.5f)
			return EVENT_DEAD;
	}

	return NO_EVENT;
}

_int CRocket::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRocket::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

void CRocket::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eROCKET, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollide = false;
		UI_Delete(Cody, InputButton_InterActive);
		m_PlayerID = GameID::eROCKET;
	}

	//May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eROCKET, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(May, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eMAY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsCollide = false;
		UI_Delete(May, InputButton_InterActive);
		m_PlayerID = GameID::eROCKET;
	}
}

HRESULT CRocket::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

void CRocket::Launch_Rocket(_double dTimeDelta)
{
	m_fUpAcceleration += (_float)dTimeDelta * 0.2f;

	//m_pTransformCom->Go_Straight(TimeDelta);

	if (m_fUpAcceleration < 0.092f)
	{
		// 실제로 상호작용 할땐 Player <-> Rocket Dir 을 축으로 회전해야함.
		_vector vPlayerPos = XMVectorZero();
		if (m_PlayerID == GameID::eCODY)
			vPlayerPos = XMVectorSetY(((CCody*)DATABASE->GetCody())->Get_Position(), 0.f);
		else if (m_PlayerID == GameID::eMAY)
			vPlayerPos = XMVectorSetY(((CMay*)DATABASE->GetMay())->Get_Position(), 0.f);


		_vector vRocketPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
		_vector vDir = XMVector3Normalize(vPlayerPos - vRocketPos);
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f ,0.f);

		_vector vRight = XMVector3Normalize(XMVector3Cross(vDir, vUp));
		m_pTransformCom->Rotate_Axis(vRight, dTimeDelta * 1.75f);
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (m_fUpAcceleration - 0.06f) * (m_fUpAcceleration - 0.06f)/*/ 4.f*/);

	}

	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), m_fUpAcceleration);
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (m_fUpAcceleration - 0.06f) * (m_fUpAcceleration - 0.06f)/*/ 4.f*/);
	m_pTransformCom->Go_Up(m_fUpAcceleration / 6.f);
}


CRocket * CRocket::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRocket* pInstance = new CRocket(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRocket::Clone_GameObject(void * pArg)
{
	CRocket* pInstance = new CRocket(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRocket::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}