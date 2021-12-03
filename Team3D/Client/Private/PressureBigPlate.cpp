#include "stdafx.h"
#include "..\public\PressureBigPlate.h"
#include "Cody.h"
#include "May.h"

#include "PressurePlateLock.h"
#include "PressurePlate.h"
#include "PressurePlateFrame.h"
#include "SupportFrame.h"
#include "ControlRoom_Door.h"
#include "BatteryBox.h"

CPressureBigPlate::CPressureBigPlate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPressureBigPlate::CPressureBigPlate(const CPressureBigPlate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPressureBigPlate::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPressureBigPlate::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	/* Option 0 : 파이프 회전 시키는 버튼 / Option 1 : 전력 연결시키는 버튼 */
	if (0 == m_iOption)
	{
		FAILED_CHECK_RETURN(Ready_Layer_Plate(TEXT("Layer_PressurePlate"), 2), E_FAIL);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(63.7634f, 219.8f, 210.848f, 1.f));
	}
	else if (1 == m_iOption)
	{
		FAILED_CHECK_RETURN(Ready_Layer_BatteryBox(TEXT("Layer_BatteryBox")), E_FAIL);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(81.101f, 219.f, 217.141f, 1.f));
		DATABASE->Set_PressureBigPlate(this);
	}

	FAILED_CHECK_RETURN(Ready_Layer_PlateLock(TEXT("Layer_PressurePlateLock")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PlateFrame(TEXT("Layer_PressurePlateFrame")), E_FAIL);

	m_UserData = USERDATA(GameID::ePRESSUREBIGPLATE, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	if (0 == m_iOption)
		DATABASE->Set_PipeCurvePtr(m_vecPressurePlate);
	else if (1 == m_iOption)
		m_vecPressurePlate = DATABASE->Get_PressurePlate();


	SetUp_DefaultPositionSetting();

	return S_OK;
}

_int CPressureBigPlate::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pTriggerCom->Update_TriggerActor();

	switch (m_iOption)
	{
	case 0:
		RotationButton_Active(dTimeDelta);
		break;
	case 1:
		PowerConnectionButton_Active(dTimeDelta);
		break;
	default:
		break;
	}

	return NO_EVENT;
}

