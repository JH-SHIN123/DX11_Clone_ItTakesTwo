#include "stdafx.h"
#include "..\Public\MainCam.h"
#include "Cam_Helper.h"
#include"Bridge.h"
CMainCam::CMainCam(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CMainCam::CMainCam(const CMainCam & rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCam::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMainCam::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);

	DATABASE->Set_MainCamPtr(this);

	m_eCamMode = CAM_MODE::CAM_MOVETOFREE;
	m_ePreCamFreeOption = CAM_FREEOPTION::OPTION_FOLLOWING_PLAYER;
	m_eCurCamFreeOption = CAM_FREEOPTION::OPTION_FOLLOWING_PLAYER;

	m_fChangeCamModeTime = 0.f;
	m_fChangeCamModeLerpSpeed = 1.f;

	XMStoreFloat4(&m_vDirAccordingToCodySize[CCody::SIZE_SMALL], XMVector3Normalize(XMVectorSet(0.f, 0.5f, -0.5f, 1.f) - XMVectorSet(0.f, 0.05f, 0.01f, 1.f)));
	XMStoreFloat4(&m_vDirAccordingToCodySize[CCody::SIZE_MEDIUM], XMVector3Normalize(XMVectorSet(0.f, 7.f, -7.f, 1.f) - XMVectorSet(0.f, 1.5f, 0.4f, 1.f)));
	XMStoreFloat4(&m_vDirAccordingToCodySize[CCody::SIZE_LARGE], XMVector3Normalize(XMVectorSet(0.f, 11.f, -8.f, 1.f) - XMVectorSet(0.f, 3.f, 1.1f, 1.f)));


	m_vQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_vRotatedDir = m_vBaseDir = m_vDirAccordingToCodySize[m_eCurCodySize];

	return S_OK;
}

_int CMainCam::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCody)
	{
		m_pCody = static_cast<CCody*>(DATABASE->GetCody());
		NULL_CHECK_RETURN(m_pCody, EVENT_ERROR);
		Safe_AddRef(m_pCody);
	}

	Update_ByPlayerState(dTimeDelta);
	m_bLerpDir = DirLerp(dTimeDelta);
	FAILED_CHECK_RETURN(Update_CamHelper(dTimeDelta), EVENT_ERROR);

	return CCamera::Tick(dTimeDelta);
}

