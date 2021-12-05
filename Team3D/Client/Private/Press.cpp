#include "stdafx.h"
#include "..\Public\Press.h"
#include "Cody.h"

CPress::CPress(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPress::CPress(const CPress & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPress::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPress::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePRESS;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CPress::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);


	if (m_pGameInstance->Key_Down(DIK_M))
		m_bSmash = true;

	Movement(dTimeDelta);
	return NO_EVENT;
}

_int CPress::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPress::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPress::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPress::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICBOX, true, this);
	}
}

void CPress::Movement(_double dTimeDelta)
{
	//Ready(dTimeDelta);
	Smesh(dTimeDelta);
}

void CPress::Ready(_double dTimeDelta)
{
	if (true == m_bSmash)
		return;

	m_pTransformCom->Set_Speed(1.f, 0.f);

	_double	dDis = dTimeDelta;
	m_dDistance += dDis;

	if (m_dDistance >= m_dSmeshDistance)
	{
		m_dDistance = 0.f;
		m_bSmash = true;
	}

	m_pTransformCom->Go_Straight(dDis);
}

void CPress::Smesh(_double dTimeDelta)
{
	if (false == m_bSmash)
		return;

	m_pTransformCom->Set_Speed(5.f, 0.f);

	_double	dDis = dTimeDelta;
	m_dDistance += dDis;

	if (m_dDistance >= m_dSmeshDistance)
	{
		m_dDistance = 0.0;
		m_bSmash = false;
	}
	m_pTransformCom->Go_Straight(-dDis);
}

HRESULT CPress::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CPress * CPress::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPress* pInstance = new CPress(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CPress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPress::Clone_GameObject(void * pArg)
{
	CPress* pInstance = new CPress(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CPress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPress::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
