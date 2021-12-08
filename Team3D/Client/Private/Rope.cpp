#include "stdafx.h"
#include "..\public\Rope.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CRope::CRope(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRope::CRope(const CRope & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRope::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRope::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rope"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	ROPEDESC RopeDesc;
	if (nullptr != pArg)
	{
		memcpy(&RopeDesc, (ROPEDESC*)pArg, sizeof(ROPEDESC));
		m_iNumPlayer = RopeDesc.iNumPlayer;
	}

	if (m_iNumPlayer == 0) // Cody
	{
		m_pTargetTransformCom = ((CCody*)CDataStorage::GetInstance()->GetCody())->Get_Transform();
		if (nullptr == m_pTargetTransformCom)
			return E_FAIL;

		m_pTargetModel = ((CCody*)CDataStorage::GetInstance()->GetCody())->Get_Model();
		if (nullptr == m_pTargetModel)
			return E_FAIL;
	}
	else if (m_iNumPlayer == 1)// May
	{
		m_pTargetTransformCom = ((CMay*)CDataStorage::GetInstance()->GetMay())->Get_Transform();
		if (nullptr == m_pTargetTransformCom)
			return E_FAIL;

		m_pTargetModel = ((CMay*)CDataStorage::GetInstance()->GetMay())->Get_Model();
		if (nullptr == m_pTargetModel)
			return E_FAIL;
	}


	Safe_AddRef(m_pTargetTransformCom);
		(m_pTargetModel);

	// 초기 위치 세팅.

	return S_OK;
}

_int CRope::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_iNumPlayer == 0)
	{
		if (((CCody*)DATABASE->GetCody())->Get_IsHooking() == true)
		{
			_matrix HandsBone = m_pTargetModel->Get_BoneMatrix("RightAttach");
			_vector UFOTriggerPos = ((CCody*)DATABASE->GetCody())->Get_TriggerTargetPos();
			UFOTriggerPos = XMVectorSetY(UFOTriggerPos, UFOTriggerPos.m128_f32[1] + 1.f);
			UFOTriggerPos = XMVectorSetW(UFOTriggerPos, 1.f);

			_vector vPlayerPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);
			 
			_float vDist = XMVectorGetX(XMVector3Length(UFOTriggerPos - vPlayerPos));
			m_fSizeY = vDist;

			_float4x4 matWorld;
			XMStoreFloat4x4(&matWorld, /*XMMatrixRotationX(XMConvertToRadians(-90.f)) **/ XMMatrixScaling(100.f, 100.f, 100.f) * HandsBone * m_pTargetTransformCom->Get_WorldMatrix());

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(matWorld._41, matWorld._42, matWorld._43, 1.f));
			m_pTransformCom->Rotate_ToTarget(UFOTriggerPos);
			m_pTransformCom->Set_Scale(XMVectorSet(1.2f, 1.2f, m_fSizeY / 4.f, 0.f));
		}
	}
	else if (m_iNumPlayer == 1)
	{
		if (((CMay*)DATABASE->GetMay())->Get_IsHooking() == true)
		{
			_matrix HandsBone = m_pTargetModel->Get_BoneMatrix("RightAttach");
			_vector UFOTriggerPos = ((CMay*)DATABASE->GetMay())->Get_TriggerTargetPos();
			UFOTriggerPos = XMVectorSetY(UFOTriggerPos, UFOTriggerPos.m128_f32[1] + 1.f);
			UFOTriggerPos = XMVectorSetW(UFOTriggerPos, 1.f);

			_vector vPlayerPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

			_float vDist = XMVectorGetX(XMVector3Length(UFOTriggerPos - vPlayerPos));
			m_fSizeY = vDist;

			_float4x4 matWorld;
			XMStoreFloat4x4(&matWorld, /*XMMatrixRotationX(XMConvertToRadians(-90.f)) **/ XMMatrixScaling(100.f, 100.f, 100.f) * HandsBone * m_pTargetTransformCom->Get_WorldMatrix());

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(matWorld._41, matWorld._42, matWorld._43, 1.f));
			m_pTransformCom->Rotate_ToTarget(UFOTriggerPos);
			m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, m_fSizeY / 4.f, 0.f));
		}
	}


	return NO_EVENT;
}

_int CRope::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_iNumPlayer == 0)
	{
		if (((CCody*)DATABASE->GetCody())->Get_IsHooking() == true)
		{
			if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
				m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		}
	}
	else if (m_iNumPlayer == 1)
	{
		if (((CMay*)DATABASE->GetMay())->Get_IsHooking() == true)
		{
			if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
				m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

		}
	}

	return NO_EVENT;
}

HRESULT CRope::Render(RENDER_GROUP::Enum eGroup)
{
	if (m_iNumPlayer == 0)
	{
		if (((CCody*)DATABASE->GetCody())->Get_IsHooking() == true)
		{
			CGameObject::Render(eGroup);
			NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
			m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
			m_pModelCom->Set_DefaultVariables_Shadow();
			m_pModelCom->Render_Model(1, 0);
		}
	}
	else if (m_iNumPlayer == 1)
	{
		if (((CMay*)DATABASE->GetMay())->Get_IsHooking() == true)
		{
			CGameObject::Render(eGroup);
			NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
			m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
			m_pModelCom->Set_DefaultVariables_Shadow();
			m_pModelCom->Render_Model(1, 0);
		}
	}

	return S_OK;
}

HRESULT CRope::Render_ShadowDepth()
{
	if (m_iNumPlayer == 0)
	{
		if (((CCody*)DATABASE->GetCody())->Get_IsHooking() == true)
		{
			NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

			m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

			// Skinned: 2 / Normal: 3
			m_pModelCom->Render_Model(3, 0, true);
		}
	}
	else if (m_iNumPlayer == 1)
	{
		if (((CMay*)DATABASE->GetMay())->Get_IsHooking() == true)
		{
			NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

			m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

			// Skinned: 2 / Normal: 3
			m_pModelCom->Render_Model(3, 0, true);
		}
	}

	return S_OK;
}

void CRope::Launch_Rope(_double dTimeDelta)
{
}


CRope * CRope::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRope* pInstance = new CRope(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRope::Clone_GameObject(void * pArg)
{
	CRope* pInstance = new CRope(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRope::Free()
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}