#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "Cody.h"
#include "CameraActor.h"
#include "PlayerActor.h"
#include"Bridge.h"
#include"CutScenePlayer.h"

CMainCamera::CMainCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CMainCamera::CMainCamera(const CMainCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMainCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	CCameraActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eMAINCAMERA, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);


	CDataStorage::GetInstance()->Set_MainCamPtr(this);

	m_eCurCamMode = CamMode::Cam_AutoToFree;
	

	m_PreWorld.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	m_PreWorld.vRotQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_PreWorld.vTrans = _float4(0.f, 0.f, 0.f, 1.f);
	
	/* Hye */
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_SMALL] =	{ 0.f,2.f,-2.f,1.f};
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM] =	{ 0.f,7.f,-7.f,1.f };
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_LARGE] =	{ 0.f,9.f,	-9.f,1.f };

	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_SMALL] = { 0.f,0.2f,0.0f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM] = { 0.f,3.f, 0.0f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_LARGE] = { 0.f,4.f,0.0f,1.f };


	_matrix matStart = MakeViewMatrixByUp(m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM], m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM]);
	XMStoreFloat4x4(&m_matBeginWorld, matStart);
	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (false == m_bStart)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetCody();
		if (m_pTargetObj)
		{
			CTransform* pPlayerTransform = static_cast<CCody*>(m_pTargetObj)->Get_Transform();
			XMStoreFloat4(&m_vPlayerPos, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vPlayerUp, pPlayerTransform->Get_State(CTransform::STATE_UP));
			Safe_AddRef(m_pTargetObj);
		}
		else
			return EVENT_ERROR;
		m_bStart = true;
	}


	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;


	Check_Player(dTimeDelta);

	_int iResult = NO_EVENT;
	

	switch (m_pCamHelper->Tick(dTimeDelta,CFilm::LScreen))
	{
	case CCam_Helper::CamHelperState::Helper_None:
		iResult = Tick_CamHelperNone(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_Act:
		ReSet_Cam_FreeToAuto();
		iResult = Tick_CamHelper_Act(dTimeDelta);
		break;
	case CCam_Helper::CamHelperState::Helper_SeeCamNode:
		ReSet_Cam_FreeToAuto();
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

_int CMainCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);



	return NO_EVENT;
}


void CMainCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return;
	CCody* pTargetPlayer = static_cast<CCody*>(m_pTargetObj);

	
	m_eCurPlayerSize = pTargetPlayer->Get_Player_Size();
	if (m_eCurPlayerSize != m_ePrePlayerSize)
	{
		m_dLerpToCurSizeTime = 0.0;
		m_ePrePlayerSize = m_eCurPlayerSize;
	}
	LerpToCurSize(m_eCurPlayerSize, dTimeDelta);
	
	if (static_cast<CCody*>(m_pTargetObj)->Get_IsWarpNextStage() == true)
	{
		m_eCurCamMode = CamMode::Cam_Warp_WormHole;
	}
	if (DATABASE->Get_BigButtonPressed() && static_cast<CBridge*>(DATABASE->Get_Bridge())->Get_IsUppendede() == false)
	{
		m_eCurCamMode = CamMode::Cam_PressButton_Bridge;
	}

}

void CMainCamera::Set_Zoom(_float fZoomVal, _double dTimeDelta)
{
	m_fCamZoomVal += (fZoomVal - m_fCamZoomVal) * (_float)dTimeDelta;
	//현재크기에서 줌인아웃
	_matrix matStart = MakeViewMatrixByUp(m_vSizeEye[m_eCurPlayerSize], m_vSizeAt[m_eCurPlayerSize],XMVectorSet(0.f,1.f,0.f,0.f));
	matStart.r[3] += matStart.r[2] * m_fCamZoomVal;
	XMStoreFloat4x4(&m_matBeginWorld, matStart);
}

_bool CMainCamera::LerpToCurSize(CCody::PLAYER_SIZE eSize,_double dTimeDelta)
{
	if (m_dLerpToCurSizeTime >= 1.f)
	{
		XMStoreFloat4x4(&m_matBeginWorld, MakeViewMatrixByUp(m_vSizeEye[eSize], m_vSizeAt[eSize]));
		return false;
	}
	_matrix CurStartMatrix = XMLoadFloat4x4(&m_matBeginWorld);
	_matrix TargetMatrix = MakeViewMatrixByUp(m_vSizeEye[eSize], m_vSizeAt[eSize]);

	m_dLerpToCurSizeTime += 1.f / 5.f;

	XMStoreFloat4x4(&m_matBeginWorld, MakeLerpMatrix(CurStartMatrix, TargetMatrix, (_float)m_dLerpToCurSizeTime));
	return true;
}