_int CMainCam::Late_Tick(_double dTimeDelta)
{
	CCamera::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

void CMainCam::Set_CamMode_MoveToFree()
{
	m_eCamMode = CAM_MODE::CAM_MOVETOFREE;
}

void CMainCam::Update_ByPlayerState(_double dTimeDelta)
{
	if (m_pCody->Get_IsWarpNextStage() == true)
		m_eCamMode = CAM_MODE::CAM_WARP_WORMHOLE;
	else if (DATABASE->Get_BigButtonPressed() && static_cast<CBridge*>(DATABASE->Get_Bridge())->Get_IsUppendede() == false)
		m_eCamMode = CAM_MODE::CAM_PRESSBUTTON_BRIDGE;
	else
	{
		m_eCurCodySize = m_pCody->Get_Player_Size();
		if (m_eCurCodySize != m_ePreCodySize)
		{
			if (false == m_bLerpDir)
			{
				m_ePreCodySize = m_eCurCodySize;
				m_dDirLerpTime = 0.0;
				m_bLerpDir = true;
			}
		}
	}
}

_bool CMainCam::DirLerp(_double dTimeDelta)
{
	if (false == m_bLerpDir)
		return false;

	m_dDirLerpTime += dTimeDelta;
	_vector vPreSizeDir = XMVector3Normalize(XMLoadFloat4(&m_vDirAccordingToCodySize[m_ePreCodySize]));
	_vector vCurSizeDir = XMVector3Normalize(XMLoadFloat4(&m_vDirAccordingToCodySize[m_eCurCodySize]));
	if (m_dDirLerpTime >= 1.0)
	{
		XMStoreFloat4(&m_vBaseDir, vCurSizeDir);
		return false;
	}
	XMStoreFloat4(&m_vBaseDir, XMVectorLerp(vPreSizeDir, vCurSizeDir, (_float)m_dDirLerpTime));
	
	return true;
}

HRESULT CMainCam::Update_CamHelper(_double dTimeDelta)
{
	HRESULT hResult = E_FAIL;

	switch (m_pCamHelper->Tick(dTimeDelta, CFilm::LScreen))
	{
	case CCam_Helper::CamHelperState::Helper_None:
		hResult = Update_CamHelper_None(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_Act:
		hResult = Update_CamHelper_Act(dTimeDelta);
		m_bMakeViewMatrixByCamDesc = false;
		break;
	case CCam_Helper::CamHelperState::Helper_SeeCamNode:
		hResult = Update_CamHelper_SeeCamNode(dTimeDelta);
		m_bMakeViewMatrixByCamDesc = false;
		break;
	}

	return hResult;
}

HRESULT CMainCam::Update_CamHelper_None(_double dTimeDelta)
{

	if (m_eCurCamFreeOption != m_ePreCamFreeOption)
	{
		if (m_eCurCamFreeOption == CAM_FREEOPTION::OPTION_FOLLOWING_PLAYER)
			Set_CamMode_MoveToFree();

		m_ePreCamFreeOption = m_eCurCamFreeOption;
	}

	return Update_Cam_ByMode(dTimeDelta);
}

HRESULT CMainCam::Update_CamHelper_Act(_double dTimeDelta)
{
	_float fFovY = 0.f;

	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Tick_Film(dTimeDelta, CFilm::LScreen, &fFovY));

	if (fFovY == 0.f)
		return S_OK;

	m_CameraDesc.fFovY = XMConvertToRadians(fFovY);

	return S_OK;
}

HRESULT CMainCam::Update_CamHelper_SeeCamNode(_double dTimeDelta)
{
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CamNodeMatrix(m_pTransformCom, dTimeDelta, CFilm::LScreen));

	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode(_double dTimeDelta)
{
	HRESULT hr = S_OK;
	switch (m_eCamMode)
	{
	case Client::CMainCam::CAM_FREE:
		m_bMakeViewMatrixByCamDesc = true;
		hr = Update_Cam_ByMode_Free(dTimeDelta);
		break;
	case Client::CMainCam::CAM_MOVETOFREE:
		m_bMakeViewMatrixByCamDesc = false;
		hr = Update_Cam_ByMode_MoveToFree(dTimeDelta);
		break;
	case Client::CMainCam::CAM_PRESSBUTTON_BRIDGE:
		m_bMakeViewMatrixByCamDesc = false;
		hr = Update_Cam_ByMode_PressBridgeButton(dTimeDelta);
		break;
	case Client::CMainCam::CAM_WARP_WORMHOLE:
		m_bMakeViewMatrixByCamDesc = false;
		hr = Update_Cam_ByMode_WarpWormhole(dTimeDelta);
		break;
	}
	
	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode_Free(_double dTimeDelta)
{
	switch (m_eCurCamFreeOption)
	{
	case Client::CMainCam::OPTION_FOLLOWING_PLAYER:
		return Update_Cam_ByOption_FollowingPlayer(dTimeDelta);
	case Client::CMainCam::OPTION_ON_RAIL:
		return Update_Cam_ByOption_OnRail(dTimeDelta);
	case Client::CMainCam::OPTION_BOSS_MINIROOM:
		break;
	}
	return E_FAIL;
}

HRESULT CMainCam::Update_Cam_ByMode_MoveToFree(_double dTimeDelta)
{
	
	m_eCamMode = CAM_MODE::CAM_FREE;


	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode_PressBridgeButton(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode_WarpWormhole(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByOption_FollowingPlayer(_double dTimeDelta)
{
	_long MouseMove = 0;

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
		m_vQuat.x += (_float)MouseMove * (_float)dTimeDelta * 0.1f;
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
		m_vQuat.y += (_float)MouseMove * (_float)dTimeDelta * 0.1f;


	_vector vPlayerUp	= m_pCody->Get_Transform()->Get_State(CTransform::STATE_UP);
	_vector vPlayerPos	= m_pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	XMStoreFloat4(&m_vRotatedDir, XMVector3TransformNormal(XMLoadFloat4(&m_vBaseDir), XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(m_vQuat.y, m_vQuat.x, 0.f))));

	_vector vDir		= XMLoadFloat4(&m_vRotatedDir);
	_float	fDist		= 0.f;

	XMStoreFloat4(&m_CameraDesc.vAt, vPlayerPos + vPlayerUp * 2.f);
	XMStoreFloat4(&m_CameraDesc.vAxisY, XMVector3Normalize(vPlayerUp));

	_bool isHit = false;

	if (m_pGameInstance->Raycast(MH_PxVec3(m_CameraDesc.vAt), MH_PxVec3(vDir), DISTANCE_PLAYER_TO_CAM, m_RaycastBuffer, PxHitFlag::eDISTANCE))
	{
		for (PxU32 i = 0; i < m_RaycastBuffer.getNbAnyHits(); ++i)
		{
			USERDATA* pUserData = (USERDATA*)m_RaycastBuffer.getAnyHit(i).actor->userData;

			if (nullptr != pUserData)
			{
				if (pUserData->eID == GameID::eCODY || pUserData->eID == GameID::eMAY)
					continue;
				else
				{
					isHit = true;
					fDist = m_RaycastBuffer.getAnyHit(i).distance;
					XMStoreFloat4(&m_CameraDesc.vEye, XMLoadFloat4(&m_CameraDesc.vAt) + vDir * fDist);
					break;
				}
			}
			else
			{
				isHit = true;
				fDist = m_RaycastBuffer.getAnyHit(i).distance;
				XMStoreFloat4(&m_CameraDesc.vEye, XMLoadFloat4(&m_CameraDesc.vAt) + vDir * fDist);
				break;
			}
		}
	}

	if (!isHit)
	{
		XMStoreFloat4(&m_CameraDesc.vEye, XMLoadFloat4(&m_CameraDesc.vAt) + vDir * DISTANCE_PLAYER_TO_CAM);
	}

	_vector	vEye = XMLoadFloat4(&m_CameraDesc.vEye);
	_vector	vAt = XMLoadFloat4(&m_CameraDesc.vAt);
	_vector	vAxisY = XMLoadFloat4(&m_CameraDesc.vAxisY);
	_vector	vLook = XMVector3Normalize(vAt - vEye);
	_vector	vRight = XMVector3Normalize(XMVector3Cross(vAxisY, vLook));
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEye);

	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByOption_OnRail(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByOption_BossMiniRoom(_double dTimeDelta)
{
	return S_OK;
}

CMainCam * CMainCam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMainCam* pInstance = new CMainCam(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMainCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCam::Clone_GameObject(void * pArg)
{
	CMainCam* pInstance = new CMainCam(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMainCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainCam::Free()
{
	if (m_pCody != nullptr) { Safe_Release(m_pCody); }

	Safe_Release(m_pCamHelper);

	CCamera::Free();
}