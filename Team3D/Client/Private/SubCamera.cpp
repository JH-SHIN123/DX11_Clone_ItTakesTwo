#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"
#include "PlayerActor.h"
#include "May.h"
#include "CameraActor.h"
#include"CutScenePlayer.h"
#include"Moon.h"
#include"UI_Generator.h"
#include"PinBall.h"
#include"PinBall_Handle.h"
#include"Cody.h"
#include"LaserTennis_Manager.h"
#include"AlphaScreen.h"
#include"MoonUFO.h"
#include "MainCamera.h"
#include"UFO.h"
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


	CCameraActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eSUBCAMERA, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;

	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);

	CDataStorage::GetInstance()->Set_SubCamPtr(this);

	
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	
	m_PreWorld.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	m_PreWorld.vRotQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_PreWorld.vTrans = _float4(0.f, 0.f, 0.f, 1.f);

	m_vStartEye =	{0.f,7.f, -7.f, 1.f};
	m_vStartAt =	{0.f,1.5f, 0.f, 1.f };



	_matrix matStart = MakeViewMatrixByUp(m_vStartEye, m_vStartAt,XMVectorSet(0.f,1.f,0.f,0.f));
	XMStoreFloat4x4(&m_matBeginWorld, matStart);
	return S_OK;
}

