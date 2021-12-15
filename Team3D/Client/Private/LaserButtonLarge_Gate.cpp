#include "stdafx.h"
#include "..\Public\LaserButtonLarge_Gate.h"
#include "LaserTennis_Manager.h"

CLaserButtonLarge_Gate::CLaserButtonLarge_Gate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CLaserButtonLarge_Gate::CLaserButtonLarge_Gate(const CLaserButtonLarge_Gate & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CLaserButtonLarge_Gate::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserButtonLarge_Gate::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(5.f, 0.f);
	XMStoreFloat4x4(&m_matResetWorld, m_pTransformCom->Get_WorldMatrix());

	LASERTENNIS->Add_LaserButtonLarge_Gate(this);

	return S_OK;
}

_int CLaserButtonLarge_Gate::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CLaserButtonLarge_Gate::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserButtonLarge_Gate::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CLaserButtonLarge_Gate::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserButtonLarge_Gate::Movement(_double dTimeDelta)
{
	if (true == m_bMovement)
	{
		/* Open */
		if (false == m_bActive)
		{
			m_fScale -= (_float)dTimeDelta * 3.f;
			m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, 1.f, m_fScale, 1.f));
			m_pTransformCom->Go_Backward(dTimeDelta);

			if (0.1f >= m_fScale)
			{
				m_fScale = 0.f;
				m_pTransformCom->Set_Scale(XMVectorSet(0.001f, 0.001f, 0.001f, 1.f));
				m_bActive = true;
				m_bMovement = false;
				LASERTENNIS->Active_LaserButtonLarge(true);
			}
		}
		/* Close */
		else
		{
			m_fScale += (_float)dTimeDelta * 3.f;
			m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, 1.f, m_fScale, 1.f));
			m_pTransformCom->Go_Straight(dTimeDelta);

			if (1.f <= m_fScale)
			{
				m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matResetWorld));
				m_bActive = false;
				m_bMovement = false;
			}
		}
	}
}

HRESULT CLaserButtonLarge_Gate::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CLaserButtonLarge_Gate * CLaserButtonLarge_Gate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserButtonLarge_Gate* pInstance = new CLaserButtonLarge_Gate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserButtonLarge_Gate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserButtonLarge_Gate::Clone_GameObject(void * pArg)
{
	CLaserButtonLarge_Gate* pInstance = new CLaserButtonLarge_Gate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserButtonLarge_Gate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserButtonLarge_Gate::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
