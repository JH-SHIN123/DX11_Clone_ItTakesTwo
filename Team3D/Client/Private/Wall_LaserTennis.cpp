#include "stdafx.h"
#include "..\Public\Wall_LaserTennis.h"
#include "LaserTennis_Manager.h"

CWall_LaserTennis::CWall_LaserTennis(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CWall_LaserTennis::CWall_LaserTennis(const CWall_LaserTennis & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CWall_LaserTennis::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CWall_LaserTennis::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(10.f, 0.f);
	m_fResetPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	LASERTENNIS->Add_Wall(this);

	return S_OK;
}

_int CWall_LaserTennis::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	if (false == m_bMovement)
		return NO_EVENT;

	/* 활성화 */
	if (false == m_bActivate)
	{
		if (m_fResetPosY + m_fDistance <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosY + m_fDistance));
			m_bActivate = true;
			m_bMovement = false;
			return NO_EVENT;
		}
		m_pTransformCom->Go_Right(dTimeDelta);
	}
	/* 비활성화 */
	else
	{
		if (m_fResetPosY >= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fResetPosY));
			m_bActivate = false;
			m_bMovement = false;
			return NO_EVENT;
		} 
		m_pTransformCom->Go_Left(dTimeDelta);
	}

	m_pStaticActorCom->Update_StaticActor();

	return NO_EVENT;
}

HRESULT CWall_LaserTennis::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CWall_LaserTennis * CWall_LaserTennis::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWall_LaserTennis* pInstance = new CWall_LaserTennis(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWall_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWall_LaserTennis::Clone_GameObject(void * pArg)
{
	CWall_LaserTennis* pInstance = new CWall_LaserTennis(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWall_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWall_LaserTennis::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