_int CSubCamera::Tick(_double dTimeDelta)
{
	CCutScenePlayer::GetInstance()->Tick_CutScene(dTimeDelta);

	if (false == m_bStart)
	{
		m_pMay = static_cast<CMay*>(CDataStorage::GetInstance()->GetMay());
		if (m_pMay)
		{
			CTransform* pPlayerTransform = m_pMay->Get_Transform();
			XMStoreFloat4(&m_vPlayerPos, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vPlayerUp, pPlayerTransform->Get_State(CTransform::STATE_UP));
			Safe_AddRef(m_pMay);
		}
		else
			return EVENT_ERROR;
		m_bStart = true;
	}


	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;

	_int iResult = NO_EVENT;
	

	if (iResult = Check_Player(dTimeDelta))
		return iResult;

	switch (m_pCamHelper->Tick(dTimeDelta, CFilm::RScreen))
	{
	case CCam_Helper::CamHelperState::Helper_None:
		iResult = Tick_CamHelperNone(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_Act:
		iResult = Tick_CamHelper_Act(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_SeeCamNode:
		iResult = Tick_CamHelper_SeeCamNode(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_End:
	default:
		iResult = EVENT_ERROR;
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



_int CSubCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;
	
	if (m_pMay->Get_IsInUFO())
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_RidingSpaceShip_May;
	if (m_pMay->Get_IsWarpNextStage() == true)
		m_eCurCamMode = CamMode::Cam_Warp_WormHole;
	if (m_pMay->Get_IsPinBall())
		m_eCurCamMode = CamMode::Cam_PinBall_May;
	if (m_pMay->Get_IsWallJump())
		m_eCurCamMode = CamMode::Cam_WallJump;
	if (CLaserTennis_Manager::GetInstance()->Get_StartGame() && m_eCurCamMode != CamMode::Cam_LaserTennis)
		m_eCurCamMode = CamMode::Cam_LaserTennis;

	if (m_bOpenThirdFloor && m_fOpenThirdFloorTime == 0.f)
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_OpenThirdFloor;
	if (m_eCurCamMode == CamMode::Cam_Free)
	{
		switch (m_eCurCamFreeOption)
		{
		case Client::CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
			m_pMay->Get_Actor()->Get_IsOnGravityPath() ? Set_Zoom(m_vStartEye,m_vStartAt,2.f, dTimeDelta) : Set_Zoom(m_vStartEye, m_vStartAt, 0.f, dTimeDelta);
			break;
		case Client::CSubCamera::CamFreeOption::Cam_Free_FreeMove:
			break;
		case Client::CSubCamera::CamFreeOption::Cam_Free_RidingSpaceShip_May:
			//Set_Zoom(m_vStartRidingUFOEye, m_vStartRidingUFOAt, -8.f, dTimeDelta);
			break;
		}
	
	}
#ifdef __TEST_JUN
	if (m_pGameInstance->Key_Down(DIK_U))
		ReSet_Cam_FreeToAuto();
	if (m_pGameInstance->Key_Down(DIK_9))
		ReSet_Cam_FreeToAuto();
#endif
	return NO_EVENT;
}

void CSubCamera::Set_Zoom(_float4 vEye, _float4 vAt, _float fZoomVal, _double dTimeDelta)
{
	m_fCamZoomVal += (fZoomVal - m_fCamZoomVal) * (_float)dTimeDelta * 5.f;

	_matrix matStart = MakeViewMatrixByUp(vEye, vAt,XMVectorSet(0.f,1.f,0.f,0.f));

	matStart.r[3] += matStart.r[2] * m_fCamZoomVal;
	XMStoreFloat4x4(&m_matBeginWorld,matStart);
}



void CSubCamera::Start_RippedOff_BossLaser()
{
	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
	m_eCurCamMode = CamMode::Cam_RippedOffBossLaser;
}

void CSubCamera::Start_HitRocket_Boss()
{
	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
	m_dHitRocketTime = 0.0;
	m_eCurCamMode = CamMode::Cam_Boss_HitRocket;
}

HRESULT CSubCamera::Start_Film(const _tchar * pFilmTag)
{
	m_pCamHelper->Start_Film(pFilmTag, CFilm::RScreen);
	return S_OK;
}

HRESULT CSubCamera::Start_CamEffect(const _tchar * pEffectTag)
{
	m_pCamHelper->Start_CamEffect(pEffectTag, CFilm::RScreen);
	return S_OK;
}



_int CSubCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;
	_int iResult = NO_EVENT;
	switch (m_eCurCamFreeOption)
	{
	case CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
		iResult = Tick_Cam_Free_FollowPlayer(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_FreeMove:
		iResult = Tick_Cam_Free_FreeMode(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_RidingSpaceShip_May:
		iResult = Tick_Cam_Free_RideSpaceShip_May(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_OpenThirdFloor:
		iResult = Tick_Cam_Free_OpenThirdFloor(dTimeDelta);
		break;
	}
	return iResult;
}


_int CSubCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;
	if (m_fChangeCamModeTime >= 1.f)
	{
		m_eCurCamMode = CamMode::Cam_Free;
		return NO_EVENT;
	}
	CTransform* pPlayerTransform = m_pMay->Get_Transform();
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		m_CameraDesc.fFovY = XMConvertToRadians(60.f);
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //시작 매트릭스
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(XMLoadFloat4x4(&m_matCurWorld), MakeViewMatrix_FollowPlayer(dTimeDelta), m_fChangeCamModeTime));

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Warp_WormHole(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;


	CTransform* pPlayerTransform = m_pMay->Get_Transform();
	if (m_dWarpTime < 2.0) //카메라가 게이트 전방으로,페이드인.
	{
		_matrix matPortal = XMLoadFloat4x4(&m_matStartPortal);
		_float4 vTargetEye, vTargetAt;
		XMStoreFloat4(&vTargetEye, matPortal.r[3] + 8 * XMVector3Normalize(matPortal.r[2]));
		XMStoreFloat4(&vTargetAt, matPortal.r[3]);
		vTargetEye.y += 7.0f;
		vTargetAt.y += 6.5f;
		if (false == m_bIsFading && m_dWarpTime > 0.1)
		{
			UI_CreateOnlyOnce(May, WhiteScreenFadeInOut);
			UI_Generator->Set_FadeInSpeed(Player::May, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);

			m_bIsFading = true;
		}

		m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vTargetEye, vTargetAt), (_float)m_dWarpTime));
		m_dWarpTime += dTimeDelta;
	}
	else if (m_pMay->Get_IsWarpNextStage() && m_pMay->Get_IsWarpDone()) //게이트안에서,페이드아웃
	{
		if (m_bIsFading)
		{
			UI_Generator->Set_FadeOut(Player::May, UI::WhiteScreenFadeInOut);
			UI_Generator->Set_FadeOutSpeed(Player::May, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);
			m_bIsFading = false;
		}
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
		_vector vCamPos = vPlayerPos - vPlayerLook * 3.f + vPlayerUp * 1.5f;
		_float4 vEye, vAt;
		XMStoreFloat4(&vEye, vCamPos);
		XMStoreFloat4(&vAt, vPlayerPos + vPlayerUp* 1.5f);
		_matrix matResult = MakeViewMatrixByUp(vEye, vAt);
		m_pCamHelper->Start_CamEffect(TEXT("Cam_Shake_Loc_Right_Warp_Potal"), CFilm::RScreen);
		m_pCamHelper->Tick_CamEffect(CFilm::RScreen, dTimeDelta, matResult);
		m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::RScreen));
	}
	else if (m_pMay->Get_IsWarpDone())
	{
		//페이드인
		if (!m_bIsFading)
		{
			UI_CreateOnlyOnce(May, WhiteScreenFadeInOut);
			UI_Generator->Set_FadeInSpeed(Player::May, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);
			m_bIsFading = true;
		}
	}
	else
	{
		//페이드아웃
		if (m_bIsFading)
		{
			UI_Generator->Set_FadeOut(Player::May, UI::WhiteScreenFadeInOut);
			UI_Generator->Set_FadeOutSpeed(Player::May, UI::WhiteScreenFadeInOut, (_float)dTimeDelta*2.f * 255.f);
			m_bIsFading = false;
		}
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);

		m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vPlayerPos - vPlayerLook * 2.f, vPlayerPos));
		m_dWarpTime = 0.0;
		ReSet_Cam_FreeToAuto(true);
		m_fChangeCamModeLerpSpeed = 4.f;
	}
	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_PinBall_May(_double dTimeDelta)
{
	if (false == m_pMay->Get_IsPinBall())
		ReSet_Cam_FreeToAuto();
	CPinBall_Handle* pPinBallHandle = static_cast<CPinBall_Handle*>(DATABASE->Get_Pinball_Handle());
	CTransform* pHandleTransform = pPinBallHandle->Get_Transform();

	_vector vHandlePos = pHandleTransform->Get_State(CTransform::STATE_POSITION);
	_vector vEye = vHandlePos + XMVectorSet(0.f, 3.f, 4.f, 0.f);
	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vHandlePos + XMVectorSet(0.f,2.f,-1.f,0.f)), (_float)dTimeDelta * 4.f));

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_WallJump(_double dTimeDelta)
{
	if (m_pMay->Get_IsWallJump() == false)
	{
		ReSet_Cam_FreeToAuto(true,true);
	}
	CTransform* pPlayerTransform = m_pMay->Get_Transform();

	_matrix matFacetoWall = m_pMay->Get_CameraTrigger_Matrix();
	_vector vProgressDir = matFacetoWall.r[1];
	_vector vTriggerPos = m_pMay->Get_CamTriggerPos();
	_float fAxisX = fabs(XMVectorGetX(vProgressDir));
	_float fAxisY = fabs(XMVectorGetY(vProgressDir));
	_float fAxisZ = fabs(XMVectorGetZ(vProgressDir));

	_float fMax = fmax(fmax(fAxisX, fAxisY), fAxisZ);
	_vector vPlayerPos = m_pMay->Get_Position();
	_vector vEye = matFacetoWall.r[3];
	_vector vPlayerScale = XMVectorSet(pPlayerTransform->Get_Scale(CTransform::STATE_RIGHT),
		pPlayerTransform->Get_Scale(CTransform::STATE_UP), pPlayerTransform->Get_Scale(CTransform::STATE_LOOK), 0.f);
	if (fMax == fAxisY)
	{
		vPlayerPos = XMVectorSetY(vTriggerPos, XMVectorGetY(vPlayerPos) + XMVectorGetY(vPlayerScale));
		vEye = XMVectorSetY(vEye, XMVectorGetY(vPlayerPos));
	}
	else if (fMax == fAxisZ)
	{
		vEye = XMVectorSetZ(vEye, XMVectorGetZ(vPlayerPos) + XMVectorGetZ(vPlayerScale));
		vPlayerPos = XMVectorSetZ(vTriggerPos, XMVectorGetZ(vPlayerPos));
	}
	else if (fMax == fAxisX)
	{
		vEye = XMVectorSetX(vEye, XMVectorGetX(vPlayerPos) + XMVectorGetX(vPlayerScale));
		vPlayerPos = XMVectorSetX(vTriggerPos, XMVectorGetX(vPlayerPos));
	}
	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vPlayerPos, matFacetoWall.r[1]), (_float)dTimeDelta * 3.f));


	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_LaserTennis(_double dTimeDelta)
{
	if (CLaserTennis_Manager::GetInstance()->Get_StartGame() == false)
	{
		if (CLaserTennis_Manager::GetInstance()->Get_Winner() == CLaserTennis_Manager::TARGET_CODY)
		{
			if (UI_Generator->Get_EmptyCheck(Player::May, UI::BlackScreenFadeInOut))
			{
				UI_CreateOnlyOnce(May, BlackScreenFadeInOut);
				UI_Generator->Set_FadeInSpeed(Player::May, UI::BlackScreenFadeInOut, 5.f);
			}
			if (static_cast<CAlphaScreen*>(UI_Generator->Get_UIObject(Player::May, UI::BlackScreenFadeInOut))->Get_Alpha() >= 1.f)
			{
				ReSet_Cam_FreeToAuto(true);
				((CMainCamera*)(DATABASE->Get_MainCam()))->ReSet_Cam_FreeToAuto(true);
			}
		}
		return NO_EVENT;
	}
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	_vector vCodyPos = XMVectorSetY(m_pMay->Get_Position(), 730.f);
	_vector vMayPos = XMVectorSetY(pCody->Get_Position(), 730.f);
	_vector vTargetPos = XMVectorSet(60.479f, 753.281f, 984.f, 1.f);

	_vector vMiddlePos = vCodyPos - (vCodyPos - vMayPos)*0.5f;

	vMiddlePos = XMVectorSetY(vMiddlePos, 730.f);
	vMiddlePos = XMVectorSetZ(vMiddlePos, 998.f);

	vTargetPos = XMVectorSetX(vTargetPos, XMVectorGetX(vMiddlePos));
	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vTargetPos, vMiddlePos), (_float)dTimeDelta));
	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_RippedOff_BossLaser(_double dTimeDelta)
{
	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	if (pUfo->Get_IsRidLaserGun() == true)
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
		ReSet_Cam_FreeToAuto(false,false,1.f);
		return NO_EVENT;
	}
	CTransform* pMayTransform = m_pMay->Get_Transform();
	CTransform* pUfoTransform = pUfo->Get_Transform();

	_matrix matLaserGunRing = pUfo->Get_Model()->Get_BoneMatrix("LaserGunRing3") * pUfoTransform->Get_WorldMatrix();

	_vector vMayPos = pMayTransform->Get_State(CTransform::STATE_POSITION);
	vMayPos = XMVectorSetY(vMayPos, XMVectorGetY(vMayPos) + 1.f);
	_vector vLaserGunPos = matLaserGunRing.r[3];

	_vector vDir = XMVector3Normalize(vLaserGunPos - vMayPos);

	_vector vLastEye = vMayPos - vDir;
	_vector vLastAt = vLaserGunPos + vDir;

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vLastEye, vLastAt), (_float)dTimeDelta));

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_HitRocket_Boss(_double dTimeDelta)
{

	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	if (pUfo->Get_Model()->Get_CurAnimIndex() == UFO_RocketKnockDown_MH)
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
		ReSet_Cam_FreeToAuto(false, false, 3.f);
	}
	CTransform* pUfoTransform = pUfo->Get_Transform();

	_matrix matUfo = pUfo->Get_Model()->Get_BoneMatrix("Chair") * pUfoTransform->Get_WorldMatrix();
	_vector vUfoPos = matUfo.r[3];
	_vector vUfoRight = XMVector3Normalize(matUfo.r[0]);
	_vector vUfoUp = XMVector3Normalize(matUfo.r[1]);
	_vector vUfoLook = XMVector3Normalize(matUfo.r[2]);
	_vector vLastEye = vUfoPos + vUfoRight* 10.f + vUfoUp*6.f - vUfoLook*4.f;

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vLastEye, vUfoPos), (_float)dTimeDelta));
	return NO_EVENT;
}


