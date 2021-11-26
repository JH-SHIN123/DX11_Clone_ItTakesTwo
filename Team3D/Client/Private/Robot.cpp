#include "stdafx.h"
#include "..\public\Robot.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include "Effect_Generator.h"

CRobot::CRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRobot::CRobot(const CRobot & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobot::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobot::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 20.f, 1.f));

	return S_OK;
}

_int CRobot::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::RobotBattery_Spark, m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CRobot::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CRobot::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CRobot::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}