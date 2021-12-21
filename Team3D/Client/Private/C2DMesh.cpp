#include "stdafx.h"
#include "..\Public\C2DMesh.h"
#include "Cody.h"
#include "EndingCredit_Manager.h"

C2DMesh::C2DMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

C2DMesh::C2DMesh(const C2DMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT C2DMesh::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT C2DMesh::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int C2DMesh::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	if (true == ENDINGCREDIT->Get_Dead_Environment())
		Set_Dead();

	m_iRandomModel = ENDINGCREDIT->Get_RandomModel();

	Movement(dTimeDelta);

	m_dCoolTime += dTimeDelta;
	return NO_EVENT;
}

_int C2DMesh::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	m_pDynamicActorCom->Update_DynamicActor();
	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	m_pTriggerActorCom->Update_TriggerActor();

	switch (m_iRandomModel)
	{
	case 0:
		if (0 < m_pModelCom_Ailen->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
		break;
	case 1:
		if (0 < m_pModelCom_Robot->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
		break;
	case 2:
		if (0 < m_pModelCom_Star->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
		break;
	case 3:
		if (0 < m_pModelCom_UFO->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
		break;
	case 4:
		if (0 < m_pModelCom_Umbrella->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
		break;
	}

	return NO_EVENT;
}

HRESULT C2DMesh::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	_float4 vColor;
	switch (m_iColorIndex)
	{
	case 0: vColor = _float4(1.f, 1.f, 1.f, 1.f);
		break;
	case 1: vColor = _float4(0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f);
		break;
	}

	switch (m_iRandomModel)
	{
	case 0:
		m_pModelCom_Ailen->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Ailen->Set_DefaultVariables_Shadow();
		m_pModelCom_Ailen->Sepd_Bind_Buffer();
		m_pModelCom_Ailen->Set_Variable("g_vColor", &vColor, sizeof(_float4));
		m_pModelCom_Ailen->Sepd_Render_Model(0, 29, false);
		break;
	case 1:
		m_pModelCom_Robot->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Robot->Set_DefaultVariables_Shadow();
		m_pModelCom_Robot->Sepd_Bind_Buffer();
		m_pModelCom_Robot->Set_Variable("g_vColor", &vColor, sizeof(_float4));
		m_pModelCom_Robot->Sepd_Render_Model(0, 29, false);
		break;
	case 2:
		m_pModelCom_Star->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Star->Set_DefaultVariables_Shadow();
		m_pModelCom_Star->Sepd_Bind_Buffer();
		m_pModelCom_Star->Set_Variable("g_vColor", &vColor, sizeof(_float4));
		m_pModelCom_Star->Sepd_Render_Model(0, 29, false);
		break;
	case 3:
		m_pModelCom_UFO->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_UFO->Set_DefaultVariables_Shadow();
		m_pModelCom_UFO->Sepd_Bind_Buffer();
		m_pModelCom_UFO->Set_Variable("g_vColor", &vColor, sizeof(_float4));
		m_pModelCom_UFO->Sepd_Render_Model(0, 29, false);
		break;
	case 4:
		m_pModelCom_Umbrella->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Umbrella->Set_DefaultVariables_Shadow();
		m_pModelCom_Umbrella->Sepd_Bind_Buffer();
		m_pModelCom_Umbrella->Set_Variable("g_vColor", &vColor, sizeof(_float4));
		m_pModelCom_Umbrella->Sepd_Render_Model(0, 29, false);
		break;
	}
	return S_OK;
}

HRESULT C2DMesh::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	switch (m_iRandomModel)
	{
	case 0:
		m_pModelCom_Ailen->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Ailen->Render_Model(3, 0, true);
		break;
	case 1:
		m_pModelCom_Robot->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Robot->Render_Model(3, 0, true);
		break;
	case 2:
		m_pModelCom_Star->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Star->Render_Model(3, 0, true);
		break;
	case 3:
		m_pModelCom_UFO->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_UFO->Render_Model(3, 0, true);
		break;
	case 4:
		m_pModelCom_Umbrella->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
		m_pModelCom_Umbrella->Render_Model(3, 0, true);
		break;
	}
	return S_OK;
}

void C2DMesh::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		/* 충돌시 랜덤하게 Force값 조정 */
		_int iRandom = rand() % 5 - 2;
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3((_float)iRandom, 0.f, (_float)iRandom));

		m_iColorIndex = 1;
		m_bCollision = true;
	}
}

