#include "stdafx.h"
#include "..\Public\MainCam.h"
#include "Cam_Helper.h"

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

	XMStoreFloat4(&m_vBaseDir, XMVector3Normalize(XMLoadFloat4(&m_CameraDesc.vEye) - XMLoadFloat4(&m_CameraDesc.vAt)));
	m_vQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_vRotatedDir = m_vBaseDir;

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

	Update_ByPlayerState();

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

void CMainCam::Update_ByPlayerState()
{

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
		Set_CamMode_MoveToFree();
		hResult = Update_CamHelper_Act(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_SeeCamNode:
		Set_CamMode_MoveToFree();
		hResult = Update_CamHelper_SeeCamNode(dTimeDelta);
		break;
	}

	return hResult;
}

HRESULT CMainCam::Update_CamHelper_None(_double dTimeDelta)
{
	// 외부에서 상태 설정 구간
#ifdef __TEST_JUN
	//if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	//{
	//	m_pCamHelper->Start_CamEffect(L"Cam_Shake_Loc_Right", CFilm::RScreen);
	//}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	{
		CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Intro");
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		CCutScenePlayer::GetInstance()->Stop_CutScene();
		return NO_EVENT;
	}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FreeMove;
	}
	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	}
	if (m_pGameInstance->Key_Down(DIK_I))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_OnRail;
	}
