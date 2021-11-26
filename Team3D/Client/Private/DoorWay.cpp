#include "stdafx.h"
#include "..\public\DoorWay.h"
#include "GameInstance.h"

CDoorWay::CDoorWay(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDoorWay::CDoorWay(const CDoorWay & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDoorWay::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();
	return S_OK;
}

HRESULT CDoorWay::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_DoorWay"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	
	m_UserData.eID = GameID::eDOORWAY;
	m_UserData.pGameObject = this;
	CStaticActor::tagArgumentDesc ActorArg;
	ActorArg.pTransform = m_pTransformCom;
	ActorArg.pUserData	= &m_UserData;
	ActorArg.pModel		= m_pModelCom;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &ActorArg), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.7f, 36.125f, 1.f));

	m_pStaticActorCom->Update_StaticActor();
	return S_OK;
}

_int CDoorWay::Tick(_double TimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.7f, 36.125f, 1.f));


	return NO_EVENT;
}

_int CDoorWay::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CDoorWay::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CDoorWay::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CDoorWay * CDoorWay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDoorWay* pInstance = new CDoorWay(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDoorWay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoorWay::Clone_GameObject(void * pArg)
{
	CDoorWay* pInstance = new CDoorWay(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDoorWay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoorWay::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	__super::Free();
}