void C2DMesh::Movement(_double dTimeDelta)
{
	/* 천천히 올라오게 Force조절 */
	m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(0.f, -0.01f, 0.f));

	/* 충돌시 회전값 상승 */
	if (false == m_bCollision)
		m_pDynamicActorCom->Get_Actor()->setAngularVelocity(PxVec3(m_fRandomAngle, m_fRandomAngle, m_fRandomAngle));
	else
		m_pDynamicActorCom->Get_Actor()->setAngularVelocity(PxVec3(m_fRandomAngle * 5000.f, m_fRandomAngle * 5000.f, m_fRandomAngle * 5000.f));

	/* 스케일 조정 */
	m_fScale += (_float)dTimeDelta / 3.f;

	if (m_fMaxScale <= m_fScale)
		m_fScale = m_fMaxScale;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	/* 코디보다 위에 있을 경우 다시 아래에서 리스폰 */
	_float fCodyY = XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fCodyY + 10.f < fMyY)
	{
		_vector vCodyPos = m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vMyPos = {};
		XMStoreFloat3(&vMyPos, vCodyPos);
		vMyPos.x = 0.f;
		vMyPos.z = 0.f;

		vMyPos.x += (_float)(rand() % 61 - 30);
		vMyPos.y -= 50.f;
		vMyPos.z += (_float)(rand() % 61 - 30);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vMyPos.x, vMyPos.y, vMyPos.z, 1.f));

		m_fScale = 0.1f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));
		m_pDynamicActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION))));

		m_iColorIndex = 0;
		m_bCollision = false;
	}
}

HRESULT C2DMesh::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_2DMesh_Ailen"), TEXT("Com_Model0"), (CComponent**)&m_pModelCom_Ailen), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_2DMesh_Robot"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom_Robot), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_2DMesh_Umbrella"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom_Umbrella), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_2DMesh_Star"), TEXT("Com_Model3"), (CComponent**)&m_pModelCom_Star), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_2DMesh_UFO"), TEXT("Com_Model4"), (CComponent**)&m_pModelCom_UFO), E_FAIL);

	m_pCodyTransformCom = ((CCody*)(DATABASE->GetCody()))->Get_Transform();
	Safe_AddRef(m_pCodyTransformCom);

	/* 랜덤 생성 범위 */
	_float3 vMyPos = { 0.f, XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f };

	vMyPos.x += (_float)(rand() % 61 - 30);
	vMyPos.y -= (_float)(rand() % 50 + 50);
	vMyPos.z += (_float)(rand() % 61 - 30);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vMyPos.x, vMyPos.y, vMyPos.z, 1.f));

	/* 랜덤 회전값 */
	m_fRandomAngle = (rand() % 41 - 20.f) * 0.1f;

	/* Dynamic */
	PxGeometry* DynamicGeom = new PxSphereGeometry(0.1f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = DynamicGeom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(DynamicGeom);

	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	m_pDynamicActorCom->Get_Actor()->setAngularDamping(0.1f);

	/* Trigger */
	PxGeometry* TriggerGeom = new PxSphereGeometry(2.5f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

C2DMesh * C2DMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	C2DMesh* pInstance = new C2DMesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - C2DMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * C2DMesh::Clone_GameObject(void * pArg)
{
	C2DMesh* pInstance = new C2DMesh(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - C2DMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void C2DMesh::Free()
{
	Safe_Release(m_pModelCom_UFO);
	Safe_Release(m_pModelCom_Ailen);
	Safe_Release(m_pModelCom_Robot);
	Safe_Release(m_pModelCom_Star);
	Safe_Release(m_pModelCom_Umbrella);

	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pCodyTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDynamicActorCom);

	CGameObject::Free();
}
