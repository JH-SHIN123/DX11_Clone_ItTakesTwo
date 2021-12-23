#include "stdafx.h"
#include "..\Public\EndingRocket.h"
#include "DataStorage.h"
#include "UI_Generator.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"
#include "Effect_Generator.h"

CEndingRocket::CEndingRocket(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEndingRocket::CEndingRocket(const CEndingRocket & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEndingRocket::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEndingRocket::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(1.5f, XMConvertToRadians(120.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_EndingRocket"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(60.f, 0.f, 15.f, 1.f));
	m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));

	m_fCurSpeed = ENDING_ROCKET_SPEED;

	DATABASE->Set_EndingRocket(this);

	EFFECT->Add_Effect(Effect_Value::EndingRocket_Smoke, m_pTransformCom->Get_WorldMatrix());
	EFFECT->Add_Effect(Effect_Value::EndingRocket_Circle, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

_int CEndingRocket::Tick(_double dTimeDelta)
{
	if (m_pGameInstance->Get_CurrentLevelStep() == 2)
		m_bStartMove = true;

	if (m_bStartMove)
	{
		Movement(dTimeDelta);
		Ready_Players(dTimeDelta);
	}

	return _int();
}

_int CEndingRocket::Late_Tick(_double dTimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CEndingRocket::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, 0);

	return S_OK;
}

void CEndingRocket::Ready_Players(_double dTimeDelta)
{
	_vector vCodyOffset = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.7f) - (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.07f);
	vCodyOffset = XMVectorSetW(vCodyOffset, 1.f);
	((CCody*)DATABASE->GetCody())->Set_EndingRocketMatrix(m_pTransformCom->Get_WorldMatrix());
	((CCody*)DATABASE->GetCody())->Set_EndingRocketOffSetPos(vCodyOffset);

	_vector vMayOffset = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.11f) - (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.1f);
	vMayOffset = XMVectorSetW(vMayOffset, 1.f);
	((CMay*)DATABASE->GetMay())->Set_EndingRocketMatrix(m_pTransformCom->Get_WorldMatrix());
	((CMay*)DATABASE->GetMay())->Set_EndingRocketOffSetPos(vMayOffset);
}

void CEndingRocket::Movement(_double dTimeDelta)
{
	_float fTimeDelta = (_float)dTimeDelta;

	/* 부스트 */
	if (true == m_bBoost)
	{
		//if (0.0 == m_dBoostTime)
		//	EFFECT->Add_Effect(Effect_Value::EndingRocket_Boost);

		m_dBoostTime += dTimeDelta;

		if (2.0 <= m_dBoostTime)
		{
			m_bBoost = false;
			m_dBoostTime = 0.0;
		}
		m_pTransformCom->Go_Straight(fTimeDelta * m_fCurSpeed);
	}
	else
	{
		if (m_fCurSpeed > ENDING_ROCKET_SPEED)
			m_fCurSpeed -= fTimeDelta * 22.5f;
		else
			m_fCurSpeed = ENDING_ROCKET_SPEED;

		m_pTransformCom->Go_Straight(fTimeDelta * m_fCurSpeed);
	}

	/* 키조작 */
	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		if (m_fUp < 0.7f)
		{
			m_fDown -= fTimeDelta;
			m_fUp += fTimeDelta;
			m_pTransformCom->Go_Up(fTimeDelta * 4.f);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		if (m_fLeft < 1.3f)
		{
			m_fRight -= fTimeDelta;
			m_fLeft += fTimeDelta;
			m_pTransformCom->Go_Left(fTimeDelta * 4.f);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (m_fDown < 0.7f)
		{
			m_fUp -= fTimeDelta;
			m_fDown += fTimeDelta;
			m_pTransformCom->Go_Down(fTimeDelta * 4.f);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		if (m_fRight < 1.3f)
		{
			m_fLeft -= fTimeDelta;
			m_fRight += fTimeDelta;
			m_pTransformCom->Go_Right(fTimeDelta * 4.f);
		}
	}
}

HRESULT CEndingRocket::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

CEndingRocket * CEndingRocket::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEndingRocket* pInstance = new CEndingRocket(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CEndingRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEndingRocket::Clone_GameObject(void * pArg)
{
	CEndingRocket* pInstance = new CEndingRocket(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEndingRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEndingRocket::Free()
{
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
