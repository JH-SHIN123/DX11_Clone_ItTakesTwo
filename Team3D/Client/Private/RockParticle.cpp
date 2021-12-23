#include "stdafx.h"
#include "..\Public\RockParticle.h"

CRockParticle::CRockParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRockParticle::CRockParticle(const CRockParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRockParticle::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRockParticle::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	//m_UserData.eID = GameID::eENVIRONMENT;
	//m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CRockParticle::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	//m_pTransformCom->Go_Down(dTimeDelta * 20.0);

	if (20.f <= m_dCoolTime)
		Set_Dead();

	m_dCoolTime += dTimeDelta;

	return NO_EVENT;
}

_int CRockParticle::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	//m_pDynamicActorCom->Update_DynamicActor();
	//m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRockParticle::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

//HRESULT CRockParticle::Render_ShadowDepth()
//{
//	CGameObject::Render_ShadowDepth();
//
//	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
//	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
//	/* Skinned: 2 / Normal: 3 */
//	m_pModelCom->Render_Model(3, 0, true);
//
//	return S_OK;
//}

HRESULT CRockParticle::Ready_Component(void * pArg)
{
	_vector vPosition = XMVectorZero();

	if (nullptr != pArg)
	{
		memcpy(&vPosition, pArg, sizeof(_vector));
	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_fScale = (rand() % 5 + 1.f) * 0.1f;
	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 11 - 5), XMVectorGetY(vPosition), _float(rand() % 3 - 1), 1.f));

	///* Dynamic */
	//PxGeometry* DynamicGeom = new PxSphereGeometry(0.05f);
	//CDynamicActor::ARG_DESC tDynamicActorArg;
	//tDynamicActorArg.pTransform = m_pTransformCom;
	//tDynamicActorArg.fDensity = 1.f;
	//tDynamicActorArg.pGeometry = DynamicGeom;
	//tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	//tDynamicActorArg.pUserData = &m_UserData;

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	//Safe_Delete(DynamicGeom);

	///* ·£´ý Force°ª */
	//_float fForce = (rand() % 21 - 10) * 0.1f;
	//m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(fForce, fForce, fForce));

	return S_OK;
}

CRockParticle * CRockParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRockParticle* pInstance = new CRockParticle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRockParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRockParticle::Clone_GameObject(void * pArg)
{
	CRockParticle* pInstance = new CRockParticle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRockParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRockParticle::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	//Safe_Release(m_pDynamicActorCom);

	CGameObject::Free();
}
