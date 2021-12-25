#include "stdafx.h"
#include "..\Public\SlideDoor.h"

_bool		CSlideDoor::m_bOpen = false;

CSlideDoor::CSlideDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CSlideDoor::CSlideDoor(const CSlideDoor & rhs)
	: CDynamic_Env(rhs)
{
}

void CSlideDoor::Open_Door()
{
	/* Sound */
	m_pGameInstance->Stop_Sound(CHANNEL_PINBALL_DOOR);
	m_pGameInstance->Play_Sound(TEXT("Pinball_Door_Open.wav"), CHANNEL_PINBALL_DOOR);

	m_bOpen = true;
}

HRESULT CSlideDoor::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSlideDoor::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Speed(2.f, 0.f);
	DATABASE->Set_SlideDoor(this);

	return S_OK;
}

_int CSlideDoor::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);
	return NO_EVENT;
}

_int CSlideDoor::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CSlideDoor::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CSlideDoor::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CSlideDoor::Movement(_double dTimeDelta)
{
	if (false == m_bOpen)
		return;

	if (1 == m_tDynamic_Env_Desc.iOption)
	{
		_float	fDis = (_float)dTimeDelta;
		m_fDistance += fDis;

		if (m_fDistance >= 1.f)
		{
			m_bOpen = false;
			m_fDistance = 0.f;
		}
		m_pTransformCom->Go_Straight(-dTimeDelta);
	}
	else
	{
		_float	fDis = (_float)dTimeDelta;
		m_fDistance += fDis;

		if (m_fDistance >= 1.f)
		{
			m_bOpen = false;
			m_fDistance = 0.f;
		}
		m_pTransformCom->Go_Straight(dTimeDelta);
	}

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CSlideDoor::Ready_Component(void * pArg)
{
	/* Static */
	CStaticActor::ARG_DESC tStaticActorArg;
	tStaticActorArg.pTransform = m_pTransformCom;
	tStaticActorArg.pModel = m_pModelCom;
	tStaticActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tStaticActorArg), E_FAIL);

	return S_OK;
}

CSlideDoor * CSlideDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSlideDoor* pInstance = new CSlideDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSlideDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSlideDoor::Clone_GameObject(void * pArg)
{
	CSlideDoor* pInstance = new CSlideDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSlideDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlideDoor::Free()
{
	Safe_Release(m_pStaticActorCom);

	CDynamic_Env::Free();
}
