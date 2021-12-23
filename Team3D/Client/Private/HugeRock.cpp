#include "stdafx.h"
#include "..\Public\HugeRock.h"
#include "Cody.h"
#include "EndingCredit_Manager.h"

CHugeRock::CHugeRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHugeRock::CHugeRock(const CHugeRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHugeRock::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CHugeRock::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CHugeRock::Tick(_double dTimeDelta)
{
	//if (true == m_isDead)
	//	return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	m_pTransformCom->Go_Down(dTimeDelta * 3.0);

	_float fMyPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fCodyY = XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fMyPosY > fCodyY - 24.f)
	{
		ENDINGCREDIT->Set_Dead_Environment();
		Create_Rocks();
		return EVENT_DEAD;
	}

	//m_fScale += (_float)dTimeDelta * 5.f;

	//if (m_fMaxScale <= m_fScale)
	//	m_fScale = m_fMaxScale;

	//_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
	//m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	//m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	return NO_EVENT;
}

_int CHugeRock::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	//m_pDynamicActorCom->Update_DynamicActor();
	//m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CHugeRock::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

//HRESULT CHugeRock::Render_ShadowDepth()
//{
//	CGameObject::Render_ShadowDepth();
//
//	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
//	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
//	/* Skinned: 2 / Normal: 3 */
//	m_pModelCom->Render_Model(3, 0, true);
//
//	return S_OK;
//}

//void CHugeRock::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
//{
//	CGameObject::Trigger(eStatus, eID, pGameObject);
//
//	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
//	{
//		ENDINGCREDIT->Set_Dead_Environment();
//		Create_Rocks();
//		Set_Dead();
//	}
//}

HRESULT CHugeRock::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pCodyTransformCom = ((CCody*)(DATABASE->GetCody()))->Get_Transform();
	Safe_AddRef(m_pCodyTransformCom);

	//_vector vCodyPosition = ((CCody*)(DATABASE->GetCody()))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//_vector vPosition = XMVectorSet(0.f, XMVectorGetY(vCodyPosition) - 85.f, 0.f, 0.f);

	_float fY;
	memcpy(&fY, pArg, sizeof(_float));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, fY - 24.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(17.f, 17.f, 17.f, 0.f));
	m_pTransformCom->Set_Rotaion(XMVectorSet(0.f, _float(rand() % 360), 0.f, 0.f));

	///* Dynamic */
	//PxGeometry* DynamicGeom = new PxSphereGeometry(0.5f);
	//CDynamicActor::ARG_DESC tDynamicActorArg;
	//tDynamicActorArg.pTransform = m_pTransformCom;
	//tDynamicActorArg.fDensity = 1.f;
	//tDynamicActorArg.pGeometry = DynamicGeom;
	//tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	//tDynamicActorArg.pUserData = &m_UserData;

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	//Safe_Delete(DynamicGeom);

	//m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	///* Trigger */
	//PxGeometry* TriggerGeom = new PxSphereGeometry(12.f);
	//CTriggerActor::ARG_DESC tTriggerArgDesc;
	//tTriggerArgDesc.pGeometry = TriggerGeom;
	//tTriggerArgDesc.pTransform = m_pTransformCom;
	//tTriggerArgDesc.pUserData = &m_UserData;

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	//Safe_Delete(TriggerGeom);

	return S_OK;
}

HRESULT CHugeRock::Create_Rocks()
{
	for (_uint i = 0; i < 100; ++i)
	{
		_vector vPositoin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_RockParticle"), &vPositoin), E_FAIL);
	}
	return S_OK;
}

CHugeRock * CHugeRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHugeRock* pInstance = new CHugeRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CHugeRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHugeRock::Clone_GameObject(void * pArg)
{
	CHugeRock* pInstance = new CHugeRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CHugeRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHugeRock::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pCodyTransformCom);

	//Safe_Release(m_pDynamicActorCom);
	//Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
