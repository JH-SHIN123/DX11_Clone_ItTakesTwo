#include "stdafx.h"
#include "..\public\ControlRoom_Door.h"
#include "Cody.h"
#include "May.h"
#include "PressurePlateLock.h"
#include"CutScenePlayer.h"

CControlRoom_Door::CControlRoom_Door(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CControlRoom_Door::CControlRoom_Door(const CControlRoom_Door & rhs)
	: CGameObject(rhs)
{
}

HRESULT CControlRoom_Door::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControlRoom_Door::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Door"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (0 == m_iOption)
	{
		//m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(66.01f, 218.552f, 232.591f, 1.f));

	}
	else if (1 == m_iOption)
	{
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(62.3026f, 218.552f, 232.591f, 1.f));
	}
	m_UserData = USERDATA(GameID::eCONTROLROOMDOOR, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CControlRoom_Door::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == m_IsOpenDoor)
	{
		if (1.2f >= m_fMove)
		{
			m_fMove += (_float)dTimeDelta;
			m_pTransformCom->Go_Right(dTimeDelta * 3.f);
		}
		else
		{
#ifdef __PLAY_CUTSCENE
			if (false == CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Boss_Intro))
			{
				CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Boss_Intro"));
				CCutScenePlayer::GetInstance()->Set_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Boss_Intro, true);
			}
#endif	
		}
	}

	return NO_EVENT;
}

_int CControlRoom_Door::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CControlRoom_Door::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CControlRoom_Door::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

void CControlRoom_Door::Set_OpenDoor()
{
	m_IsOpenDoor = true;

}


HRESULT CControlRoom_Door::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CControlRoom_Door * CControlRoom_Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControlRoom_Door* pInstance = new CControlRoom_Door(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControlRoom_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CControlRoom_Door::Clone_GameObject(void * pArg)
{
	CControlRoom_Door* pInstance = new CControlRoom_Door(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControlRoom_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlRoom_Door::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}