#include "stdafx.h"
#include "..\Public\Laser_LaserTennis.h"

CLaser_LaserTennis::CLaser_LaserTennis(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLaser_LaserTennis::CLaser_LaserTennis(const CLaser_LaserTennis & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLaser_LaserTennis::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_LaserTennis::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eLASER_LASERTENNIS;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(8.f, 0.f);
	m_pTransformCom->Set_Scale(XMVectorSet(15.f, 15.f, 15.f, 1.f));

	return S_OK;
}

_int CLaser_LaserTennis::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CLaser_LaserTennis::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	m_pTriggerActorCom->Update_TriggerActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaser_LaserTennis::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	if (m_eTarget == CLaserTennis_Manager::TARGET::TARGET_CODY)
		m_pModelCom->Render_Model(17, 0);
	else
		m_pModelCom->Render_Model(16, 0);

	return S_OK;
}

HRESULT CLaser_LaserTennis::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaser_LaserTennis::Movement(_double dTimeDelta)
{
	m_fCheckDistance += (_float)(dTimeDelta * m_pTransformCom->Get_SpeedPerSec());
	if (m_fDistance <= m_fCheckDistance)
		Set_Dead();

	m_pTransformCom->Go_Direction(XMLoadFloat3(&m_vDirection), dTimeDelta);
}

HRESULT CLaser_LaserTennis::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Laser_LaserTennis"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tArg.vPosition), 1.f));
	m_vDirection = tArg.vDirection;
	m_eTarget = tArg.eTarget;

	if (0 != m_vDirection.x)
		m_pTransformCom->RotateYaw_Angle(90.f);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition += XMLoadFloat3(&m_vDirection) * 2.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(5.f, 0.2f, 0.2f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

CLaser_LaserTennis * CLaser_LaserTennis::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_LaserTennis* pInstance = new CLaser_LaserTennis(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaser_LaserTennis::Clone_GameObject(void * pArg)
{
	CLaser_LaserTennis* pInstance = new CLaser_LaserTennis(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaser_LaserTennis::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