_int CSubCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{
	m_pTransformCom->Set_WorldMatrix(MakeViewMatrix_FollowPlayer(dTimeDelta));
	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	return _int();
}

_int CSubCamera::Tick_Cam_Free_RideSpaceShip_May(_double dTimeDelta)
{
	if (m_pMay->Get_IsInUFO() == false)
	{
		ReSet_Cam_FreeToAuto();
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
		return NO_EVENT;
	}
	CTransform* pPlayerTransform = static_cast<CMoonUFO*>(DATABASE->Get_MoonUFO())->Get_Transform();

	CMoon* pMoon = static_cast<CMoon*>(DATABASE->Get_Mooon());
	if (nullptr == pMoon)
		return EVENT_ERROR;
	
	_vector vMoonPos = pMoon->Get_Position();
	_vector vAt = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDirMoonWithAt = XMVector3Normalize(vAt - vMoonPos);


	_matrix matBegin = XMLoadFloat4x4(&m_matBeginWorld);
	_matrix matPlayer = pPlayerTransform->Get_WorldMatrix();
	_vector vEye = vAt - matPlayer.r[2] * 10.f + matPlayer.r[1] * 9.f;
	OffSetPhsX(vEye, vAt, dTimeDelta, &vEye);
	_vector vDirMoonWithEye = XMVector3Normalize(vEye - vMoonPos);
	_vector vLook = XMVector3Normalize(vAt - vEye);
	
	_vector vRight = XMVector3Normalize(XMVector3Cross(vDirMoonWithEye, vLook));
	
	_float fAngle = acosf(XMVectorGetX(XMVector3Dot(vDirMoonWithEye,vDirMoonWithAt)));
	
	_vector vAxis = XMQuaternionNormalize(XMQuaternionRotationAxis(XMVector3Normalize(vRight/*matPlayer.r[0]*/),-fAngle/* +  XMConvertToRadians(45.f)*/));

	_vector vAxisConj = XMQuaternionNormalize(XMQuaternionConjugate(vAxis));
	_vector vOrigin = XMQuaternionNormalize(XMQuaternionRotationMatrix(matBegin *matPlayer));
	vOrigin = XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(vAxis, vOrigin),vAxisConj));
	
	_matrix matRot = XMMatrixRotationQuaternion(vOrigin);
	matRot.r[3] = vEye;
	m_pTransformCom->Set_WorldMatrix(matRot);
	


	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_OpenThirdFloor(_double dTimeDelta)
{
	m_fOpenThirdFloorTime += (_float)dTimeDelta;

	_float fDelay = 2.f;
	if (m_fOpenThirdFloorTime > fDelay * 9.f)
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
		ReSet_Cam_FreeToAuto(true);
	}

	_vector vMayPos = m_pMay->Get_Position();
	_vector vUpPos = XMVectorSet(65.f,218.f,179.f,1.f);
	_vector vMiddleDir = (vMayPos - vUpPos);
	_vector vLook = XMVector3Normalize(vMiddleDir);
	_vector vMiddlePos = vMayPos - vMiddleDir * 0.5f;
	_vector vTargetPos = vMiddlePos;

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_vector vEye = vMayPos + XMVectorSet(XMVectorGetX(vUp)* 100.f , 0.f , XMVectorGetZ(vUp) * 100.f,0.f);
	vEye = XMVectorSetY(vEye, XMVectorGetY(vMiddlePos));
	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vMiddlePos),(_float)dTimeDelta));


	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vMiddlePos), (_float)dTimeDelta));

	return NO_EVENT;
}



