#include "stdafx.h"
#include "..\public\PressureBigPlate.h"
#include "Cody.h"
#include "May.h"

#include "PressurePlateLock.h"
#include "PressurePlate.h"
#include "PressurePlateFrame.h"
#include "SupportFrame.h"

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
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Plate(TEXT("Layer_PressurePlate"), 2), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PlateLock(TEXT("Layer_PressurePlateLock")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PlateFrame(TEXT("Layer_PressurePlateFrame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SupportFrame(TEXT("Layer_SupportFrame")), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 1.f, 0.f, 1.f));
	//m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.0f);
	m_UserData = USERDATA(GameID::ePRESSUREBIGPLATE, this);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);
	//DATABASE->Set_PressureBigPlatePtr(this);

	SetUp_DefaultPositionSetting();

	return S_OK;
}

_int CPressureBigPlate::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	Button_Active(dTimeDelta);

	return NO_EVENT;
}

_int CPressureBigPlate::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

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
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		m_IsCollision = true;

		Check_Collision_PlayerAnim();
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsCollision = false;
		m_IsButtonActive = false;
	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		m_IsCollision = true;

		Check_Collision_PlayerAnim();
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsCollision = false;
		m_IsButtonActive = false;
	}
}

void CPressureBigPlate::Check_Collision_PlayerAnim()
{
	if (m_IsCollision == true && (((CCody*)DATABASE->GetCody())->Get_Model())->Get_CurAnimIndex() == ANI_C_Bhv_GroundPound_Land
		|| m_IsCollision == true && (((CMay*)DATABASE->GetMay())->Get_Model())->Get_CurAnimIndex() == ANI_M_GroundPound_Land)
	{
		m_IsButtonActive = true;

		for (auto pPlate : m_vecPressurePlate)
		{
			if(false == pPlate->Get_ButtonActive())
				pPlate->Set_PipeCurveRotate(true);
		}
	}
}


void CPressureBigPlate::SetUp_DefaultPositionSetting()
{
	NULL_CHECK(m_pTransformCom);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;
	XMStoreFloat4(&vConvertPos, vPos);

	vConvertPos.y -= 0.3f;
	m_pPlateLock->Set_Position(XMLoadFloat4(&vConvertPos));

	vConvertPos.y = 0.4f;
	m_pPlateFrame->Set_Position(XMLoadFloat4(&vConvertPos));

	vConvertPos.y = 0.f;
	m_pSupportFrame->Set_Position(XMLoadFloat4(&vConvertPos));
}

void CPressureBigPlate::Button_Active(_double TimeDelta)
{
	if (false == m_IsButtonActive)
	{
		if (true == m_IsCollision)
		{
			if (0.3f > m_fMove)
			{
				m_fMove += (_float)TimeDelta;
				m_pTransformCom->Go_Down(TimeDelta * 0.2f);
				m_pPlateLock->Get_Transform()->Go_Down(TimeDelta * 0.2f);
			}
			else
				m_fMove = 0.3f;
		}
		else if (false == m_IsCollision)
		{
			if (0.f < m_fMove)
			{
				m_fMove -= (_float)TimeDelta;
				m_pTransformCom->Go_Up(TimeDelta * 0.2f);
				m_pPlateLock->Get_Transform()->Go_Up(TimeDelta * 0.2f);
			}
			else
				m_fMove = 0.f;
		}
	}
	else
	{
		if (true == m_IsCollision)
		{
			if (0.8f > m_fMove)
			{
				m_fMove += (_float)TimeDelta;
				m_pTransformCom->Go_Down(TimeDelta * 0.2f);
				m_pPlateLock->Get_Transform()->Go_Down(TimeDelta * 0.2f);
			}
			else
				m_fMove = 0.8f;
		}
		else if (false == m_IsCollision)
		{
			if (0.f < m_fMove)
			{
				m_fMove -= (_float)TimeDelta;
				m_pTransformCom->Go_Up(TimeDelta * 0.2f);
				m_pPlateLock->Get_Transform()->Go_Up(TimeDelta * 0.2f);
			}
			else
				m_fMove = 0.f;
		}
	}

	m_pStaticActorCom->Update_StaticActor();
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

HRESULT CPressureBigPlate::Ready_Layer_SupportFrame(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 0;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SupportFrame"), &iOption, &pGameObject), E_FAIL);
	m_pSupportFrame = static_cast<CSupportFrame*>(pGameObject);
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
	Safe_Release(m_pSupportFrame);

	for (auto pPressurePlate : m_vecPressurePlate)
		Safe_Release(pPressurePlate);

	m_vecPressurePlate.clear();

	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}