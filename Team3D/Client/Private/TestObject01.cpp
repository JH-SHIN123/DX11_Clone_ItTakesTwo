#include "stdafx.h"
#include "..\public\TestObject01.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"

CTestObject01::CTestObject01(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTestObject01::CTestObject01(const CTestObject01 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestObject01::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTestObject01::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 1.f, 0.f, 1.f));

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eMOONBABOON, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = &PxSphereGeometry(0.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CTestObject01::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CTestObject01::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTestObject01::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CTestObject01::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eMOONBABOON, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eMOONBABOON, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

HRESULT CTestObject01::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CTestObject01 * CTestObject01::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTestObject01* pInstance = new CTestObject01(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTestObject01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTestObject01::Clone_GameObject(void * pArg)
{
	CTestObject01* pInstance = new CTestObject01(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTestObject01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestObject01::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}