_int CSubCamera::Tick_CamHelperNone(_double dTimeDelta)
{
	if (m_eCurCamMode != m_ePreCamMode)
	{
		switch (m_eCurCamMode)
		{
		case Client::CSubCamera::CamMode::Cam_Free:
			if (false == UI_Generator->Get_EmptyCheck(Player::May, UI::BlackScreenFadeInOut))
			{
				UI_Generator->Set_FadeOut(Player::May, UI::BlackScreenFadeInOut);
				UI_Generator->Set_FadeOutSpeed(Player::May, UI::BlackScreenFadeInOut, 6.f);
			}
			break;
		case Client::CSubCamera::CamMode::Cam_AutoToFree:
			break;
		case Client::CSubCamera::CamMode::Cam_Warp_WormHole:
			break;
		case Client::CSubCamera::CamMode::Cam_PinBall_May:
			break;
		case Client::CSubCamera::CamMode::Cam_LaserTennis:
			break;
		case Client::CSubCamera::CamMode::Cam_WallJump:
			break;
		}
		m_ePreCamMode = m_eCurCamMode;
	}
	if (m_eCurCamFreeOption != m_ePreCamFreeOption)
	{
		switch (m_eCurCamFreeOption)
		{
		case Client::CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
			ReSet_Cam_FreeToAuto();
			break;
		case Client::CSubCamera::CamFreeOption::Cam_Free_FreeMove:
			break;
		case Client::CSubCamera::CamFreeOption::Cam_Free_RidingSpaceShip_May:
			break;
		}
		m_ePreCamFreeOption = m_eCurCamFreeOption;
	}



	_int iResult = NO_EVENT;
	switch (m_eCurCamMode)
	{
	case Client::CSubCamera::CamMode::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_AutoToFree:
		iResult = Tick_Cam_AutoToFree(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_Warp_WormHole:
		iResult = Tick_Cam_Warp_WormHole(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_PinBall_May:
		iResult = Tick_Cam_PinBall_May(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_WallJump:
		iResult = Tick_Cam_WallJump(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_LaserTennis:
		iResult = Tick_Cam_LaserTennis(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_RippedOffBossLaser:
		iResult = Tick_Cam_RippedOff_BossLaser(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_Boss_HitRocket:
		iResult = Tick_Cam_HitRocket_Boss(dTimeDelta);
		break;
	}
	return iResult;
}

_int CSubCamera::Tick_CamHelper_Act(_double dTimeDelta)
{
	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;
	_float fFovY = 0.f;
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Tick_Film(dTimeDelta, CFilm::RScreen, &fFovY));
	if (fFovY == 0.f)
		return NO_EVENT;
	m_CameraDesc.fFovY = XMConvertToRadians(fFovY);
	return NO_EVENT;
}

_int CSubCamera::Tick_CamHelper_SeeCamNode(_double dTimeDelta)
{
	if (m_pCamHelper == nullptr)
		return EVENT_ERROR;
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CamNodeMatrix(m_pTransformCom, dTimeDelta, CFilm::RScreen));
	return NO_EVENT;
}

_int CSubCamera::ReSet_Cam_FreeToAuto(_bool bCalculatePlayerLook,_bool bIsCalculateCamLook,_float fLerpSpeed)
{

	m_fChangeCamModeLerpSpeed = fLerpSpeed;
	m_fChangeCamModeTime = 0.f;
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	for (_uint i = 0; i < Rev_End; i++)
	{
		m_fMouseRev[i] = 0.f;
		m_fCurMouseRev[i] = 0.f;
	}
	if (bCalculatePlayerLook)
	{
		_vector vOriginAxis = 
			bIsCalculateCamLook ? m_pTransformCom->Get_State(CTransform::STATE_LOOK): XMVectorSet(0.f, 0.f, 1.f, 0.f);
		_vector vRotAxis = XMVector3Normalize(m_pMay->Get_Transform()->Get_State(CTransform::STATE_LOOK));
		_float fAxisX = XMVectorGetX(vRotAxis);
		_float fDot = acosf(XMVectorGetX(XMVector3Dot(vRotAxis, vOriginAxis)));
		if (fAxisX < 0.f)
			fDot = -fDot;
		_float fCalculateRotation = XMConvertToDegrees(fDot);
		m_fCurMouseRev[Rev_Holizontal] = fCalculateRotation;
		m_fMouseRev[Rev_Holizontal] = fCalculateRotation;

	}

	return NO_EVENT;
}

_bool CSubCamera::OffSetPhsX(_fvector vEye, _fvector vAt, _double dTimeDelta, _vector * pOut)
{
	_bool isHit = false;

	_vector vDistanceFromCam = vEye - vAt;
	_vector vDir = XMVector3Normalize(vDistanceFromCam);
	_float	fDist = XMVectorGetX(XMVector3Length(vDistanceFromCam));

	if (m_pGameInstance->Raycast(MH_PxVec3(vAt), MH_PxVec3(vDir), fDist, m_RayCastBuffer, PxHitFlag::eDISTANCE))
	{
		for (PxU32 i = 0; i < m_RayCastBuffer.getNbAnyHits(); ++i)
		{
			USERDATA* pUserData = (USERDATA*)m_RayCastBuffer.getAnyHit(i).actor->userData;

			if (nullptr != pUserData)
			{
				if (pUserData->eID == GameID::eCODY || pUserData->eID == GameID::eMAY)
					continue;
				fDist = m_RayCastBuffer.getAnyHit(i).distance;
			}
			else
				fDist = m_RayCastBuffer.getAnyHit(i).distance;

		}
	}
	*pOut = vAt + vDir * fDist;
	return true;
}

_fmatrix CSubCamera::MakeViewMatrixByUp(_float4 Eye, _float4 At, _fvector vUp)
{

	_matrix Result = XMMatrixIdentity();
	_vector vNormalizedUp = XMVectorSetW(XMVector3Normalize(vUp), 0.f);
	_vector vPos = XMVectorSetW(XMLoadFloat4(&Eye), 1.f);
	_vector vLook = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&At), 1.f) - vPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));
	vNormalizedUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	Result.r[0] = vRight;
	Result.r[1] = vNormalizedUp;
	Result.r[2] = vLook;
	Result.r[3] = vPos;

	return Result;

}

