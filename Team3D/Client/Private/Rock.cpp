#include "stdafx.h"
#include "..\Public\Rock.h"
#include "Cody.h"


CRock::CRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRock::CRock(const CRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRock::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRock::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CRock::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_fScale += (_float)dTimeDelta / 4.f;

	if (m_fMaxScale <= m_fScale)
		m_fScale = m_fMaxScale;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	_float fCodyY = XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fCodyY < fMyY)
	{
		_vector vCodyPos = m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vMyPos = {};
		XMStoreFloat3(&vMyPos, vCodyPos);

		vMyPos.x += (_float)(rand() % 61 - 30);
		vMyPos.y -= 50.f;
		vMyPos.z += (_float)(rand() % 61 - 30);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vMyPos.x, vMyPos.y, vMyPos.z, 1.f));

		m_fScale = 0.1f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

		_vector vScale, vRotQuat, vPosition;
		XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());
		m_pDynamicActorCom->Get_Actor()->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
		m_pDynamicActorCom->Get_Actor()->putToSleep();
	}

	return NO_EVENT;
}

_int CRock::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	//m_pDynamicActorCom->Update_DynamicActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRock::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

HRESULT CRock::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CRock::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pCodyTransformCom = ((CCody*)(DATABASE->GetCody()))->Get_Transform();
	Safe_AddRef(m_pCodyTransformCom);

	_float3 vMyPos = { 0.f, -500.f, 0.f };

	vMyPos.x += (_float)(rand() % 61 - 30);
	vMyPos.y -= (_float)(rand() % 50 + 20);
	vMyPos.z += (_float)(rand() % 61 - 30);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vMyPos.x, vMyPos.y, vMyPos.z, 1.f));

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
	m_pDynamicActorCom->Get_Actor()->putToSleep();

	//_vector vScale, vRotQuat, vPosition;
	//XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());
	//(m_pDynamicActorCom->Get_Actor())->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));

	return S_OK;
}

CRock * CRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRock* pInstance = new CRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRock::Clone_GameObject(void * pArg)
{
	CRock* pInstance = new CRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRock::Free()
{
	Safe_Release(m_pCodyTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDynamicActorCom);

	CGameObject::Free();
}
