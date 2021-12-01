#include "stdafx.h"
#include "..\public\PressurePlate.h"
#include "Cody.h"
#include "May.h"
#include "PressurePlateFrame.h"
#include "SupportFrame.h"
#include "PipeCurve.h"
#include "PressurePlateLock.h"

CPressurePlate::CPressurePlate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPressurePlate::CPressurePlate(const CPressurePlate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPressurePlate::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPressurePlate::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_PressurePlate"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (0 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(55.1267f, 218.752f, 217.445f, 1.f));
	}
	else if (1 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(72.2947f, 218.752f, 227.324f, 1.f));
	}

	FAILED_CHECK_RETURN(Ready_Layer_PipeCurve(TEXT("Layer_PipeCurve")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PlateFrame(TEXT("Layer_PressurePlateFrame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_SupportFrame(TEXT("Layer_SupportFrame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_PlateLock(TEXT("Layer_PressurePlateLock"), 4), E_FAIL);

	m_UserData = USERDATA(GameID::ePRESSUREPLATE, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.7f);
	TriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);
	//DATABASE->Set_PressurePlatePtr(this);

	SetUp_DefaultPositionSetting();

	return S_OK;
}

_int CPressurePlate::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	Button_Active(dTimeDelta);


	return NO_EVENT;
}

_int CPressurePlate::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CPressurePlate::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CPressurePlate::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		m_IsButtonActive = true;

		for (auto pPressurePlateLock : m_vecPressurePlateLock)
			pPressurePlateLock->Set_LockActive(true);

		m_pSupportFrame->Set_LockActive(true);
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		m_IsButtonActive = false;

		for (auto pPressurePlateLock : m_vecPressurePlateLock)
			pPressurePlateLock->Set_LockActive(false);

		m_pSupportFrame->Set_LockActive(false);
	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		m_IsButtonActive = true;

		for (auto pPressurePlateLock : m_vecPressurePlateLock)
			pPressurePlateLock->Set_LockActive(true);

		m_pSupportFrame->Set_LockActive(true);
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsButtonActive = false;

		for (auto pPressurePlateLock : m_vecPressurePlateLock)
			pPressurePlateLock->Set_LockActive(false);

		m_pSupportFrame->Set_LockActive(false);
	}

}

_bool CPressurePlate::Get_PipeConnected()
{
	return m_pPipeCurve->Get_Connected();
}

void CPressurePlate::Set_Position(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CPressurePlate::Set_PipeCurveRotate(_bool IsCheck)
{
	NULL_CHECK(m_pPipeCurve);

	m_pPipeCurve->Set_Rotate(IsCheck);
}

void CPressurePlate::SetUp_DefaultPositionSetting()
{
	if (nullptr == m_pPressurePlateFrame || nullptr == m_pPipeCurve ||
		nullptr == m_pSupportFrame || true == m_vecPressurePlateLock.empty())
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;


	if (0 == m_iOption)
	{
		_vector vObjectPos = { 55.1267f, 215.752f, 217.445f, 1.f };
		m_pSupportFrame->Set_Position(vObjectPos);

		vObjectPos = { 55.8f, 216.152f, 217.446f, 1.f };
		m_pPipeCurve->Set_Position(vObjectPos);
	}
	else if (1 == m_iOption)
	{
		_vector vObjectPos = { 72.2947f, 215.552f, 227.324f, 1.f };
		m_pSupportFrame->Set_Position(vObjectPos);

		vObjectPos = { 72.f, 216.152f, 227.324f, 1.f };
		m_pPipeCurve->Set_Position(vObjectPos);
	}

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.y -= 0.1f;
	m_pPressurePlateFrame->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.z += 1.f;
	m_vecPressurePlateLock[0]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x -= 1.f;
	vConvertPos.z += 1.f;
	m_vecPressurePlateLock[1]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x -= 1.f;
	vConvertPos.z -= 1.f;
	m_vecPressurePlateLock[2]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.z -= 1.f;
	m_vecPressurePlateLock[3]->Set_Position(XMLoadFloat4(&vConvertPos));
}

void CPressurePlate::Button_Active(_double TimeDelta)
{
	if (true == m_IsButtonActive)
	{
		if (0.3f > m_fMove)
		{
			m_fMove += (_float)TimeDelta;
			m_pTransformCom->Go_Down(TimeDelta * 0.2f);
		}
		else
			m_fMove = 0.3f;
	}
	else if (false == m_IsButtonActive)
	{
		if (0.f < m_fMove)
		{
			m_fMove -= (_float)TimeDelta;
			m_pTransformCom->Go_Up(TimeDelta * 0.2f);
		}
		else
			m_fMove = 0.f;
	}

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CPressurePlate::Ready_Layer_PlateFrame(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateFrame"), nullptr, &pGameObject), E_FAIL);
	m_pPressurePlateFrame = static_cast<CPressurePlateFrame*>(pGameObject);
	return S_OK;
}

HRESULT CPressurePlate::Ready_Layer_PipeCurve(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PipeCurve"), &m_iOption, &pGameObject), E_FAIL);
	m_pPipeCurve = static_cast<CPipeCurve*>(pGameObject);

	return S_OK;
}

HRESULT CPressurePlate::Ready_Layer_SupportFrame(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	_uint iOption = 1;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_SupportFrame"), &iOption, &pGameObject), E_FAIL);
	m_pSupportFrame = static_cast<CSupportFrame*>(pGameObject);
	return S_OK;
}

HRESULT CPressurePlate::Ready_Layer_PlateLock(const _tchar * pLayerTag, _uint iCount)
{
	CGameObject* pGameObject = nullptr;
	m_vecPressurePlateLock.reserve(iCount);
	_uint iOption = 0;

	for (_uint i = 0; i < iCount; ++i)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateLock"), &iOption, &pGameObject), E_FAIL);
		m_vecPressurePlateLock.emplace_back(static_cast<CPressurePlateLock*>(pGameObject));
	}

	return S_OK;
}


HRESULT CPressurePlate::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CPressurePlate * CPressurePlate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPressurePlate* pInstance = new CPressurePlate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPressurePlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPressurePlate::Clone_GameObject(void * pArg)
{
	CPressurePlate* pInstance = new CPressurePlate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPressurePlate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPressurePlate::Free()
{
	Safe_Release(m_pPipeCurve);
	Safe_Release(m_pSupportFrame);
	Safe_Release(m_pPressurePlateFrame);

	for (auto pPressurePlateLock : m_vecPressurePlateLock)
		Safe_Release(pPressurePlateLock);

	m_vecPressurePlateLock.clear();

	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}