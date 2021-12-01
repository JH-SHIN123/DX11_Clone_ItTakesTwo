#include "stdafx.h"
#include "..\Public\PinBall_HandleBase.h"

CPinBall_HandleBase::CPinBall_HandleBase(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPinBall_HandleBase::CPinBall_HandleBase(const CPinBall_HandleBase & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPinBall_HandleBase::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPinBall_HandleBase::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);
	
	m_fRespawnPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CDataStorage::GetInstance()->Set_Pinball_HandleBase(this);

	return S_OK;
}

_int CPinBall_HandleBase::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	return NO_EVENT;
}

_int CPinBall_HandleBase::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPinBall_HandleBase::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPinBall_HandleBase::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPinBall_HandleBase::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CPinBall_HandleBase::PlayerMove()
{
	_float fX = XMVectorGetX(CDataStorage::GetInstance()->GetMay()->Get_Position()) + 1.f;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetX(vPos, fX);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pStaticActorCom->Update_StaticActor();
}

void CPinBall_HandleBase::Respawn_Pos(_double dTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + (3.f * (_float)dTimeDelta));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pStaticActorCom->Update_StaticActor();

	if (0.1f >= (m_fRespawnPosX - XMVectorGetX(vPos)))
	{
		vPos = XMVectorSetX(vPos, m_fRespawnPosX);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pStaticActorCom->Update_StaticActor();
	}
}

CPinBall_HandleBase * CPinBall_HandleBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPinBall_HandleBase* pInstance = new CPinBall_HandleBase(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPinBall_HandleBase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPinBall_HandleBase::Clone_GameObject(void * pArg)
{
	CPinBall_HandleBase* pInstance = new CPinBall_HandleBase(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPinBall_HandleBase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPinBall_HandleBase::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
