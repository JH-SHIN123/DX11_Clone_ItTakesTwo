#include "stdafx.h"
#include "..\Public\Pedal.h"
#include "Cody.h"
#include "MoonBaboon_SpaceShip.h"

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
	XMStoreFloat4x4(&m_matResetWorld, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

_int CPedal::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	PlaySound_by_PlayerDistance();
	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CPedal::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vRight = XMVector3Normalize(vRight);

	m_pTriggerActorCom->Update_TriggerActor(vRight * -0.4f);

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

void CPedal::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePEDAL, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CPedal::Movement(_double dTimeDelta)
{
	m_fProgressAnim = ((CMoonBaboon_SpaceShip*)(DATABASE->Get_MoonBaboon_SpaceShip()))->Get_Model()->Get_ProgressAnim();

	float fAngle = 0.f;
	float fRotateAngle = 0.f;

	if (0.05f >= m_fProgressAnim)
	{
		m_bSoundOnePlay = false;
		m_fAddAngle = 0.f;
		m_pTransformCom->RotateRoll_Angle((dTimeDelta * 30.f));
	}
	else if (0.1f >= m_fProgressAnim)
	{
		if (false == m_bSoundOnePlay)
		{
			m_bSoundCheck = false;
			m_bSoundOnePlay = true;
		}
	}
	else if (0.5f >= m_fProgressAnim)
	{
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matResetWorld));
	}
	else
	{
		fAngle = ((m_fProgressAnim - 0.5f) * 2.f * 30.f);
		fRotateAngle = fAngle - m_fAddAngle;
		m_fAddAngle = fAngle;
		m_pTransformCom->RotateRoll_Angle(-fRotateAngle);
	}
}

void CPedal::PlaySound_by_PlayerDistance()
{
	if (true == m_bSoundCheck)
		return;

	_vector vPosition = ((CCody*)(DATABASE->GetCody()))->Get_Position();

	if (0.7f <= XMVectorGetY(vPosition) - XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		return;

	_float vDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPosition));

	_float fVolum = 1.5f - vDistance;
	if (0 >= fVolum)
		fVolum = 0.f;

	if (false == m_bSoundCheck)
	{
		m_pGameInstance->Play_Sound(TEXT("InUFO_MoonBaboonStep.wav"), CHANNEL_INUFO_PEDAL, fVolum * 1.5f);
		m_bSoundCheck = true;
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

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(0.35f, 0.01f, 0.23f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

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
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
