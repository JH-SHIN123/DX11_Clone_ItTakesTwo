#include "stdafx.h"
#include "..\public\Robot.h"
#include "Effect_Generator.h"

CRobot::CRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRobotParts(pDevice, pDeviceContext)
{
}

CRobot::CRobot(const CRobotParts & rhs)
	: CRobotParts(rhs)
{
}

HRESULT CRobot::NativeConstruct_Prototype()
{
	CRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobot::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_tRobotPartsDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	_vector vPosition = m_tRobotPartsDesc.vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	
	//ÇÇ¹þ YÃà
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tRobotPartsDesc.RotY);
	m_UserData = USERDATA(GameID::eROBOT, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);
	
	return S_OK;
}

_int CRobot::Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::RobotBattery_Spark, m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CRobot::Late_Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRobot::Render(RENDER_GROUP::Enum eGroup)
{
	CRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CRobot::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRobot::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

CRobot * CRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobot* pInstance = new CRobot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobot::Clone_GameObject(void * pArg)
{
	CRobot* pInstance = new CRobot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobot::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}