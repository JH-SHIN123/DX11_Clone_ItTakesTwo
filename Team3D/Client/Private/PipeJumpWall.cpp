#include "stdafx.h"
#include "..\public\PipeJumpWall.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CPipeJumpWall::CPipeJumpWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPipeJumpWall::CPipeJumpWall(const CPipeJumpWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPipeJumpWall::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPipeJumpWall::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PipeJumpWall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransformCom), E_FAIL);

	ROBOTDESC PipeJumpWallDesc;
	if (nullptr != pArg)
		memcpy(&PipeJumpWallDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, PipeJumpWallDesc.vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 12.f, 1.f, 1.f));

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans));
	m_pPhysxTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysxTransformCom->Set_WorldMatrix(PhysxWorldMatrix);

	// 트리거
	m_UserData = USERDATA(GameID::ePIPEJUMPWALL, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransformCom;
	ArgDesc.pGeometry = new PxBoxGeometry(2.f, 17.4f, 2.f);

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

_int CPipeJumpWall::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}


_int CPipeJumpWall::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPipeJumpWall::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPipeJumpWall::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	 // Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePIPEJUMPWALL , true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePIPEJUMPWALL, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = false;
	}
}

HRESULT CPipeJumpWall::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CPipeJumpWall * CPipeJumpWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPipeJumpWall* pInstance = new CPipeJumpWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPipeJumpWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPipeJumpWall::Clone_GameObject(void * pArg)
{
	CPipeJumpWall* pInstance = new CPipeJumpWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPipeJumpWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeJumpWall::Free()
{
	Safe_Release(m_pPhysxTransformCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}