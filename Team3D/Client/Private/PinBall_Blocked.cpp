#include "stdafx.h"
#include "..\Public\PinBall_Blocked.h"

_float	CPInBall_Blocked::m_fUpPosY = 0.f;
_float	CPInBall_Blocked::m_fDownPosY = 0.f;
_bool	CPInBall_Blocked::m_IsSwitching = false;

CPInBall_Blocked::CPInBall_Blocked(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPInBall_Blocked::CPInBall_Blocked(const CPInBall_Blocked & rhs)
	: CDynamic_Env(rhs)
{
}

void CPInBall_Blocked::Switching()
{	
	m_IsSwitching = !m_IsSwitching;
}

HRESULT CPInBall_Blocked::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPInBall_Blocked::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eBLOCKED;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);
	m_pTransformCom->Set_Speed(5.f, 0.f);

	if (0 == lstrcmp(TEXT("Component_Model_Space_Pinball_Blockade01"), m_tDynamic_Env_Desc.szModelTag))
		m_fUpPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	else if (0 == lstrcmp(TEXT("Component_Model_Space_Pinball_Blockade02"), m_tDynamic_Env_Desc.szModelTag))
		m_fDownPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CDataStorage::GetInstance()->Set_Pinball_Blocked(this);

	return S_OK;
}

_int CPInBall_Blocked::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CPInBall_Blocked::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPInBall_Blocked::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CPInBall_Blocked::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPInBall_Blocked::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CPInBall_Blocked::Movement(_double dTimeDelta)
{
	if (0 == lstrcmp(TEXT("Component_Model_Space_Pinball_Blockade01"), m_tDynamic_Env_Desc.szModelTag))
	{
		if (true == m_IsSwitching)
		{
			if (m_fDownPosY < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Down(dTimeDelta);
		}
		else
		{
			if (m_fUpPosY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Up(dTimeDelta);
		}
	}
	else if (0 == lstrcmp(TEXT("Component_Model_Space_Pinball_Blockade02"), m_tDynamic_Env_Desc.szModelTag))
	{
		if (true == m_IsSwitching)
		{
			if (m_fUpPosY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Up(dTimeDelta);
		}
		else
		{
			if (m_fDownPosY < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Down(dTimeDelta);
		}
	}
	else if (0 == lstrcmp(TEXT("Component_Model_Space_Pinball_Blockade_Half"), m_tDynamic_Env_Desc.szModelTag))
	{
		if (true == m_IsSwitching)
		{
			if (m_fUpPosY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Up(dTimeDelta);
		}
		else
		{
			if (m_fDownPosY < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
				m_pTransformCom->Go_Down(dTimeDelta);
		}
	}

	m_pStaticActorCom->Update_StaticActor();
}

CPInBall_Blocked * CPInBall_Blocked::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPInBall_Blocked* pInstance = new CPInBall_Blocked(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPInBall_Blocked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPInBall_Blocked::Clone_GameObject(void * pArg)
{
	CPInBall_Blocked* pInstance = new CPInBall_Blocked(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPInBall_Blocked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPInBall_Blocked::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
