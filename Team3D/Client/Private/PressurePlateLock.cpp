#include "stdafx.h"
#include "..\public\PressurePlateLock.h"
#include "Cody.h"
#include "May.h"
#include "Effect_Generator.h"

CPressurePlateLock::CPressurePlateLock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPressurePlateLock::CPressurePlateLock(const CPressurePlateLock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPressurePlateLock::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPressurePlateLock::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlateLock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));

	/* Option 0 : 아래로 도킹 되는 놈 / Option 1 : 위로 도킹 되는 놈 / Option 2 : BigPlate 지지대 */
	OptionSetting();

	m_UserData = USERDATA(GameID::ePRESSUREPLATELOCK, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CPressurePlateLock::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	LockActive(dTimeDelta);

	return NO_EVENT;
}

_int CPressurePlateLock::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPressurePlateLock::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPressurePlateLock::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CPressurePlateLock::Set_Position(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CPressurePlateLock::Set_LockActive(_bool IsCheck)
{
	m_IsLockActive = IsCheck;
}

HRESULT CPressurePlateLock::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPressurePlateLock::LockActive(_double TimeDelta)
{
	if (2 == m_iOption)
		return;

	if (true == m_IsLockActive)
	{
		_float fMaxScale = 2.f;

		_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
		_float fScaleY = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
		_float fScaleZ = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

		fScaleX += (_float)TimeDelta * 2.5f;

		if (fMaxScale <= fScaleX)
		{
			if (false == m_bConnectEffectOnce) 
			{
				_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
				matWorld.r[3].m128_f32[1] -= fMaxScale;
				EFFECT->Add_Effect(Effect_Value::PipeLocker_Connected, matWorld);
				m_bConnectEffectOnce = true;
			}
			return;
		}

		_vector vScale = { fScaleX, fScaleY, fScaleZ, 1.f };

		m_pTransformCom->Set_Scale(vScale);
	}
	else
	{
		_float fMaxScale = 0.f;

		_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
		_float fScaleY = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
		_float fScaleZ = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

		fScaleX -= (_float)TimeDelta * 2.5f;

		if (fMaxScale >= fScaleX)
			return;

		_vector vScale = { fScaleX, fScaleY, fScaleZ, 1.f };
		m_pTransformCom->Set_Scale(vScale);

		m_bConnectEffectOnce = false;
	}
}

void CPressurePlateLock::OptionSetting()
{
	if (0 == m_iOption)
	{
		_vector vScale = { 0.1f, m_pTransformCom->Get_Scale(CTransform::STATE_UP), m_pTransformCom->Get_Scale(CTransform::STATE_LOOK), 1.f };
		m_pTransformCom->Set_Scale(vScale);
	}
	else if (1 == m_iOption)
	{
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));

		_vector vScale = { 0.1f, m_pTransformCom->Get_Scale(CTransform::STATE_UP), m_pTransformCom->Get_Scale(CTransform::STATE_LOOK), 1.f };
		m_pTransformCom->Set_Scale(vScale);
	}
	else if (2 == m_iOption)
	{
	}
}

CPressurePlateLock * CPressurePlateLock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPressurePlateLock* pInstance = new CPressurePlateLock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPressurePlateLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPressurePlateLock::Clone_GameObject(void * pArg)
{
	CPressurePlateLock* pInstance = new CPressurePlateLock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPressurePlateLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPressurePlateLock::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}