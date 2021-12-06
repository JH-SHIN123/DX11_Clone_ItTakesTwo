#include "stdafx.h"
#include "..\Public\Pedal.h"
#include "Cody.h"

CPedal::CPedal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPedal::CPedal(const CPedal & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPedal::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPedal::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePEDAL;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	//m_pTransformCom->RotateRoll_Angle(-45.f);
	return S_OK;
}

_int CPedal::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);
	m_pStaticActorCom->Update_StaticActor();

	return NO_EVENT;
}

_int CPedal::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPedal::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPedal::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPedal::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::ePEDAL, true, this);
	}
}

void CPedal::Movement(_double dTimeDelta)
{
	if (false == m_bSmash)
	{
		_double dAngle = (dTimeDelta * ( 0.5f * 45.f));
		if (45.f <= m_dAngle + dAngle)
		{
			m_bSmash = !m_bSmash;
			dAngle = 45.f - m_dAngle;
			m_dAngle = 0.0;
		}
		else
			m_dAngle += dAngle;
		m_pTransformCom->RotateRoll_Angle(-dAngle);
	}
	else
	{
		_double dAngle = (dTimeDelta * (10.f * 45.f));
		if (45.f <= m_dAngle + dAngle)
		{
			m_bSmash = !m_bSmash;
			dAngle = 45.f - m_dAngle;
			m_dAngle = 0.0;
		}
		else
			m_dAngle += dAngle;
		m_pTransformCom->RotateRoll_Angle(dAngle);
	}
}

HRESULT CPedal::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CPedal * CPedal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPedal* pInstance = new CPedal(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CPedal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPedal::Clone_GameObject(void * pArg)
{
	CPedal* pInstance = new CPedal(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CPedal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPedal::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
