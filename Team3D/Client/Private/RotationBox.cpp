#include "stdafx.h"
#include "..\Public\RotationBox.h"
#include "Cody.h"

CRotationBox::CRotationBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CRotationBox::CRotationBox(const CRotationBox & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CRotationBox::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotationBox::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eELECTRICBOX;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CRotationBox::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	m_dCoolTime += dTimeDelta;

	return NO_EVENT;
}

_int CRotationBox::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotationBox::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CRotationBox::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRotationBox::Movement(_double dTimeDelta)
{
	//Rotate_Right(2.f, dTimeDelta * 20.f);
	//Rotate_Left(2.f, dTimeDelta * 20.f);
	//Random_Rotate(dTimeDelta);

	Rotate_Angle(5.f, dTimeDelta);
	Rotate_Random(dTimeDelta);

	m_pStaticActorCom->Update_StaticActor();
}

void CRotationBox::Rotate_Right(_float fMaxAngle, _double dTimeDelta)
{
	if (true == m_bSwitching || true == m_bTrigger)
		return;

	_float fAngle = (_float)dTimeDelta;

	if (false == m_bDirChange)
	{
		m_fAngle += fAngle;

		if (m_fAngle >= fMaxAngle)
		{
			m_fAngle = 0.f;
			fAngle = m_fAngle - fAngle;
			m_bDirChange = true;
		}
		m_pTransformCom->RotateRoll_Angle(fAngle);
	}
	else
	{
		m_fAngle += fAngle;

		if (m_fAngle >= fMaxAngle)
		{
			m_fAngle = 0.f;
			fAngle = m_fAngle - fAngle;
			m_bDirChange = false;
			m_bSwitching = true;

			if (m_dCoolTime >= 5.f)
			{
				m_iRandom = rand() % 2;
				m_bTrigger = true;
				m_dCoolTime = 0.f;
				return;
			}
		}
		m_pTransformCom->RotateRoll_Angle(-fAngle);
	}
}

void CRotationBox::Rotate_Left(_float fMaxAngle, _double dTimeDelta)
{
	if (false == m_bSwitching || true == m_bTrigger)
		return;

	_float fAngle = (_float)dTimeDelta;

	if (false == m_bDirChange)
	{
		m_fAngle -= fAngle;

		if (m_fAngle <= -fMaxAngle)
		{
			m_fAngle = 0.f;
			fAngle = m_fAngle - fAngle;
			m_bDirChange = true;
		}
		m_pTransformCom->RotateRoll_Angle(fAngle);
	}
	else
	{
		m_fAngle -= fAngle;

		if (m_fAngle <= -fMaxAngle)
		{
			m_fAngle = 0.f;
			fAngle = m_fAngle - fAngle;
			m_bDirChange = false;
			m_bSwitching = false;

			if (m_dCoolTime >= 5.f)
			{
				m_iRandom = rand() % 2;
				m_bTrigger = true;
				m_dCoolTime = 0.f;
				return;
			}
		}
		m_pTransformCom->RotateRoll_Angle(-fAngle);
	}
}

void CRotationBox::Random_Rotate(_double dTimeDelta)
{
	if (false == m_bTrigger)
		return;

	_float fAngle = (_float)dTimeDelta * 100.f;

	m_fAngle += fAngle;

	if (m_fAngle >= 90.f)
	{
		m_fAngle = 0.f;
		fAngle = m_fAngle - fAngle;
		m_bTrigger = false;
	}

	if (0 == m_iRandom)
		m_pTransformCom->RotateRoll_Angle(fAngle);
	else
		m_pTransformCom->RotateRoll_Angle(-fAngle);
}

void CRotationBox::Rotate_Angle(_float fMaxAngle, _double dTimeDelta)
{
	if (true == m_bRotate_Random)
		return;

	if (true == m_bRotate_Start)
		fMaxAngle *= 2.f;

	m_dRotate_Random += dTimeDelta;
	if (20.0 <= m_dRotate_Random)
	{
		if (true == m_bRotate_Left)
			m_pTransformCom->RotateRoll_Angle(m_fAngle);
		else
			m_pTransformCom->RotateRoll_Angle(-m_fAngle);

		m_bRotate_Random = true;
		m_dRotate_Random = 0.0;
		m_fAngle = 0.f;
		//m_iRandom = rand() % 2;
		m_iRandom = 0;
		return;
	}

	_float fAngle = (_float)dTimeDelta * (2.f * fMaxAngle);
	m_fAngle += fAngle;
	if (fMaxAngle <= m_fAngle)
	{
		m_bRotate_Left = !m_bRotate_Left;
		fAngle = m_fAngle - fMaxAngle;
		m_fAngle = 0.f;
		m_bRotate_Start = true;
	}
	if (true == m_bRotate_Left)
		m_pTransformCom->RotateRoll_Angle(-fAngle);
	else
		m_pTransformCom->RotateRoll_Angle(fAngle);
}

void CRotationBox::Rotate_Random(_double dTimeDelta)
{
	if (true == m_bRotate_Random)
		return;

	_float fAngle = (_float)dTimeDelta * (2.f * 90.f);

	m_fAngle += fAngle;

	if (m_fAngle >= 90.f)
	{
		m_bRotate_Left = true;
		m_bRotate_Start = false;
		m_bRotate_Random = false;
		fAngle = m_fAngle - 90.f;
		m_fAngle = 0.f;
	}

	if (0 == m_iRandom)
		m_pTransformCom->RotateRoll_Angle(fAngle);
	else
		m_pTransformCom->RotateRoll_Angle(-fAngle);
}

HRESULT CRotationBox::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CRotationBox * CRotationBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotationBox* pInstance = new CRotationBox(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CRotationBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRotationBox::Clone_GameObject(void * pArg)
{
	CRotationBox* pInstance = new CRotationBox(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CRotationBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotationBox::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
