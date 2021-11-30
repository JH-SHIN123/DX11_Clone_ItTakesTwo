#include "stdafx.h"
#include "..\public\PipeCurve.h"
#include "Cody.h"
#include "May.h"

CPipeCurve::CPipeCurve(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPipeCurve::CPipeCurve(const CPipeCurve & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPipeCurve::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPipeCurve::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PipeCurve"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	/* Option 0 : 처음에 회전 안먹은거 / Option 1 : 회전 먹은거 ㅇㅇ */
	if (1 == m_iOption)
	{
		m_fAngle = 90.f;
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fAngle));
	}

	m_UserData = USERDATA(GameID::ePIPECURVE, this);
	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CPipeCurve::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_float fRotateMaxAngle = 90.f;

	if (true == m_IsRotate)
	{
		m_fAngle += (_float)dTimeDelta * 100.f;
		m_fAngleIncreaseLimit += (_float)dTimeDelta * 100.f;

		if (fRotateMaxAngle <= m_fAngleIncreaseLimit)
		{
			_float fAngleDifference = m_fAngleIncreaseLimit - fRotateMaxAngle;
			m_fAngle -= fAngleDifference;
			m_IsRotate = false;

			if (0 == m_iOption && 175.f <= m_fAngle && 185.f >= m_fAngle)
				m_IsConnected = true;
			else if (1 == m_iOption && 355.f <= m_fAngle && 365.f >= m_fAngle)
				m_IsConnected = true;
			else
				m_IsConnected = false;

			CompareAngle(m_fAngle);
		}

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fAngle));

	}
	else
		m_fAngleIncreaseLimit = 0.f;

	return NO_EVENT;
}

_int CPipeCurve::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPipeCurve::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
 	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPipeCurve::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CPipeCurve::Set_Position(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CPipeCurve::Set_Rotate(_bool IsCheck)
{
	m_IsRotate = IsCheck;
}

_bool CPipeCurve::Get_Connected()
{
	return m_IsConnected;
}

HRESULT CPipeCurve::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CPipeCurve::CompareAngle(_float fAngle)
{
	if (0 == m_iOption)
	{
		_float fCompareAngle = fabs(fAngle - 360.f);

		if (0.1f >= fCompareAngle)
			m_fAngle = 0.f;
	}
	else if (1 == m_iOption)
	{
		_float fCompareAngle = fabs(fAngle - 360.f);

		if (0.1f >= fCompareAngle)
			m_fAngle = 0.f;
	}
}

CPipeCurve * CPipeCurve::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPipeCurve* pInstance = new CPipeCurve(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPipeCurve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPipeCurve::Clone_GameObject(void * pArg)
{
	CPipeCurve* pInstance = new CPipeCurve(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPipeCurve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeCurve::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}