HRESULT CMainCamera::Start_Film(const _tchar * pFilmTag)
{
	m_pCamHelper->Start_Film(pFilmTag, CFilm::LScreen);

	return S_OK;
}



_int CMainCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;
	_int iResult = NO_EVENT;
	switch (m_eCurCamFreeOption)
	{
	case CMainCamera::CamFreeOption::Cam_Free_FollowPlayer:
		iResult = Tick_Cam_Free_FollowPlayer(dTimeDelta);
		break;
	case CMainCamera::CamFreeOption::Cam_Free_FreeMove:
		iResult = Tick_Cam_Free_FreeMode(dTimeDelta);
		break;
	case CMainCamera::CamFreeOption::Cam_Free_OnRail:
		iResult = Tick_Cam_Free_OnRail(dTimeDelta);
		break;
	}
	return iResult;
}

_int CMainCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{

	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;
	if (m_fChangeCamModeTime >= 1.f)
	{
		_vector vScale, vRotQuat, vTrans;
		XMMatrixDecompose(&vScale, &vRotQuat, &vTrans,m_pTransformCom->Get_WorldMatrix()); //계산 완료한 이번 프레임의 월드
		XMStoreFloat4(&m_PreWorld.vTrans, vTrans);
		XMStoreFloat4(&m_PreWorld.vRotQuat, vRotQuat);

		m_eCurCamMode = CamMode::Cam_Free;
		return NO_EVENT;
	}
	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();
	if (m_fChangeCamModeTime == 0.f) 
	{
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); 
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld);

	_matrix matRev = XMMatrixRotationQuaternion(XMLoadFloat4(&m_PreWorld.vRotQuat)) *
		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos);


	_matrix matWorld = XMLoadFloat4x4(&m_matCurWorld);
	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

	matNext *= matRev;

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(matWorld, matNext, m_fChangeCamModeTime));

	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{
	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();

	_long MouseMove = 0;
	
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
		m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
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
	

	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vCurPlayerPos = XMVectorSetY(vCurPlayerPos, m_vSizeAt[m_eCurPlayerSize].y);

	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));

	vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos, XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 10.f);
	

	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)

	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]) ,
		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal] ) ,0.f);


	//_matrix matQuat = ;

	
	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat,vCurTrans;
	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMLoadFloat4x4(&m_matBeginWorld) * XMMatrixRotationQuaternion(vCurQuartRot)
		*MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos));


	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);

	vCurRotQuat =	XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f);
	vCurTrans =		XMVectorLerp(vPreTrans, vTrans,0.5f );

	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);

	_matrix matAffine = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		vCurRotQuat, vCurTrans);
	
	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, matAffine))
			matAffine =  m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen);
	}
	
	_vector vResultPos = XMVectorZero();
	 OffSetPhsX(matAffine, dTimeDelta, &vResultPos); 
	
	m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vResultPos, vPlayerPos,pPlayerTransform->Get_State(CTransform::STATE_UP)/*matAffine.r[2]*/));

	return NO_EVENT;
}
#pragma region DEBUG_FREEMOVE
_int CMainCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, m_pTransformCom->Get_WorldMatrix()))
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));

	}
	KeyCheck(dTimeDelta);
	return NO_EVENT;
}
void CMainCamera::KeyCheck(_double dTimeDelta)
{

	_long MouseMove = 0;
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (_float)MouseMove * 0.1f * dTimeDelta);

	}
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
		m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)MouseMove * dTimeDelta* 0.1f);
	}
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD4))
	{
		m_pTransformCom->Go_Left(dTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_NUMPAD6))
	{
		m_pTransformCom->Go_Right(dTimeDelta);
	}
}
#pragma endregion
_int CMainCamera::Tick_Cam_Free_OnRail(_double dTimeDelta)
{
	return NO_EVENT;
}
_int CMainCamera::Tick_Cam_Warp_WormHole(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;
	
	CCody* pCody = static_cast<CCody*>(m_pTargetObj);
	
	CTransform* pPlayerTransform = pCody->Get_Transform();
	if (m_dWarpTime < 2.0) //카메라가 게이트 전방으로,페이드인.
	{
		_matrix matPortal =XMLoadFloat4x4(&m_matStartPortal);
		_float4 vTargetEye, vTargetAt;
		XMStoreFloat4(&vTargetEye, matPortal.r[3] - 2*XMVector3Normalize(matPortal.r[2]));
		XMStoreFloat4(&vTargetAt, matPortal.r[3]);
		vTargetEye.y += 4.f;
		vTargetAt.y += 4.f;


		m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vTargetEye, vTargetAt),m_dWarpTime));
		m_dWarpTime += dTimeDelta / 2.0;
	}
	else if (pCody->Get_IsWarpNextStage() && pCody->Get_IsWarpDone()) //게이트안에서,페이드아웃
	{
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vCamPos = vPlayerPos - vPlayerLook * 2.f;
		_float4 vEye, vAt;
		XMStoreFloat4(&vEye, vCamPos);
		XMStoreFloat4(&vAt, vPlayerPos);
		m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vEye, vAt));
	}
	else if (pCody->Get_IsWarpDone())
	{
		//페이드아웃
	}
	else
	{
		m_dWarpTime = 0.0;
		ReSet_Cam_FreeToAuto();
		m_eCurCamMode = CamMode::Cam_AutoToFree;
	}
	return NO_EVENT;
}
_int CMainCamera::Tick_Cam_PressButton_Bridge(_double dTimeDelta)
{
	if (m_fBridgeUppendTime >= 1.f)
	{
		m_fBridgeUppendTime = 0.f;
		m_bStartBridgeUppendCam = false;
		ReSet_Cam_FreeToAuto();
		m_eCurCamMode = CamMode::Cam_AutoToFree;
		return NO_EVENT;
	}
	if (m_fBridgeUppendTime == 0.f)
	{
		for (_uint i = 0; i < 2; i++)
			m_fCurMouseRev[i] = fmodf(m_fCurMouseRev[i], 360.f);
	}

	_matrix matResult = XMMatrixIdentity();
	_vector vBridgePos = XMVectorSet(64.f, 17.f, 155.f , 1.f);
	_bool bIsFinishRev[Rev_End] = { false,false };
	_float fTargetRev[Rev_End];
	if (false == m_bStartBridgeUppendCam)
	{
		fTargetRev[Rev_Holizontal] = -45.f;
		fTargetRev[Rev_Prependicul] = -45.f;
	}
	else
	{
		fTargetRev[Rev_Holizontal] = 0.f;
		fTargetRev[Rev_Prependicul] = 0.f;
		m_fBridgeUppendTime += dTimeDelta;
	}
	for (_uint i = 0; i < 2; i++)
	{
		if (fabs(fTargetRev[i] - m_fCurMouseRev[i]) < 1.f)
			bIsFinishRev[i] = true;
		m_fCurMouseRev[i] += (fTargetRev[i] - m_fCurMouseRev[i])* dTimeDelta;
	}
	if (bIsFinishRev[Rev_Holizontal] && bIsFinishRev[Rev_Prependicul])
		m_bStartBridgeUppendCam = true;
	_matrix matCurSize = MakeViewMatrixByUp(m_vSizeEye[m_eCurPlayerSize], m_vSizeAt[m_eCurPlayerSize]);
	matCurSize.r[3] += matCurSize.r[2] * (m_fCurMouseRev[Rev_Prependicul]) * 0.08f;
	if (m_bStartBridgeUppendCam == false)
	{
		_vector vCamPos = (matCurSize*	XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f)
			*XMMatrixTranslation(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z)).r[3];
		matResult = MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix() , MakeViewMatrixByUp(vCamPos, vBridgePos), 0.1f);
	}
	else
	{
		matResult = MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix() , matCurSize*
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),
				XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f)
			*XMMatrixTranslation(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z), m_fBridgeUppendTime);
		/*_vector vOut = XMVectorZero();
		if (m_bIsCollision = OffSetPhsX(matResult, dTimeDelta, &vOut))
		{
			_float4 vEye, vAt;

			XMStoreFloat4(&vEye, vOut);
			XMStoreFloat4(&vAt,matResult.r[3] + matResult.r[2]);
			matResult = MakeViewMatrixByUp(vEye, vAt);
		}*/
	}
	m_pTransformCom->Set_WorldMatrix(matResult);
	
	return NO_EVENT;
}



