#include "stdafx.h"
#include "..\public\RotatedNoBatterySign.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "RotatedRobotHead.h"

CRotatedNoBatterySign::CRotatedNoBatterySign(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRotatedRobotParts(pDevice, pDeviceContext)
{
}

CRotatedNoBatterySign::CRotatedNoBatterySign(const CRotatedRobotParts & rhs)
	: CRotatedRobotParts(rhs)
{
}

HRESULT CRotatedNoBatterySign::NativeConstruct_Prototype()
{
	CRotatedRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotatedNoBatterySign::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedNoBatterySign"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_tRtRotatedRobotPartsDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));

	_vector vPosition = m_tRtRotatedRobotPartsDesc.vPosition;
	vPosition.m128_f32[0] += 3.312f;
	vPosition.m128_f32[1] -= 1.3f;
	vPosition.m128_f32[2] += 1.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	

	CStaticActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eNOBATTERYSIGN, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CRotatedNoBatterySign::Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);

	if (m_bBatteryCharged == false && m_bHitLever == true)
	{
		m_fRotateTime += (_float)dTimeDelta;

		if (m_fRotateTime < 0.2f)
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 0.f, 1.f, 0.f), dTimeDelta * 7.f);
			if (m_bSoundOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_ROBOT_NOBATTERY, m_fNoBatterySignVolume);
				m_pGameInstance->Play_Sound(TEXT("Robot_NoBattery.wav"), CHANNEL_ROBOT_NOBATTERY, m_fNoBatterySignVolume);
				m_bSoundOnce = true;
			}
		}
		else if (m_fRotateTime > 0.4f && m_fRotateTime < 0.6f)
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 0.f, 1.f, 0.f), -dTimeDelta * 7.f);
		else if (m_fRotateTime >= 0.6f)
		{
			m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));
			m_fRotateTime = 0.f;
			m_bHitLever = false;
			m_bSoundOnce = false;
		}
	}

	return NO_EVENT;
}

_int CRotatedNoBatterySign::Late_Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotatedNoBatterySign::Render(RENDER_GROUP::Enum eGroup)
{
	CRotatedRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CRotatedNoBatterySign::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CRotatedNoBatterySign::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CRotatedNoBatterySign * CRotatedNoBatterySign::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedNoBatterySign* pInstance = new CRotatedNoBatterySign(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRotatedNoBatterySign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedNoBatterySign::Clone_GameObject(void * pArg)
{
	CRotatedNoBatterySign* pInstance = new CRotatedNoBatterySign(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedNoBatterySign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedNoBatterySign::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}