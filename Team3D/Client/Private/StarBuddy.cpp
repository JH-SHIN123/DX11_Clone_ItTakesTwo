#include "stdafx.h"
#include "..\public\StarBuddy.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

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

	return S_OK;
}

_int CStarBuddy::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
	m_pTransformCom->RotatePitch(dTimeDelta * 0.2f);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide)
	{
		m_bLaunch = true;
		UI_Delete(May, InputButton_InterActive);
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
		}
		if (m_fLifeTime > 3.5f)
			return EVENT_DEAD; // 
	}


	return NO_EVENT;
}


_int CStarBuddy::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	InterActive_UI();

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
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollide = false;
		UI_Delete(Cody, InputButton_InterActive);
		m_PlayerID = GameID::eSTARBUDDY;
	}

	// May

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eSTARBUDDY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(May, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsCollide = false;
		UI_Delete(May, InputButton_InterActive);
		m_PlayerID = GameID::eSTARBUDDY;
	}
}

HRESULT CStarBuddy::InterActive_UI()
{
	CCody* pCody = (CCody*)DATABASE->GetCody();
	NULL_CHECK_RETURN(pCody, E_FAIL);
	CMay* pMay = (CMay*)DATABASE->GetMay();
	NULL_CHECK_RETURN(pMay, E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCodyComparePos = vPos - vCodyPos;
	_vector vMayComparePos = vPos - vMayPos;

	_float fRange = 20.f;

	_float vCodyComparePosX = abs(XMVectorGetX(vCodyComparePos));
	_float vCodyComparePosZ = abs(XMVectorGetZ(vCodyComparePos));

	if (fRange >= vCodyComparePosX && fRange >= vCodyComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::Cody, UI::InputButton_Dot))
			UI_Create(Cody, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(Cody, InputButton_Dot);

	_float vMayComparePosX = abs(XMVectorGetX(vMayComparePos));
	_float vMayComparePosZ = abs(XMVectorGetZ(vMayComparePos));

	if (fRange >= vMayComparePosX && fRange >= vMayComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::May, UI::InputButton_Dot))
			UI_Create(May, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(May, InputButton_Dot);

	return S_OK;
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
	_vector vPlayerPos = XMVectorZero();
	if (m_PlayerID == GameID::eCODY)
		vPlayerPos = XMVectorSetY(((CCody*)DATABASE->GetCody())->Get_Position(), 0.f);
	else if (m_PlayerID == GameID::eMAY)
		vPlayerPos = XMVectorSetY(((CMay*)DATABASE->GetMay())->Get_Position(), 0.f);


	_vector vStarPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
	_vector vDir = XMVector3Normalize(vStarPos - vPlayerPos);

	m_pTransformCom->MoveToDir(vDir, dTimeDelta * 5.f);

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

	CGameObject::Free();
}