#include "stdafx.h"
#include "..\public\ControlRoom_Glass.h"
#include "Cody.h"
#include "May.h"

CControlRoom_Glass::CControlRoom_Glass(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CControlRoom_Glass::CControlRoom_Glass(const CControlRoom_Glass & rhs)
	: CGameObject(rhs)
{
}

HRESULT CControlRoom_Glass::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControlRoom_Glass::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Glass"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(67.465f, 218.752f, 223.54f, 1.f));

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CControlRoom_Glass::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	return NO_EVENT;
}

_int CControlRoom_Glass::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);


	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);

	return NO_EVENT;
}

HRESULT CControlRoom_Glass::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(5);

	return S_OK;
}

void CControlRoom_Glass::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

CControlRoom_Glass * CControlRoom_Glass::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControlRoom_Glass* pInstance = new CControlRoom_Glass(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControlRoom_Glass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CControlRoom_Glass::Clone_GameObject(void * pArg)
{
	CControlRoom_Glass* pInstance = new CControlRoom_Glass(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControlRoom_Glass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlRoom_Glass::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}