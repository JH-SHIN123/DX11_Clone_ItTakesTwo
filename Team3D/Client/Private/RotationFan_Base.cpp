#include "stdafx.h"
#include "..\Public\RotationFan_Base.h"
#include "Cody.h"

CRotationFan_Base::CRotationFan_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CRotationFan_Base::CRotationFan_Base(const CRotationFan_Base & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CRotationFan_Base::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotationFan_Base::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(0.f, 50.f);

	return S_OK;
}

_int CRotationFan_Base::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	m_pTransformCom->RotateYaw_Speed(dTimeDelta);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());

	if (0 == lstrcmp(m_tDynamic_Env_Desc.szModelTag, TEXT("Component_Model_Saucer_RotatingPlatform_02")))
	{
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.2f);
		(m_pDynamicActorCom->Get_Actor())->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));
	}
	else
		(m_pDynamicActorCom->Get_Actor())->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));

	return NO_EVENT;
}

_int CRotationFan_Base::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotationFan_Base::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CRotationFan_Base::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRotationFan_Base::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICBOX, true, this);
	}
}

void CRotationFan_Base::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);
}

HRESULT CRotationFan_Base::Ready_Component(void * pArg)
{
	/* Dynamic */
	PxGeometry* Geom = new PxBoxGeometry(1.2f, 0.1f, 1.2f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);

	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		
	return S_OK;
}

CRotationFan_Base * CRotationFan_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotationFan_Base* pInstance = new CRotationFan_Base(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CRotationFan_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRotationFan_Base::Clone_GameObject(void * pArg)
{
	CRotationFan_Base* pInstance = new CRotationFan_Base(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CRotationFan_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotationFan_Base::Free()
{
	Safe_Release(m_pDynamicActorCom);

	CDynamic_Env::Free();
}