_int CMainCamera::ReSet_Cam_FreeToAuto()
{
	m_fChangeCamModeLerpSpeed = 6.f;
	m_fChangeCamModeTime = 0.f;
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	for (_uint i = 0; i < Rev_End; i++)
	{
		m_fMouseRev[i] = 0.f;
		m_fCurMouseRev[i] = 0.f;
	}
	
	return NO_EVENT;
}

_bool CMainCamera::OffSetPhsX(_fmatrix matWorld, _double dTimeDelta,_vector * pOut)
{
	
	_bool isHit = false;

	_vector vAt = XMVectorSetW(XMLoadFloat4(&m_vPlayerPos),1.f);
	_vector vDistanceFromCam = matWorld.r[3] - vAt;
	_vector vDir = XMVector3Normalize(vDistanceFromCam);
	_float	fDist = XMVectorGetX(XMVector3Length(vDistanceFromCam));

	if (m_pGameInstance->Raycast(MH_PxVec3(vAt), MH_PxVec3(vDir),fDist, m_RayCastBuffer, PxHitFlag::eDISTANCE))
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


_fmatrix CMainCamera::MakeViewMatrixByUp(_float4 Eye, _float4 At, _fvector vUp)
{

	_matrix Result = XMMatrixIdentity();
	_vector vNormalizedUp = XMVectorSetW(XMVector3Normalize(vUp), 0.f);
	_vector vPos = XMVectorSetW(XMLoadFloat4(&Eye),1.f);
	_vector vLook = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&At),1.f) - vPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));
	vNormalizedUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	Result.r[0] = vRight;
	Result.r[1] = vNormalizedUp;
	Result.r[2] = vLook;
	Result.r[3] = vPos;

	return Result;
}

