#include "stdafx.h"
#include "..\Public\ElectricBox.h"
#include "Cody.h"
#include "Effect_Generator.h"

CElectricBox::CElectricBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CElectricBox::CElectricBox(const CElectricBox & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CElectricBox::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CElectricBox::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eELECTRICBOX;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CElectricBox::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);
	 
	if (true == m_bElectric)
	{
		m_dElectricTime += dTimeDelta;

		if (0.5f <= m_dElectricTime)
		{
			m_dElectricTime = 0.0;
			m_bElectric = false;
		}
	}

	/* Charge ¿Ã∆Â∆Æ */
	if ((m_iRandomTime - 1) <= m_dCoolTime && false == m_bCharge)
	{
		_matrix World = m_pTransformCom->Get_WorldMatrix();//XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(m_vOriginPos.x, m_vOriginPos.y - 1.f, m_vOriginPos.z);
		World.r[3] += World.r[1] *= 0.98f;

		for (_uint i = 0; i < 5; ++i)
			EFFECT->Add_Effect(Effect_Value::UFO_Inside_Battery_Particle, World);

		m_bCharge = true;
	}

	/* Electric ¿Ã∆Â∆Æ */
	if (m_iRandomTime <= m_dCoolTime)
	{
		_matrix World = m_pTransformCom->Get_WorldMatrix();//XMMatrixTranslation(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z);
		World.r[3] += World.r[1] *= 0.98f;
		EFFECT->Add_Effect(Effect_Value::UFO_Inside_Battery_Spark, World);
		EFFECT->Add_Effect(Effect_Value::May_Boots_Walking, World);
		EFFECT->Add_Effect(Effect_Value::UFO_Inside_Battery_Explosion, World);
		World.r[3] -= World.r[1] *= 0.98f;

		m_bElectric = true;
		m_iRandomTime = rand() % 3 + 3;
		m_dCoolTime = 0.0;
		m_bCharge = false;
	}

	m_dCoolTime += dTimeDelta;
	return NO_EVENT;
}

_int CElectricBox::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp = XMVector3Normalize(vUp);
	m_pTriggerActorCom->Update_TriggerActor(vUp);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CElectricBox::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CElectricBox::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CElectricBox::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	return;
	CDynamic_Env::Trigger(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICBOX, true, this);
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::eELECTRICBOX, false, this);
}

void CElectricBox::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);
}

HRESULT CElectricBox::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	/* Trigger */
	PxGeometry* Geom = new PxBoxGeometry(0.2f, 0.4f, 0.2f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = Geom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(Geom);

	XMStoreFloat3(&m_vOriginPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vOriginPos.y += 1.1f;

	m_iRandomTime = rand() % 3 + 3;

	return S_OK;
}

CElectricBox * CElectricBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CElectricBox* pInstance = new CElectricBox(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CElectricBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CElectricBox::Clone_GameObject(void * pArg)
{
	CElectricBox* pInstance = new CElectricBox(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CElectricBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CElectricBox::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