_fmatrix CSubCamera::MakeViewMatrixByUp(_fvector vEye, _fvector vAt, _fvector vUp)
{
	_matrix Result = XMMatrixIdentity();
	_vector vNormalizedUp = XMVectorSetW(XMVector3Normalize(vUp), 0.f);
	_vector vPos = XMVectorSetW(vEye, 1.f);
	_vector vLook = XMVector3Normalize(XMVectorSetW(vAt, 1.f) - vPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));
	vNormalizedUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	Result.r[0] = vRight;
	Result.r[1] = vNormalizedUp;
	Result.r[2] = vLook;
	Result.r[3] = vPos;

	return Result;
}

_fmatrix CSubCamera::MakeViewMatrixByQuaternion(_fvector vEye, _fvector vAt, _fvector vUp)
{
#define m00 vRight.x
#define m01 vNormalUp.x
#define m02 vLook.x
#define m10 vRight.y
#define m11 vNormalUp.y
#define m12 vLook.y
#define m20 vRight.z
#define m21 vNormalUp.z
#define m22 vLook.z

	_vector vAxisY = XMVector3Normalize(vUp);
	_vector vAxisZ = XMVector3Normalize(vAt - vEye);
	_vector vAxisX = XMVector3Normalize(XMVector3Cross(vAxisY, vAxisZ));

	
	_float3 vNormalUp, vLook, vRight;

	XMStoreFloat3(&vLook, vAxisZ);
	XMStoreFloat3(&vRight,vAxisY);
	XMStoreFloat3(&vNormalUp, XMVector3Normalize(XMVector3Cross(vAxisZ, vAxisX)));
	_float4 ret;
	ret.w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
	_float w4_recip = 1.0f / (4.0f * ret.w);
	ret.x = (m21 - m12) * w4_recip;
	ret.y = (m02 - m20) * w4_recip;
	ret.z = (m10 - m01) * w4_recip;

	_vector vQuat = XMLoadFloat4(&ret);

	_matrix matResult = XMMatrixRotationQuaternion(vQuat);
	matResult.r[3] = vEye;
	return matResult;

}

