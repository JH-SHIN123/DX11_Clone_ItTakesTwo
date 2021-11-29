#include "stdafx.h"
#include "..\Public\WarpGate.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "Effect_RespawnTunnel.h"
#include "Effect_RespawnTunnel_Portal.h"

CWarpGate::CWarpGate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWarpGate::CWarpGate(const CWarpGate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWarpGate::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWarpGate::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eStageValue, pArg, sizeof(STAGE_VALUE));

	Ready_Component();

	return S_OK;
}

_int CWarpGate::Tick(_double TimeDelta)
{
	return _int();
}

_int CWarpGate::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CWarpGate::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CWarpGate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	_matrix OutputPortal = Get_NextPortal_Matrix(m_eStageValue);


	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		((CMay*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eWARPGATE, true, OutputPortal);
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		((CMay*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eWARPGATE, false, OutputPortal);


	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eWARPGATE, true, OutputPortal);
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Matrix(GameID::Enum::eWARPGATE, false, OutputPortal);

}

HRESULT CWarpGate::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CWarpGate::Ready_Component()
{

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	//m_pTransformCom_Trigger = m_pTransformCom;
	//Safe_AddRef(m_pTransformCom_Trigger);

	Check_Stage_Value();

// 	_vector vPos = XMVectorSet(0.f, 3.f, 0.f, 1.f);
// 	_matrix vPhysWorld = m_pTransformCom->Get_WorldMatrix();
// 	vPhysWorld.r[3] += vPos;
// 	m_pTransformCom_Trigger->Set_WorldMatrix(vPhysWorld);

	m_UserData = USERDATA(GameID::eWARPGATE, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxBoxGeometry(5.f, 5.f, 0.1f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	CStaticActor::ARG_DESC Static_ArgDesc;
	Static_ArgDesc.pModel = m_pModelCom;
	Static_ArgDesc.pTransform = m_pTransformCom;
	Static_ArgDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &Static_ArgDesc), E_FAIL);

	//m_pRespawnTunnel_Portal
	EFFECT_DESC_CLONE Effect_Desc;
	XMStoreFloat4x4(&Effect_Desc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RespawnPortal"), Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel"), &Effect_Desc, (CGameObject**)&m_pRespawnTunnel), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_RespawnPortal"), Level::LEVEL_STAGE, TEXT("GameObject_3D_RespawnTunnel_Portal"), &Effect_Desc, (CGameObject**)&m_pRespawnTunnel_Portal), E_FAIL);

	return S_OK;
}

void CWarpGate::Check_Stage_Value()
{
	_vector vPos = XMVectorZero();
	_float	fDegree = 0.f;

	switch (m_eStageValue)
	{
	case Client::CWarpGate::MAIN_UMBRELLA:
		m_vWarpPos = {};
		vPos = XMVectorSet(31.f, 125.25f, 195.8f, 1.f);
		fDegree = 90.f;
		break;
	case Client::CWarpGate::STAGE_UMBRELLA:
		m_vWarpPos = {};
		vPos = XMVectorSet(-617.f, 754.f, 196.f, 1.f);
		fDegree = -90.f;
		break;
	case Client::CWarpGate::MAIN_PLANET:
		m_vWarpPos = {};
		vPos = XMVectorSet(97.8f, 125.25f, 195.8f, 1.f);
		fDegree = -90.f;
		break;
	case Client::CWarpGate::STAGE_PLANET:
		m_vWarpPos = {};
		vPos = XMVectorSet(617.f, 755.f, 196.2f, 1.f);
		fDegree = 90.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fDegree));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

_fmatrix CWarpGate::Get_NextPortal_Matrix(STAGE_VALUE eValue)
{
	_matrix NextPortalMatrix = XMMatrixIdentity();

	_vector vPos = XMVectorZero();
	_float	fDegree = 0.f;

	// 현재 스테이지 > 다음 스테이지
	switch (eValue)
	{
	case Client::CWarpGate::MAIN_UMBRELLA:
		vPos = XMVectorSet(-617.f, 754.f, 196.f, 1.f);
		fDegree = -90.f;
		break;
	case Client::CWarpGate::STAGE_UMBRELLA:
		vPos = XMVectorSet(31.f, 125.25f, 195.8f, 1.f);
		fDegree = 90.f;
		break;
	case Client::CWarpGate::MAIN_PLANET:
		vPos = XMVectorSet(617.f, 755.f, 196.2f, 1.f);
		fDegree = 90.f;
		break;
	case Client::CWarpGate::STAGE_PLANET:
		vPos = XMVectorSet(97.8f, 125.25f, 195.8f, 1.f);
		fDegree = -90.f;
		break;
	default:
		break;
	}

	vPos.m128_f32[1] += 5.f;

	NextPortalMatrix = XMMatrixRotationY(XMConvertToRadians(fDegree));
	NextPortalMatrix.r[3] = vPos;
	_vector vDir = NextPortalMatrix.r[2];
	NextPortalMatrix.r[3] += vDir;

	return NextPortalMatrix;
}

CWarpGate * CWarpGate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWarpGate* pInstance = new CWarpGate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWarpGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWarpGate::Clone_GameObject(void * pArg)
{
	CWarpGate* pInstance = new CWarpGate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWarpGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarpGate::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom_Trigger);
	Safe_Release(m_pRespawnTunnel);
	Safe_Release(m_pRespawnTunnel_Portal);

	__super::Free();
}
