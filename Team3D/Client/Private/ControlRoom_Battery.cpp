#include "stdafx.h"
#include "..\public\ControlRoom_Battery.h"
#include "Cody.h"
#include "May.h"
#include "UI_Generator.h"
#include "ControlRoom_Door.h"
#include "PressureBigPlate.h"

CControlRoom_Battery::CControlRoom_Battery(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CControlRoom_Battery::CControlRoom_Battery(const CControlRoom_Battery & rhs)
	: CGameObject(rhs)
{
}

HRESULT CControlRoom_Battery::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CControlRoom_Battery::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_TriggerTransform"), (CComponent**)&m_pTriggerTransform, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_ControlRoom_Battery"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Door(TEXT("Layer_ControlRoom_Door"), 2), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.659f, 221.12184f, 224.44f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(25.f));
	m_fAngle = 25.f;

	m_UserData = USERDATA(GameID::eCONTROLROOMBATTERY, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;
	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.y += 1.f;
	m_pTriggerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vConvertPos));

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTriggerTransform;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	m_fRotate = 25.f;

	return S_OK;
}

_int CControlRoom_Battery::Tick(_double dTimeDelta)
{
 	CGameObject::Tick(dTimeDelta);
	
	m_pTriggerCom->Update_TriggerActor();

	InterActive_Battery(dTimeDelta);

	UI_Generator->CreateInterActiveUI_AccordingRange(Player::Cody, UI::ControlRoom_Battery, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f, m_IsCollision, false);

	return NO_EVENT;
}

_int CControlRoom_Battery::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CControlRoom_Battery::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CControlRoom_Battery::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eCONTROLROOMBATTERY, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollision = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		m_IsCollision = false;
}


_int CControlRoom_Battery::InterActive_Battery(_double TimeDelta)
{

	if (true == m_IsCollision && m_pGameInstance->Key_Down(DIK_F) && false == m_IsPlayerInterActive)
		m_IsPlayerInterActive = true;

	if (true == m_IsPlayerInterActive)
	{
		m_fAngle -= (_float)TimeDelta * 15.f;
		m_fRotate += (_float)TimeDelta * 15.f;

		if (0.f >= m_fAngle)
		{
			m_IsPlayerInterActive = false;
			m_IsBatteryHolding = true;
		}

		if (1.f <= m_fRotate)
		{
			m_fRotate = 0.f;
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4 vConvertPos;
			XMStoreFloat4(&vConvertPos, vPos);
			vConvertPos.z += 0.01f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vConvertPos));
		}

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fAngle));
	}
	else if (true == m_IsBatteryHolding)
	{
		CCody* pCody = (CCody*)DATABASE->GetCody();
		NULL_CHECK_RETURN(pCody, EVENT_ERROR);

		if (false == pCody->Get_PushingBattery())
		{
			m_fAngle += (_float)TimeDelta * 15.f;
			m_fRotate += (_float)TimeDelta * 15.f;

			if (25.f <= m_fAngle)
			{
				m_IsBatteryHolding = false;
				m_IsPlayerInterActive = false;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.659f, 221.12184f, 224.44f, 1.f));
			}

			if (1.f <= m_fRotate)
			{
				m_fRotate = 0.f;
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4 vConvertPos;
				XMStoreFloat4(&vConvertPos, vPos);
				vConvertPos.z -= 0.01f;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vConvertPos));
			}

			m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fAngle));
		}
		else
		{
			CPressureBigPlate* pBigPlate = (CPressureBigPlate*)DATABASE->Get_PressureBigPlate();
			NULL_CHECK_RETURN(pBigPlate, EVENT_ERROR);

			if (true == pBigPlate->Get_PowerSupplyActive())
			{
				for (auto pDoor : m_vecDoor)
					pDoor->Set_OpenDoor();

				m_IsBatteryHolding = false;
			}
		}
	}

	return NO_EVENT;
}

HRESULT CControlRoom_Battery::Ready_Layer_Door(const _tchar * pLayerTag, _uint iCount)
{
	CGameObject* pGameObject = nullptr;
	m_vecDoor.reserve(iCount);
	_uint iOption = 0;

	for (_uint i = 0; i < iCount; ++i)
	{
		iOption = i;
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_ControlRoom_Door"), &iOption, &pGameObject), E_FAIL);
		m_vecDoor.emplace_back(static_cast<CControlRoom_Door*>(pGameObject));
	}

	return S_OK;
}



HRESULT CControlRoom_Battery::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CControlRoom_Battery * CControlRoom_Battery::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CControlRoom_Battery* pInstance = new CControlRoom_Battery(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CControlRoom_Battery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CControlRoom_Battery::Clone_GameObject(void * pArg)
{
	CControlRoom_Battery* pInstance = new CControlRoom_Battery(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CControlRoom_Battery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CControlRoom_Battery::Free()
{
	for (auto pDoor : m_vecDoor)
		Safe_Release(pDoor);

	m_vecDoor.clear();

	Safe_Release(m_pTriggerTransform);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}