_fmatrix CSubCamera::MakeLerpMatrix(_fmatrix matDst, _fmatrix matSour, _float fTime)
{
	if (fTime >= 1.f)
		return matSour;
	_vector	  vPreRight = matDst.r[0], vNextRight = matSour.r[0]
		, vPreUp = matDst.r[1], vNextUp = matSour.r[1]
		, vPreLook = matDst.r[2], vNextLook = matSour.r[2]
		, vPrePos = matDst.r[3], vNextPos = matSour.r[3];

	_vector vCurRight = XMVectorLerp(vPreRight, vNextRight, fTime),
		vCurUp = XMVectorLerp(vPreUp, vNextUp, fTime),
		vCurLook = XMVectorLerp(vPreLook, vNextLook, fTime),
		vCurPos = XMVectorLerp(vPrePos, vNextPos, fTime);

	_matrix matCurWorld = XMMatrixIdentity();
	matCurWorld.r[0] = vCurRight;
	matCurWorld.r[1] = vCurUp;
	matCurWorld.r[2] = vCurLook;
	matCurWorld.r[3] = vCurPos;

	return matCurWorld;
}

_fvector CSubCamera::MakeQuatMul(_fvector vQ, _fvector vP)
{
	_vector QVector = XMVectorSetW(vQ, 0.f),PVector = XMVectorSetW(vP,0.f);
	_float	QReal = XMVectorGetW(vQ), PReal = XMVectorGetW(vP);
	_float rW = QReal* PReal - XMVectorGetX(XMVector3Dot(QVector,PVector));
	_vector rV = PVector * QReal + QVector * PReal  + XMVector3Cross(QVector, PVector);
	
	return XMVectorSet(XMVectorGetX(rV), XMVectorGetY(rV), XMVectorGetZ(rV), rW);
}