_fmatrix CMainCamera::MakeViewMatrixByUp(_fvector vEye, _fvector vAt, _fvector vUp)
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

_fmatrix CMainCamera::MakeLerpMatrix(_fmatrix matDst, _fmatrix matSour, _float fTime)
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

#pragma region Cam_Helper
_int CMainCamera::Tick_CamHelperNone(_double dTimeDelta)
{

	//외부에서 상태 설정 구간
#ifdef __TEST_JUN

	if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Rot_Right", CFilm::RScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_pCamHelper->Start_Film(L"Line_BossRoom_MiniCody",CFilm::LScreen);
		return NO_EVENT;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	{
		CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Intro");
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Active_GravityPath_01");
		return NO_EVENT;
	}
	/*if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		CCutScenePlayer::GetInstance()->Stop_CutScene();
		return NO_EVENT;
	}*/

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
		switch (m_eCurCamFreeOption)
		{
		case Client::CMainCamera::CamFreeOption::Cam_Free_FollowPlayer:
			ReSet_Cam_FreeToAuto();
			break;
		case Client::CMainCamera::CamFreeOption::Cam_Free_FreeMove:
			break;
		case Client::CMainCamera::CamFreeOption::Cam_Free_OnRail:
			break;
		}
		m_ePreCamFreeOption = m_eCurCamFreeOption;
	}

	_int iResult = NO_EVENT;
	switch (m_eCurCamMode)
	{
	case Client::CMainCamera::CamMode::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_AutoToFree:
		iResult = Tick_Cam_AutoToFree(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_Warp_WormHole:
		iResult = Tick_Cam_Warp_WormHole(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_PressButton_Bridge:
		iResult = Tick_Cam_PressButton_Bridge(dTimeDelta);
		break;
	}
	return iResult;
}

_int CMainCamera::Tick_CamHelper_Act(_double dTimeDelta)
{
	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;
	_float fFovY = 0.f;
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Tick_Film(dTimeDelta, CFilm::LScreen,&fFovY));
	if (fFovY == 0.f)
		return NO_EVENT;
	m_CameraDesc.fFovY = XMConvertToRadians(fFovY);
	return NO_EVENT;
}

_int CMainCamera::Tick_CamHelper_SeeCamNode(_double dTimeDelta)
{
	if (m_pCamHelper == nullptr)
		return EVENT_ERROR;
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CamNodeMatrix(m_pTransformCom, dTimeDelta, CFilm::LScreen));
	return NO_EVENT;
}

#pragma endregion
CMainCamera * CMainCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCamera::Clone_GameObject(void * pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainCamera::Free()
{
	Safe_Release(m_pTargetObj);
	Safe_Release(m_pCamHelper);

	CCamera::Free();

}
