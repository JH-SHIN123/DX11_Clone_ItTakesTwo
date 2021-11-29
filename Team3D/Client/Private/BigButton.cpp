#include "stdafx.h"
#include "..\public\BigButton.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CBigButton::CBigButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBigButton::CBigButton(const CBigButton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBigButton::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBigButton::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_BigButton"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	ROBOTDESC BigButtonDesc;
	if (nullptr != pArg)
		memcpy(&BigButtonDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, BigButtonDesc.vPosition);

	CStaticActor::ARG_DESC StaticDesc;
	m_UserData = USERDATA(GameID::eBIGBUTTON, this);
	StaticDesc.pModel = m_pModelCom;
	StaticDesc.pTransform = m_pTransformCom;
	StaticDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &StaticDesc), E_FAIL);

	CTriggerActor::ARG_DESC ArgDesc;
	m_UserData = USERDATA(GameID::eBIGBUTTON, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = new PxSphereGeometry(5.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);
	return S_OK;
}

_int CBigButton::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	if (m_bUpdate == true)
	{
		Check_Collision_PlayerAnim();

		if (m_bPressed == true)
		{
			if (m_fMoveDist < 0.6f)
			{
				m_fMoveDist += (_float)dTimeDelta;
				m_pTransformCom->Go_Down(dTimeDelta * 0.4f);
			}
			if (m_fMoveDist >= 0.5f)
			{
				m_bUpdate = false;
			}
		}
	}
	return NO_EVENT;
}


_int CBigButton::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBigButton::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CBigButton::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (m_bUpdate == true)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBIGBUTTON, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCollide = true;
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		{
			m_IsCollide = false;
		}
	}
}

HRESULT CBigButton::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CBigButton::Check_Collision_PlayerAnim()
{
	if (m_IsCollide == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land
		&& (((CCody*)DATABASE->GetCody())->Get_Player_Size() == CCody::PLAYER_SIZE::SIZE_LARGE))
	{
		m_bPressed = true;
	}
	

}

CBigButton * CBigButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBigButton* pInstance = new CBigButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBigButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBigButton::Clone_GameObject(void * pArg)
{
	CBigButton* pInstance = new CBigButton(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBigButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBigButton::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}