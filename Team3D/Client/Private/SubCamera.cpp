#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "May.h"

CSubCamera::CSubCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CSubCamera::CSubCamera(const CSubCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CSubCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSubCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);


	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	m_eCurCamMode = Cam_Free;
	CDataBase::GetInstance()->Set_SubCamPtr(this);

	return S_OK;
}

_int CSubCamera::Tick(_double dTimeDelta)
{
	//if (m_pGameInstance->Key_Pressing(DIK_W))
	//	m_pTransformCom->Go_Straight(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_A))
	//	m_pTransformCom->Go_Left(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_S))
	//	m_pTransformCom->Go_Backward(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_D))
	//	m_pTransformCom->Go_Right(dTimeDelta);
	if (m_pTargetObj == nullptr)
	{
		m_pTargetObj = CDataBase::GetInstance()->GetMay();
		if (m_pTargetObj)
			Safe_AddRef(m_pTargetObj);
	}


	if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
		m_eCurCamMode = Cam_FreeToAuto;
	if (m_pGameInstance->Key_Down(DIK_NUMPADENTER))
		m_eCurCamMode = Cam_AutoToFree;


	if (m_eCurCamMode != m_ePreCamMode)
	{
		m_ePreCamMode = m_eCurCamMode;
		m_fChangeCamModeTime = 0.f;
	}


	_int iResult = NO_EVENT;

	switch (m_eCurCamMode)
	{
	case Client::CSubCamera::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CSubCamera::Cam_Auto:
		iResult = Tick_Cam_Auto(dTimeDelta);
		break;
	case Client::CSubCamera::Cam_FreeToAuto:
		iResult = Tick_Cam_FreeToAuto(dTimeDelta);
		break;
	case Client::CSubCamera::Cam_AutoToFree:
		iResult = Tick_Cam_AutoToFree(dTimeDelta);
		break;
	}
	if (NO_EVENT != iResult)
		return iResult;
	return CCamera::Tick(dTimeDelta);
}

_int CSubCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);



	return NO_EVENT;
}

CSubCamera * CSubCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSubCamera* pInstance = new CSubCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSubCamera");
		Safe_Release(pInstance);
	}
	//Test
	return pInstance;
}

CGameObject * CSubCamera::Clone_GameObject(void * pArg)
{
	CSubCamera* pInstance = new CSubCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSubCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubCamera::Free()
{
	Safe_Release(m_pTargetObj);
	CCamera::Free();
}

_int CSubCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;
	CTransform* pPlayerTransform = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform();


	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_long MouseMove = 5;

	if (/*MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X)*/m_pGameInstance->Key_Pressing(DIK_O))
	{
		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta * m_fMouseRevSpeed[Rev_Holizontal];
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
			m_fMouseRev[Rev_Holizontal] = 0.f;
	}
	if (/*MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X)*/m_pGameInstance->Key_Pressing(DIK_P))
	{
		m_fMouseRev[Rev_Holizontal] -= (_float)MouseMove * (_float)dTimeDelta * m_fMouseRevSpeed[Rev_Holizontal];
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
			m_fMouseRev[Rev_Holizontal] = 0.f;
	}
	if (/*MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y)*/m_pGameInstance->Key_Pressing(DIK_U))
	{
		m_fMouseRev[Rev_Prependicul] += (_float)MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * (_float)dTimeDelta;
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		if (m_fMouseRev[Rev_Prependicul] > 30.f)
			m_fMouseRev[Rev_Prependicul] = 30.f;
		if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
	if (/*MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y)*/m_pGameInstance->Key_Pressing(DIK_I))
	{
		m_fMouseRev[Rev_Prependicul] -= (_float)MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * (_float)dTimeDelta;
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		if (m_fMouseRev[Rev_Prependicul] > 30.f)
			m_fMouseRev[Rev_Prependicul] = 30.f;
		if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
#ifdef _DEBUG
	if (m_pGameInstance->Key_Pressing(DIK_Z))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_X))
		m_pTransformCom->Go_Backward(dTimeDelta);
#endif
	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;
	XMStoreFloat4x4(&m_matPreRev, matRev);

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Auto(_double dTimeDelta)
{

	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD8))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD4))
		m_pTransformCom->Go_Left(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD2))
		m_pTransformCom->Go_Backward(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD6))
		m_pTransformCom->Go_Right(dTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD7))
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD9))
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -dTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD1))
		m_pTransformCom->Rotate_Axis(XMVectorSet(1.f, 0.f, 0.f, 0.f), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD3))
		m_pTransformCom->Rotate_Axis(XMVectorSet(1.f, 0.f, 0.f, 0.f), -dTimeDelta);

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{

	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;

	_vector vPlayerPos = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_fChangeCamModeTime += (_float)dTimeDelta;
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld);
	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;
	XMStoreFloat4x4(&m_matPreRev, matRev);

	matNext *= matRev;

	_vector	  vPreRight = matWorld.r[0], vNextRight = matNext.r[0]
		, vPreUp = matWorld.r[1], vNextUp = matNext.r[1]
		, vPreLook = matWorld.r[2], vNextLook = matNext.r[2]
		, vPrePos = matWorld.r[3], vNextPos = matNext.r[3];

	_vector vCurRight = XMVectorLerp(vPreRight, vNextRight, m_fChangeCamModeTime),
		vCurUp = XMVectorLerp(vPreUp, vNextUp, m_fChangeCamModeTime),
		vCurLook = XMVectorLerp(vPreLook, vNextLook, m_fChangeCamModeTime),
		vCurPos = XMVectorLerp(vPrePos, vNextPos, m_fChangeCamModeTime);

	_matrix matCurWorld = XMMatrixIdentity();
	matCurWorld.r[0] = vCurRight;
	matCurWorld.r[1] = vCurUp;
	matCurWorld.r[2] = vCurLook;
	matCurWorld.r[3] = vCurPos;


	m_pTransformCom->Set_WorldMatrix(matCurWorld);
	if (m_fChangeCamModeTime >= 1.f)
	{

		m_eCurCamMode = Cam_Free;

	}

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_FreeToAuto(_double dTimeDelta)
{

	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());

	m_fMouseRev[Rev_Holizontal] = 0.f;
	m_fMouseRev[Rev_Prependicul] = 0.f;

	m_eCurCamMode = Cam_Auto;
	return NO_EVENT;
}

