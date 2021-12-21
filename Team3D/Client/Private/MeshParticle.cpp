#include "stdafx.h"
#include "..\Public\MeshParticle.h"


CMeshParticle::CMeshParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMeshParticle::CMeshParticle(const CMeshParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshParticle::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMeshParticle::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CMeshParticle::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	if (1.f <= m_dCoolTime)
		Set_Dead();

	m_dCoolTime += dTimeDelta;
	return NO_EVENT;
}

_int CMeshParticle::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	PxTransform pxTrans = m_pDynamicActorCom->Get_Actor()->getGlobalPose();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pxTrans.p.x, pxTrans.p.y, pxTrans.p.z, 1.f));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CMeshParticle::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Sepd_Bind_Buffer();

	_float4 vColor;
	switch (m_iColorType)
	{
	case 0:
		vColor = _float4(1.f, 0.f, 0.f, 1.f);
		break;
	case 1:
		vColor = _float4(0.f, 1.f, 0.2f, 1.f);
		break;
	case 2:
		vColor = _float4(1.f, 1.f, 0.f, 1.f);
		break;
	}
	m_pModelCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pModelCom->Sepd_Render_Model(0, 29, false);

	return S_OK;
}

HRESULT CMeshParticle::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CMeshParticle::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_LaserBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	_float iScale = rand() % 2 + 1.f;
	m_pTransformCom->Set_Scale(XMVectorSet(iScale, iScale, iScale, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tArg.vPosition), 1.f));

	/* Dynamic */
	PxGeometry* DynamicGeom = new PxSphereGeometry(0.01f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = DynamicGeom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(DynamicGeom);

	/* ·£´ý Force°ª */
	_int iForce = rand() % 210 - 100;
	m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(iForce * 0.00001f, 0.00001f, iForce * 0.00001f));

	if (0 == tArg.iColorType)
		m_iColorType = 0;
	else
		m_iColorType = rand() % 2 + 1;

	return S_OK;
}

CMeshParticle * CMeshParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMeshParticle* PInstance = new CMeshParticle(pDevice, pDeviceContext);

	if (FAILED(PInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMeshParticle");
		Safe_Release(PInstance);
	}
	return PInstance;
}

CGameObject * CMeshParticle::Clone_GameObject(void * pArg)
{
	CMeshParticle* PInstance = new CMeshParticle(*this);

	if (FAILED(PInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMeshParticle");
		Safe_Release(PInstance);
	}
	return PInstance;
}

void CMeshParticle::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDynamicActorCom);

	CGameObject::Free();
}
