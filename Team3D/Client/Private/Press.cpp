#include "stdafx.h"
#include "..\Public\Press.h"
#include "Cody.h"

CPress::CPress(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CPress::CPress(const CPress & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CPress::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPress::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::ePRESS;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	//m_fOpenPos = XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//m_fClosePos = m_fOpenPos - XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	XMStoreFloat3(&m_vOpenPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if(0 == m_tDynamic_Env_Desc.iOption)
		XMStoreFloat3(&m_vClosePos, vPos - vRight * 0.27f);
	else
		XMStoreFloat3(&m_vClosePos, vPos + vRight * 0.27f);

	return S_OK;
}

_int CPress::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);
	return NO_EVENT;
}

_int CPress::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPress::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CPress::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPress::OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CDynamic_Env::OnContact(eStatus, eID, pGameObject);

	/* Cody */
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY && true == m_bSmash)
	{
		((CCody*)pGameObject)->SetTriggerID_Ptr(GameID::Enum::ePRESS, true, this);
	}
}

void CPress::Movement(_double dTimeDelta)
{
	Close_Press(dTimeDelta);
	Open_Press(dTimeDelta);
}

void CPress::Open_Press(_double dTimeDelta)
{
	if (true == m_bSmash)
		return;

	m_dCoolTime += dTimeDelta;
	if (5.0 <= m_dCoolTime)
	{
		m_dDistance = 0.0;
		m_dCoolTime = 0.0;
		m_bSmash = true;
		return;
	}

	m_dDistance += dTimeDelta;

	if (0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->Go_Right_NoneSpeed(dTimeDelta);
	else
		m_pTransformCom->Go_Right_NoneSpeed(-dTimeDelta);

	if (0.27 <= m_dDistance)
	{
		_vector vOpenPos = XMVectorSetW(XMLoadFloat3(&m_vOpenPos), 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOpenPos);
	}

	//m_dCoolTime += dTimeDelta;
	//if (3.0 <= m_dCoolTime)
	//{
	//	m_dCoolTime = 0.0;
	//	m_bSmash = true;
	//	return;
	//}

	//m_pTransformCom->Go_Straight_NoneSpeed(dTimeDelta);

	//if (0 > XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK)))
	//{
	//	if (m_fOpenPos >= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	//	{
	//		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//		vPos = XMVectorSetZ(vPos, m_fOpenPos);

	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	//	}
	//}
	//else
	//{
	//	if (m_fOpenPos <= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	//	{
	//		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//		vPos = XMVectorSetZ(vPos, m_fOpenPos);

	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	//	}
	//}
}

void CPress::Close_Press(_double dTimeDelta)
{
 	if (false == m_bSmash)
		return;

	m_dCoolTime += dTimeDelta;
	if (3.0 <= m_dCoolTime)
	{
		m_dCoolTime = 0.0;
		m_dDistance = 0.0;
		m_bSmash = false;
		return;
	}

	m_dDistance += dTimeDelta * 3.0;

	if(0 == m_tDynamic_Env_Desc.iOption)
		m_pTransformCom->Go_Right_NoneSpeed(-(dTimeDelta * 3.0));
	else
		m_pTransformCom->Go_Right_NoneSpeed(dTimeDelta * 3.0);

	if (0.27 <= m_dDistance)
	{
		_vector vClosePos = XMVectorSetW(XMLoadFloat3(&m_vClosePos), 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vClosePos);
	}

	//m_dCoolTime += dTimeDelta;
	//if (5.0 <= m_dCoolTime)
	//{
	//	m_dCoolTime = 0.0;
	//	m_bSmash = false;
	//	return;
	//}

	//m_pTransformCom->Go_Straight_NoneSpeed(-(dTimeDelta * 5.f));

	//if (0 < XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK)))
	//{
	//	if (m_fClosePos >= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	//	{
	//		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//		vPos = XMVectorSetZ(vPos, m_fClosePos);

	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	//	}
	//}
	//else
	//{
	//	if (m_fClosePos <= XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	//	{
	//		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//		vPos = XMVectorSetZ(vPos, m_fClosePos);

	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	//	}
	//}
}

HRESULT CPress::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CPress * CPress::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPress* pInstance = new CPress(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CPress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPress::Clone_GameObject(void * pArg)
{
	CPress* pInstance = new CPress(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CPress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPress::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
