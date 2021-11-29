#include "stdafx.h"
#include "..\public\BetteryBox.h"
#include "Cody.h"
#include "May.h"

CBetteryBox::CBetteryBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBetteryBox::CBetteryBox(const CBetteryBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBetteryBox::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBetteryBox::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_BetteryBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 0.f, 1.f));
	
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CBetteryBox::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CBetteryBox::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBetteryBox::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CBetteryBox::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CBetteryBox::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CBetteryBox * CBetteryBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBetteryBox* pInstance = new CBetteryBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBetteryBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBetteryBox::Clone_GameObject(void * pArg)
{
	CBetteryBox* pInstance = new CBetteryBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBetteryBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBetteryBox::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}