_int CPressureBigPlate::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (1 == m_iOption && true == m_vecPressurePlate[0]->Get_PipeConnected() && true == m_vecPressurePlate[1]->Get_PipeConnected())
		m_IsPowerSupplyAvailable = true;
	else
		m_IsPowerSupplyAvailable = false;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPressureBigPlate::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPressureBigPlate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (0 == m_iOption)
	{
		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		{
			m_fMove = 0.f;
			m_IsCollision = true;

			Check_Collision_PlayerAnim();
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
			m_IsCollision = false;

		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		{
			m_fMove = 0.f;
			m_IsCollision = true;

			Check_Collision_PlayerAnim();
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
			m_IsCollision = false;
	}
	else if (1 == m_iOption)
	{
		//if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		//{
		//	m_fMove = 0.f;
		//	m_IsCollision = true;
		//	Check_Collision_PlayerAnim();
		//}
		//else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
		//	m_IsCollision = false;

		if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		{
			m_fMove = 0.f;
			m_IsCollision = true;
			Check_Collision_PlayerAnim();
		}
		else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
			m_IsCollision = false;
	}
}

void CPressureBigPlate::Check_Collision_PlayerAnim()
{
	/* 테스트*/
	_uint i = ((CCody*)DATABASE->GetCody())->Get_Model()->Get_CurAnimIndex();  

	if (m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land
		|| m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Falling
		|| m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Start
		|| m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land_Exit
		|| m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land_MH
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Land
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Falling
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Start
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Land_Exit
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Land_MH)
	{
		m_fMove = 0.f;
		m_fActiveMove = 0.f;
		m_IsButtonActive = true;

		if (0 == m_iOption)
		{
			for (auto pPlate : m_vecPressurePlate)
			{
				if (false == pPlate->Get_ButtonActive())
					pPlate->Set_PipeCurveRotate(true);
			}
		}
	}
}

void CPressureBigPlate::PowerConnectionButton_Active(_double TimeDelta)
{
	if (true == m_IsDoorOpen || false == m_IsPowerSupplyAvailable && true == m_IsPowerSupplyActive)
		return;

	m_pTriggerCom->Update_TriggerActor();
	m_pStaticActorCom->Update_StaticActor();
		
	if (true == m_IsPowerButtonUp)
	{
		if (0.17f > m_fMove)
		{
			m_fMove += (_float)TimeDelta;
			m_pTransformCom->Go_Up(TimeDelta);
			m_pPlateLock->Get_Transform()->Go_Right(TimeDelta);
		}
		else
		{
			m_fMove = 0.f;
			m_IsPowerButtonUp = false;
		}
	}

	if (false == m_IsPowerButtonUp)
	{
		if (false == m_IsButtonActive && true == m_IsCollision)
		{
			if (0.05f > m_fMove)
			{
				m_fMove += (_float)TimeDelta;
				m_pTransformCom->Go_Down(TimeDelta);
				m_pPlateLock->Get_Transform()->Go_Left(TimeDelta);
			}
			else
				m_fMove = 0.05f;
		}
		/* 점프만 해서 올라 타고 버튼 위에서 그냥 내려올 때 */
		else if (false == m_IsButtonActive && false == m_IsCollision)
		{
			if (0.f < m_fMove)
			{
				m_fMove -= (_float)TimeDelta;
				m_pTransformCom->Go_Up(TimeDelta);
				m_pPlateLock->Get_Transform()->Go_Right(TimeDelta);
			}
			else
				m_fMove = 0.f;
		}
	}

	/* 점프 찍기로 버튼을 활성화 했을 때 */
	if (true == m_IsButtonActive)
	{
		if (0.17f > m_fActiveMove && false == m_IsBatteryCheck)
		{
			m_fActiveMove += (_float)TimeDelta;
			m_pTransformCom->Go_Down(TimeDelta);
			m_pPlateLock->Get_Transform()->Go_Left(TimeDelta);
		}
		else
		{
			m_IsBatteryCheck = true;
			m_IsButtonActive = false;
			m_fActiveMove = 0.17f;
		}
	}

	if (true == m_IsBatteryCheck && true == m_pBatteryBox->Get_BatteryHolding())
	{
		m_IsPowerSupplyActive = true;
		m_IsPowerSupplyAvailable = false;
		m_IsDoorOpen = true;
	}
	else if (true == m_IsBatteryCheck &&  false == m_pBatteryBox->Get_BatteryHolding())
	{
		if (0.f < m_fActiveMove)
		{
			m_fActiveMove -= (_float)TimeDelta;
			m_pTransformCom->Go_Up(TimeDelta);
			m_pPlateLock->Get_Transform()->Go_Right(TimeDelta);
		}
		else
		{
			m_IsBatteryCheck = false;
			m_IsButtonActive = false;
			m_fActiveMove = 0.f;
		}
	}
}

void CPressureBigPlate::SetUp_DefaultPositionSetting()
{
	NULL_CHECK(m_pTransformCom);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vObjectPos;
	_float4 vConvertPos;

	if (0 == m_iOption)
	{
		vObjectPos = { 63.7634f, 218.987f, 210.838f, 1.f };
		m_pPlateFrame->Set_Position(vObjectPos);
	}
	else if (1 == m_iOption)
	{
		vObjectPos = { 81.1f, 219.f, 217.15f, 1.f };
		m_pPlateFrame->Set_Position(vObjectPos);
	}

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.y -= 0.1f;
	m_pPlateLock->Set_Position(XMLoadFloat4(&vConvertPos));

}

void CPressureBigPlate::RotationButton_Active(_double TimeDelta)
{
	/* 그냥 점프만 해서 올라 탔을 때*/
	if (false == m_IsButtonActive && true == m_IsCollision)
	{
		if (0.05f > m_fMove)
		{
			m_fMove += (_float)TimeDelta;
			m_pTransformCom->Go_Down(TimeDelta);
			/* 피벗 땜시 ㅜㅜ */
			m_pPlateLock->Get_Transform()->Go_Left(TimeDelta);
			m_pStaticActorCom->Update_StaticActor();
		}
		else
			m_fMove = 0.05f;
	}
	/* 점프만 해서 올라 타고 버튼 위에서 그냥 내려올 때 */
	else if (false == m_IsButtonActive && false == m_IsCollision)
	{
		if (0.f < m_fMove)
		{
			m_fMove -= (_float)TimeDelta;
			m_pTransformCom->Go_Up(TimeDelta);
			m_pPlateLock->Get_Transform()->Go_Right(TimeDelta);
			m_pStaticActorCom->Update_StaticActor();
		}
		else
			m_fMove = 0.f;
	}

	/* 점프 찍기로 버튼을 활성화 했을 때 */
	if (true == m_IsButtonActive && false == m_IsReset)
	{
		if (0.17f > m_fActiveMove)
		{
  			m_fActiveMove += (_float)TimeDelta;
			m_pTransformCom->Go_Down(TimeDelta);
			m_pPlateLock->Get_Transform()->Go_Left(TimeDelta);
			m_pStaticActorCom->Update_StaticActor();
		}
		/* 버튼이 활성화 됬을 때 눌린 상태로 조금 기다렸다가 리셋 하자 */
		else if(0.17f <= m_fActiveMove)
		{
			m_fWaitingTime += (_float)TimeDelta;
			m_fActiveMove = 0.17f;

			if (0.5f <= m_fWaitingTime)
				m_IsReset = true;
		}
	}
	/* 버튼이 아직 활성화 되있고 대기 시간이 끝나서 리셋이 활성화 됬을 때 */
	else if (true == m_IsButtonActive && true == m_IsReset)
	{
		if (0.f < m_fActiveMove)
		{
			m_fActiveMove -= (_float)TimeDelta;
			m_pTransformCom->Go_Up(TimeDelta);
  			m_pPlateLock->Get_Transform()->Go_Right(TimeDelta);
			m_pStaticActorCom->Update_StaticActor();
		}
		else
		{
			m_fActiveMove = 0.f;
			m_fWaitingTime = 0.f;
			m_fMove = 0.f;
			m_IsReset = false;
			m_IsButtonActive = false;
		}
	}
}

HRESULT CPressureBigPlate::Ready_Layer_Plate(const _tchar * pLayerTag, _uint iCount)
{
	CGameObject* pGameObject = nullptr;
	m_vecPressurePlate.reserve(iCount);
	_uint iOption = 0;

	for (_uint i = 0; i < iCount; ++i)
	{
		iOption = i;
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlate"), &iOption, &pGameObject), E_FAIL);
		m_vecPressurePlate.emplace_back(static_cast<CPressurePlate*>(pGameObject));
	}

	return S_OK;
}


HRESULT CPressureBigPlate::Ready_Layer_PlateLock(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 2;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateLock"), &iOption, &pGameObject), E_FAIL);
	m_pPlateLock = static_cast<CPressurePlateLock*>(pGameObject);

	return S_OK;
}

HRESULT CPressureBigPlate::Ready_Layer_PlateFrame(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateFrame"), nullptr, &pGameObject), E_FAIL);
	m_pPlateFrame = static_cast<CPressurePlateFrame*>(pGameObject);

	return S_OK;
}

HRESULT CPressureBigPlate::Ready_Layer_BatteryBox(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_BatteryBox"), nullptr, &pGameObject), E_FAIL);
	m_pBatteryBox = static_cast<CBatteryBox*>(pGameObject);

	return S_OK;
}


HRESULT CPressureBigPlate::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}


CPressureBigPlate * CPressureBigPlate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPressureBigPlate* pInstance = new CPressureBigPlate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPressureBigPlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPressureBigPlate::Clone_GameObject(void * pArg)
{
	CPressureBigPlate* pInstance = new CPressureBigPlate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPressureBigPlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPressureBigPlate::Free()
{
	Safe_Release(m_pPlateLock);
	Safe_Release(m_pPlateFrame);

	if(1 == m_iOption)
		Safe_Release(m_pBatteryBox);

	if (0 == m_iOption)
	{
		for (auto pPressurePlate : m_vecPressurePlate)
			Safe_Release(pPressurePlate);

		m_vecPressurePlate.clear();
	}

	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}