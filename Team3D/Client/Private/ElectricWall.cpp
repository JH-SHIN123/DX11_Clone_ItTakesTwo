#include "stdafx.h"
#include "..\Public\ElectricWall.h"
#include "Cody.h"
#include "Effect_Generator.h"

CElectricWall::CElectricWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CElectricWall::CElectricWall(const CElectricWall & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CElectricWall::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CElectricWall::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eELECTRICWALL;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CElectricWall::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	/*¡ÿ∫Ò ¿Ã∆Â∆Æ*/
	if (3.75 <= m_dCoolTime && true == m_bElectric_Ready)
	{
		m_bElectric_Ready = false;
		for (_uint i = 0; i < 8; ++i)
		{
			_matrix World = m_pTransformCom->Get_WorldMatrix();
			World.r[3] += XMVector3Normalize(World.r[1]) * (-0.25f * i) + XMVector3Normalize(World.r[2]) * 0.28f;
			EFFECT->Add_Effect(Effect_Value::UFO_Inside_ElectricWall_Particle, World);
		}
	}

	/* Electric ¿Ã∆Â∆Æ */
	if (5.0 <= m_dCoolTime)
	{
		m_bElectric = true;

		if (2 > m_iEffectCount)
		{
			++m_iEffectCount;
			for (_uint i = 0; i < 2; ++i)
			{
				_matrix World = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) 
					* XMMatrixTranslation(m_vOriginPos.x, m_vOriginPos.y + (i * 1.f) + 0.65f, m_vOriginPos.z + 0.0125f);
				EFFECT->Add_Effect(Effect_Value::UFO_Inside_ElectricWall_Spark, World);
			}
			for (_uint i = 0; i < 8; ++i)
			{
				_matrix World = m_pTransformCom->Get_WorldMatrix();
				World.r[3] += XMVector3Normalize(World.r[1]) * (-0.25f * i) + XMVector3Normalize(World.r[2]) * 0.28f;

				EFFECT->Add_Effect(Effect_Value::UFO_Inside_ElectricWall_Particle, World);
				EFFECT->Add_Effect(Effect_Value::UFO_Inside_ElectricWall_Explosion, World);
				EFFECT->Add_Effect(Effect_Value::UFO_Inside_ElectricWall_Explosion, World);
			}
		}

		if (5.75 <= m_dCoolTime)
		{
			m_bElectric = false;
			m_bElectric_Ready = true;
			m_dCoolTime = 0.0;

			m_dElectricReady_Time = 0.0;
			m_iEffectCount = 0;
		}
	}

	m_dElectricReady_Time += dTimeDelta;
	m_dCoolTime += dTimeDelta;

	return NO_EVENT;
}

_int CElectricWall::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CElectricWall::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CElectricWall::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CElectricWall::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICWALL, true, this);
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICWALL, false, this);
}

void CElectricWall::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);
}

HRESULT CElectricWall::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* Geom = new PxBoxGeometry(0.18f, 0.7f, 0.05f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = Geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(Geom);

	_vector vPosition, vLook, vUp;
	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition += (vLook * 0.27f);
	vPosition += (vUp * -0.7f);
	m_pTriggerActorCom->Get_Actor()->setGlobalPose(PxTransform(MH_PxVec3(vPosition)));

	vPosition -= (vLook * 0.02f);
	XMStoreFloat3(&m_vOriginPos, vPosition);
	m_vOriginPos.y -= 1.5f;

	return S_OK;
}

CElectricWall * CElectricWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CElectricWall* pInstance = new CElectricWall(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CElectricWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CElectricWall::Clone_GameObject(void * pArg)
{
	CElectricWall* pInstance = new CElectricWall(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CElectricWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CElectricWall::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
