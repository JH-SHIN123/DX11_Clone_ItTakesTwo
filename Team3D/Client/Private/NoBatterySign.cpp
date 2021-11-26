#include "stdafx.h"
#include "..\public\NoBatterySign.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "RobotHead.h"

CNoBatterySign::CNoBatterySign(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CNoBatterySign::CNoBatterySign(const CNoBatterySign & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNoBatterySign::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CNoBatterySign::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_NoBatterySign"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 2.2f, 15.3f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(1.f, 0.f, 0.f ,0.f), XMConvertToRadians(-90.f));

	CStaticActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eNOBATTERYSIGN, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	DATABASE->Set_NoBatterySignPtr(this);

	return S_OK;
}

_int CNoBatterySign::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_bBatteryCharged == false && m_bHitLever == true)
	{
		m_fRotateTime += (_float)dTimeDelta;

		if (m_fRotateTime < 0.2f)
			m_pTransformCom->Rotate_Axis(XMVectorSet(1.f, 0.f, 0.f, 0.f), dTimeDelta * 7.f);
		else if (m_fRotateTime > 0.4f && m_fRotateTime < 0.6f)
			m_pTransformCom->Rotate_Axis(XMVectorSet(1.f, 0.f, 0.f, 0.f), -dTimeDelta * 7.f);
		else if (m_fRotateTime >= 0.6f)
		{
			m_pTransformCom->Set_RotateAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			m_fRotateTime = 0.f;
			m_bHitLever = false;
		}
	}


	return NO_EVENT;
}

_int CNoBatterySign::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CNoBatterySign::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CNoBatterySign::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CNoBatterySign::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CNoBatterySign * CNoBatterySign::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNoBatterySign* pInstance = new CNoBatterySign(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CNoBatterySign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNoBatterySign::Clone_GameObject(void * pArg)
{
	CNoBatterySign* pInstance = new CNoBatterySign(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CNoBatterySign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoBatterySign::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}