#include "stdafx.h"
#include "..\public\DummyWall.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CDummyWall::CDummyWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDummyWall::CDummyWall(const CDummyWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyWall::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDummyWall::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_DummyWall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransformCom), E_FAIL);

	ROBOTDESC DummyWallDesc;
	if (nullptr != pArg)
		memcpy(&DummyWallDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, DummyWallDesc.vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 12.f, 1.f, 0.f));

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans) + 15.f, XMVectorGetZ(vTrans));
	m_pPhysxTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysxTransformCom->Set_WorldMatrix(PhysxWorldMatrix);


	// 트리거
	m_UserData = USERDATA(GameID::eDUMMYWALL, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransformCom;
	ArgDesc.pGeometry = new PxBoxGeometry(2.2f, 26.1f, 2.2f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	// 스태틱
	CStaticActor::ARG_DESC StaticActorDesc;
	StaticActorDesc.pModel = m_pModelCom;
	StaticActorDesc.pTransform = m_pTransformCom;
	StaticActorDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &StaticActorDesc), E_FAIL);

	return S_OK;
}

_int CDummyWall::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return NO_EVENT;
}


_int CDummyWall::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CDummyWall::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CDummyWall::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	 // Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eDUMMYWALL , true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eDUMMYWALL, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = false;
		m_PlayerID = GameID::eDUMMYWALL;
	}

	// May

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eDUMMYWALL, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
		m_PlayerID = GameID::eCODY;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eDUMMYWALL, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = false;
		m_PlayerID = GameID::eDUMMYWALL;
	}
}

HRESULT CDummyWall::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CDummyWall * CDummyWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDummyWall* pInstance = new CDummyWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDummyWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummyWall::Clone_GameObject(void * pArg)
{
	CDummyWall* pInstance = new CDummyWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDummyWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummyWall::Free()
{
	Safe_Release(m_pPhysxTransformCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}