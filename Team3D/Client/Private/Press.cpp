#include "stdafx.h"
#include "..\Public\Press.h"
#include "Cody.h"
#include "Effect_Generator.h"

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

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	XMStoreFloat3(&m_vOpenPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if(0 == m_tDynamic_Env_Desc.iOption)
		XMStoreFloat3(&m_vClosePos, vPos - vRight * 0.175f);
	else
		XMStoreFloat3(&m_vClosePos, vPos + vRight * 0.175f);

	return S_OK;
}

_int CPress::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);
	PlaySound_by_PlayerDistance();

	return NO_EVENT;
}

_int CPress::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook);
	vLook *= -0.5f;
	vLook = XMVectorSetY(vLook, XMVectorGetY(vLook) - 0.05f);
	m_pTriggerActorCom->Update_TriggerActor(vLook);

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
	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::ePRESS, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CPress::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);
}

void CPress::Movement(_double dTimeDelta)
{
	Close_Press(dTimeDelta);
	Open_Press(dTimeDelta);
}

void CPress::Open_Press(_double dTimeDelta)
{
	if (true == m_bSmash)
		return;

	m_dCoolTime += dTimeDelta;
	if (1.5 <= m_dCoolTime)
	{
		m_dDistance = 0.0;
		m_dCoolTime = 0.0;
		m_bSmash = true;
		m_bSoundCheck = true;
		return;
	}

	m_dDistance += dTimeDelta;

	if (0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->Go_Right_NoneSpeed(dTimeDelta);
	else
		m_pTransformCom->Go_Right_NoneSpeed(-dTimeDelta);

	if (0.175f <= m_dDistance)
	{
		_vector vOpenPos = XMVectorSetW(XMLoadFloat3(&m_vOpenPos), 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOpenPos);
	}
}

void CPress::Close_Press(_double dTimeDelta)
{
 	if (false == m_bSmash)
		return;

	m_dCoolTime += dTimeDelta;
	if (3.0 <= m_dCoolTime)
	{
		m_dCoolTime = 0.0;
		m_dDistance = 0.0;
		m_bSmash = false;
		m_bEffect = false;
		return;
	}

	m_dDistance += dTimeDelta * 4.0;

	if(0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->Go_Right_NoneSpeed(-(dTimeDelta * 4.0));
	else
		m_pTransformCom->Go_Right_NoneSpeed(dTimeDelta * 4.0);

	if (0.2f <= m_dDistance)
	{
		_vector vClosePos = XMVectorSetW(XMLoadFloat3(&m_vClosePos), 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vClosePos);

		Press_Effect();
	}
}

void CPress::Press_Effect()
{
	if (true == m_bEffect)
		return;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += XMVector3Normalize(WorldMatrix.r[1]) * 0.1f;
	WorldMatrix.r[3] += XMVector3Normalize(WorldMatrix.r[2]) * 0.75f;
	for (_int i = 0; i < 3; ++i)
	{
		EFFECT->Add_Effect(Effect_Value::UFO_Inside_PressWall_Particle, WorldMatrix);
		EFFECT->Add_Effect(Effect_Value::UFO_Inside_PressWall_Smoke, WorldMatrix);
		WorldMatrix.r[3] -= XMVector3Normalize(WorldMatrix.r[2]) * 0.75f;
	}	

	m_bEffect = true;
}

void CPress::PlaySound_by_PlayerDistance()
{
	if (false == m_bSoundCheck)
		return;

	_vector vPosition = ((CCody*)(DATABASE->GetCody()))->Get_Position();

	if (0.7f <= XMVectorGetY(vPosition) - XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		return;

	_float vDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPosition));

	_float fVolum = 1.5f - vDistance;
	if (0 >= fVolum)
		fVolum = 0.f;

	m_pGameInstance->Play_Sound(TEXT("InUFO_WallPress.wav"), CHANNEL_INUFO_WALLPRESS, fVolum * 1.5f);

	m_bSoundCheck = false;
}

HRESULT CPress::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* Geom = new PxBoxGeometry(0.01f, 0.23f, 0.48f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = Geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(Geom);

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
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
