#include "stdafx.h"
#include "..\public\BigPlanet.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CBigPlanet::CBigPlanet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBigPlanet::CBigPlanet(const CBigPlanet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBigPlanet::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBigPlanet::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_HitPlanet"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransformCom), E_FAIL);

	ROBOTDESC PlanetDesc;
	if (nullptr != pArg)
		memcpy(&PlanetDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, PlanetDesc.vPosition);

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, PlanetDesc.vPosition.m128_f32[1] - 50.f);
	m_pPhysxTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pPhysxTransformCom->Set_WorldMatrix(PhysxWorldMatrix);


	CTriggerActor::ARG_DESC ArgDesc;
	m_UserData = USERDATA(GameID::ePLANET, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransformCom;
	ArgDesc.pGeometry = new PxCapsuleGeometry(7.5f, 290.f);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	CStaticActor::ARG_DESC Arg;
	Arg.pModel = m_pModelCom;
	Arg.pTransform = m_pTransformCom;
	Arg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &Arg), E_FAIL);


	return S_OK;
}

_int CBigPlanet::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide )
	{
		UI_Delete(Cody, InputButton_InterActive);
		if (((CCody*)DATABASE->GetCody())->Get_Player_Size() == CCody::SIZE_LARGE)
		{
			m_bLaunch = true;

			_vector vCodyPos = XMVectorSetY((((CCody*)DATABASE->GetCody())->Get_Transform()->Get_State(CTransform::STATE_POSITION)), 0.f);
			_vector vPlanetPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);

			_vector vDir = XMVector3Normalize(vCodyPos - vPlanetPos);
			_vector vUp = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_vector vRight = XMVector3Cross(vDir, vUp);

			XMStoreFloat3(&m_vRotateDir, vRight);
		}
	}

	if (m_bLaunch == true)
	{
		m_fDelay += (_float)dTimeDelta;
		if (m_fDelay <= 0.71f)
		{

		}
		else if (m_fDelay > 0.71f)
		{
			Pendulum(dTimeDelta);
		}
	}
	//m_pStaticActorCom->Update_StaticActor();
	return NO_EVENT;
}


_int CBigPlanet::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	InterActive_UI();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 105.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBigPlanet::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CBigPlanet::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePLANET, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		UI_Create(Cody, InputButton_InterActive);
		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollide = false;
		UI_Delete(Cody, InputButton_InterActive);
	}

	// May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		m_IsMayCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsMayCollide = false;
	}
}

HRESULT CBigPlanet::InterActive_UI()
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

HRESULT CBigPlanet::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CBigPlanet::Pendulum(_double dTimeDelta)
{
	m_fRotateTime += (_float)dTimeDelta;
	
	if (m_IsMayCollide == true)
	{
		((CMay*)DATABASE->GetMay())->Add_OffSet_Pos(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
	}

	if (m_fRotateTime < 1.f)
	{
		m_pTransformCom->Rotate_Axis(XMLoadFloat3(&m_vRotateDir), ((-dTimeDelta) * m_fSpeed) / 30.f);
		m_fSpeed -= (_float)dTimeDelta * 20.f;
	}
	else if (m_fRotateTime >= 1.f && m_fRotateTime < 2.2f)
	{
		//if(m_fRotateTime >= 1.f && m_fRotateTime < 1.5f)
			m_fSpeed += (_float)dTimeDelta * 14.69f;
		//else if (m_fRotateTime >= 1.5f && m_fRotateTime < 2.2f)
		//	m_fSpeed -= dTimeDelta * 5.f;
		 
		m_pTransformCom->Rotate_Axis(XMLoadFloat3(&m_vRotateDir), (dTimeDelta)* m_fSpeed / 30.f);
	}
	else if (m_fRotateTime >= 2.2f && m_fRotateTime < 2.4f)
	{
		m_fSpeed -= (_float)dTimeDelta * 120.f;
		m_pTransformCom->Rotate_Axis(XMLoadFloat3(&m_vRotateDir), -(dTimeDelta)* m_fSpeed / 30.f);
	}
	else if (m_fRotateTime >= 2.4f)
	{
		m_pTriggerCom->Update_TriggerActor();
		m_pStaticActorCom->Update_StaticActor();
		m_fSpeed = 20.f;
		m_fRotateTime = 0.f;
		m_IsCollide = false;
		m_bLaunch = false;
		m_fDelay = 0.f;
	}
}

CBigPlanet * CBigPlanet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBigPlanet* pInstance = new CBigPlanet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBigPlanet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBigPlanet::Clone_GameObject(void * pArg)
{
	CBigPlanet* pInstance = new CBigPlanet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBigPlanet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBigPlanet::Free()
{
	Safe_Release(m_pPhysxTransformCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}