_fmatrix CSubCamera::MakeViewMatrix_FollowPlayer(_double dTimeDelta)
{
	CTransform* pPlayerTransform = m_pMay->Get_Transform();
	_long MouseMove = 0;

#ifdef __CONTROL_MAY_KEYBOARD
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X)/*m_pGameInstance->Get_Pad_RStickX() - 32767*/)
		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal];
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y)/*(65535 - m_pGameInstance->Get_Pad_RStickY()) - 32767*/)
	{
		//if (abs(MouseMove) < 2000)
		//	MouseMove = 0;
		//else
		//	MouseMove = MouseMove / 2000;


		_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);

		if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
			m_fMouseRev[Rev_Prependicul] = 40.f;
		else if (m_fMouseRev[Rev_Prependicul] + fVal < -80.f)
			m_fMouseRev[Rev_Prependicul] = -80.f;
		else
			m_fMouseRev[Rev_Prependicul] += fVal;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;
#else
	if (MouseMove = m_pGameInstance->Get_Pad_RStickX() - 32767)
	{
		if (abs(MouseMove) < 2000)
			MouseMove = 0;
		else
			MouseMove = MouseMove / 700;
		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta * m_fMouseRevSpeed[Rev_Holizontal];

	}
	if (MouseMove = (/*65535 - */m_pGameInstance->Get_Pad_RStickY()) - 32767)
	{
		if (abs(MouseMove) < 2000)
			MouseMove = 0;
		else
			MouseMove = MouseMove / 700;


		_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);

		if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
			m_fMouseRev[Rev_Prependicul] = 40.f;
		else if (m_fMouseRev[Rev_Prependicul] + fVal < -80.f)
			m_fMouseRev[Rev_Prependicul] = -80.f;
		else
			m_fMouseRev[Rev_Prependicul] += fVal;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;