#endif
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
	if (m_eCamMode == CAM_MODE::CAM_FREE)
		return Update_Cam_ByMode_Free(dTimeDelta);
	else if (m_eCamMode == CAM_MODE::CAM_MOVETOFREE)
		return Update_Cam_ByMode_MoveToFree(dTimeDelta);
	else if (m_eCamMode == CAM_MODE::CAM_PRESSBUTTON_BRIDGE)
		return Update_Cam_ByMode_PressBridgeButton(dTimeDelta);
	else if (m_eCamMode == CAM_MODE::CAM_WARP_WORMHOLE)
		return Update_Cam_ByMode_WarpWormhole(dTimeDelta);

	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode_Free(_double dTimeDelta)
{
	if (m_eCurCamFreeOption == CAM_FREEOPTION::OPTION_FOLLOWING_PLAYER)
		return Update_Cam_ByOption_FollowingPlayer(dTimeDelta);
	else if (m_eCurCamFreeOption == CAM_FREEOPTION::OPTION_FREE_MOVE)
		return Update_Cam_ByOption_FreeMove(dTimeDelta);
	else if (m_eCurCamFreeOption == CAM_FREEOPTION::OPTION_ON_RAIL)
		return Update_Cam_ByOption_OnRail(dTimeDelta);

	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByMode_MoveToFree(_double dTimeDelta)
{
	//if (m_fChangeCamModeTime >= 1.f)
	//{
	//	//_vector vScale, vRotQuat, vTrans;
	//	//XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, m_pTransformCom->Get_WorldMatrix()); //계산 완료한 이번 프레임의 월드
	//	//XMStoreFloat4(&m_PreWorld.vTrans, vTrans);
	//	//XMStoreFloat4(&m_PreWorld.vRotQuat, vRotQuat);

	//	//m_eCurCamMode = CamMode::Cam_Free;
	//	m_eCamMode = CAM_MODE::CAM_FREE;

	//	return S_OK;
	//}

	//CTransform* pPlayerTransform = m_pCody->Get_Transform();

	//if (m_fChangeCamModeTime == 0.f)
	//	XMStoreFloat4x4(&m_MatrixBuffer_MoveToFree, m_pTransformCom->Get_WorldMatrix());

	//_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
	//_matrix matNext = XMLoadFloat4x4(&m_matStart); //목표 매트릭스

	//_matrix matRev = XMMatrixRotationQuaternion(XMLoadFloat4(&m_PreWorld.vRotQuat)) *
	//	MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos);


	//_matrix matWorld = XMLoadFloat4x4(&m_MatrixBuffer_MoveToFree);
	//m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

	//matNext *= matRev;

	//m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(matWorld, matNext, m_fChangeCamModeTime));


	////////////////////////////
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

	XMStoreFloat4(&m_vRotatedDir, XMVector3TransformNormal(XMLoadFloat4(&m_vBaseDir), XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(m_vQuat.y, m_vQuat.x, 0.f))));


	_vector vPlayerUp	= m_pCody->Get_Transform()->Get_State(CTransform::STATE_UP);
	_vector vPlayerPos	= m_pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
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

	//{
		//m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
	////}
	//if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	//{
	//	_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);

	//	if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
	//		m_fMouseRev[Rev_Prependicul] = 40.f;
	//	else if (m_fMouseRev[Rev_Prependicul] + fVal < -85.f)
	//		m_fMouseRev[Rev_Prependicul] = -85.f;
	//	else
	//		m_fMouseRev[Rev_Prependicul] += fVal;
	//}
	//m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
	//m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;


//////////
//////////	CTransform* pPlayerTransform = m_pCody->Get_Transform();
//////////
//////////	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
//////////
//////////	if (true == m_bIsCollision)
//////////		matWorld = XMLoadFloat4x4(&m_matBeforeSpringCam);
//////////
//////////	//m_pTransformCom->Set_WorldMatrix(matWorld);
//////////	_long MouseMove = 0;
//////////	//이전 회전값
//////////	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
//////////	{
//////////		m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
//////////
//////////	}
//////////	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
//////////	{
//////////		_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);
//////////
//////////		if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
//////////			m_fMouseRev[Rev_Prependicul] = 40.f;
//////////		else if (m_fMouseRev[Rev_Prependicul] + fVal < -85.f)
//////////			m_fMouseRev[Rev_Prependicul] = -85.f;
//////////		else
//////////			m_fMouseRev[Rev_Prependicul] += fVal;
//////////	}
//////////	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
//////////	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;
//////////
//////////
//////////
//////////	//카메라 회전에 따른 거리체크
//////////
//////////	_vector vLook = XMLoadFloat4x4(&m_matBeginWorld).r[2];
//////////	_vector vStartPos = XMLoadFloat4x4(&m_matStart).r[3];
//////////	_vector vDir = XMVectorZero();
//////////
//////////	//카메라 수직이동에따른 거리조절
//////////	switch (m_eCurPlayerSize)
//////////	{
//////////	case Client::CCody::SIZE_SMALL:
//////////		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.01f);
//////////		//vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.02f);
//////////		break;
//////////	case Client::CCody::SIZE_MEDIUM:
//////////		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.065f);
//////////		break;
//////////	case Client::CCody::SIZE_LARGE:
//////////		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.08f);
//////////		break;
//////////	}
//////////
//////////	memcpy(&m_matBeginWorld._41, &(vStartPos - vDir), sizeof(_float4));
//////////
//////////
//////////	//CamEffect
//////////
//////////	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
//////////	{
//////////		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld))) //카메라의 원점 
//////////			XMStoreFloat4x4(&m_matBeginWorld, m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
//////////	}
//////////
//////////	//카메라 움직임이 끝나고 체크할것들
//////////	//For.GravityUp
//////////
//////////
//////////
//////////	//SoftMoving
//////////	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
//////////	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
//////////	vCurPlayerPos += XMVectorSetW(XMLoadFloat4(&m_vSizeAt[m_eCurPlayerSize]), 0.f);
//////////	//카메라와 플레이어의 실제 거리
//////////	_vector vPlayerPos = vCurPlayerPos;
//////////	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));
//////////	_bool bIsTeleport = false;
//////////	if (fDist < 10.f && fDist > 0.01f) //순간이동안했을때
//////////	{
//////////		vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos,
//////////			XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 10.f);
//////////	}
//////////	else if (fDist > 10.f)
//////////		bIsTeleport = true;
//////////
//////////
//////////	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
//////////
//////////	//회전 보간(마우스)
//////////
//////////	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
//////////		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),
//////////		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);
//////////	//Sehoon
//////////
//////////	_matrix matQuat = XMMatrixRotationQuaternion(vCurQuartRot);
//////////
//////////
//////////	_vector vScale, vRotQuat, vTrans;
//////////	_vector  vCurRotQuat, vCurTrans;
//////////	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMLoadFloat4x4(&m_matBeginWorld) * matQuat *
//////////		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos)); //계산 완료한 이번 프레임의 월드
//////////
//////////
//////////
//////////																							   /*vCurRotQuat = vRotQuat;
//////////																							   vCurTrans = vTrans;*/
//////////	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
//////////	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);
//////////
//////////
//////////	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f/*XMVectorGetX(XMVector4Length(vPreQuat - vRotQuat) * dTimeDelta * 20.f)*/);
//////////	vCurTrans = XMVectorLerp(vPreTrans, vTrans, 0.5f /*XMVectorGetX(XMVector4Length(vPreTrans - vTrans) * dTimeDelta * 20.f)*/);
//////////
//////////
//////////	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
//////////	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);
//////////
//////////	_matrix matAffine = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
//////////		vCurRotQuat, vCurTrans);
//////////
//////////	XMStoreFloat4x4(&m_matBeforeSpringCam, matAffine);
//////////#pragma region PhsyX Check
//////////	_vector vResultPos = XMVectorZero();
//////////	if (false == bIsTeleport)
//////////	{
//////////		// m_bIsCollision = OffSetPhsX(matAffine, dTimeDelta, &vResultPos); //SpringCamera
//////////		//
//////////		//_float4 vEye, vAt;
//////////		//
//////////		//XMStoreFloat4(&vEye, vResultPos);
//////////		//XMStoreFloat4(&vAt, vPlayerPos);
//////////		//_matrix matCurWorld = MakeViewMatrixByUp(vEye, vAt);
//////////		//matAffine = matCurWorld;
//////////
//////////	}
//////////	else
//////////		m_bIsCollision = false;
//////////	m_pTransformCom->Set_WorldMatrix(matAffine);
//////////
//////////#pragma endregion

	return S_OK;
}

HRESULT CMainCam::Update_Cam_ByOption_FreeMove(_double dTimeDelta)
{
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