#endif
	//카메라 움직임이 끝나고 체크할것들
	//SoftMoving
	_vector vTargetPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_vector vPlayerUp = XMLoadFloat4(&m_vPlayerUp);
	_vector vUpDir = (vTargetPlayerUp - vPlayerUp);
	if (XMVectorGetX(XMVector4Length(vUpDir)) > 0.01f)
		vPlayerUp += vUpDir * (_float)dTimeDelta * 5.f;


	vPlayerUp = m_pMay->Get_IsInGravityPipe() || m_pMay->Get_IsInRocket()? XMVectorSet(0.f, 1.f, 0.f, 0.f) : vPlayerUp;
	XMStoreFloat4(&m_vPlayerUp, vPlayerUp);

	//_vector vPlayerUp = vTargetPlayerUp;
	//XMStoreFloat4(&m_vPlayerUp, vPlayerUp);


	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vCurUp = XMVector3TransformNormal(XMVectorSetW(XMLoadFloat4(&m_vStartAt), 0.f), 
	//	/*MakeRotationByQuaternion(XMVectorSet(0.f,0.f,0.f,1.f),vPlayerUp)*/MH_RotationMatrixByUp(vPlayerUp));
	//vCurPlayerPos += vCurUp;
	_vector vCalculateUp = XMVector3TransformNormal(XMVectorSetW(XMLoadFloat4(&m_vStartAt), 0.f), MH_RotationMatrixByUp(vPlayerUp));
	vCurPlayerPos += vCalculateUp;
	//카메라와 플레이어의 실제 거리
	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));

	vPlayerPos = fDist > 10.f ? vCurPlayerPos : XMVectorLerp(vPrePlayerPos, vCurPlayerPos, XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 10.f);


	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)
	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),
		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);

	_matrix matQuat = XMMatrixRotationQuaternion(vCurQuartRot);

	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat, vCurTrans;


	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans,
		XMLoadFloat4x4(&m_matBeginWorld) * matQuat *
		MH_RotationMatrixByUp(vPlayerUp, vPlayerPos));

	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);


	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f);
	vCurTrans = XMVectorLerp(vPreTrans, vTrans, 0.5f);

	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);


	_matrix matAffine = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		vCurRotQuat, vCurTrans);


	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::RScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::RScreen, dTimeDelta, matAffine)) //카메라의 원점 
		{
			matAffine = m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::RScreen);
			vPlayerUp = XMVector3TransformNormal(vPlayerUp, matAffine);
		}
	}
	_vector vResultPos = matAffine.r[3];
	OffSetPhsX(matAffine.r[3], vPlayerPos, dTimeDelta, &vResultPos);

	//return MakeViewMatrixByQuaternion(vResultPos, vPlayerPos, vPlayerUp);
	return MakeViewMatrixByUp(vResultPos, vPlayerPos, vPlayerUp/*matAffine.r[2]*/);

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
	Safe_Release(m_pMay);
	Safe_Release(m_pCamHelper);

	CCamera::Free();
}
