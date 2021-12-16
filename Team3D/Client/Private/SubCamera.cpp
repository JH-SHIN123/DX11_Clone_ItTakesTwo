#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"
#include "PlayerActor.h"
#include "May.h"
#include "CameraActor.h"
#include"CutScenePlayer.h"
#include"Moon.h"
#include<complex>
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
	m_vStartAt =	{0.f,3.0f, 0.f, 1.f };



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
	if (m_eCurCamMode == Cam_Free)
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



HRESULT CSubCamera::Start_Film(const _tchar * pFilmTag)
{
	m_pCamHelper->Start_Film(pFilmTag, CFilm::RScreen);
	return S_OK;
}



_int CSubCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;

	switch (m_eCurCamFreeOption)
	{
	case CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
		Tick_Cam_Free_FollowPlayer(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_FreeMove:
		Tick_Cam_Free_FreeMode(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_RidingSpaceShip_May:
		Tick_Cam_Free_RideSpaceShip_May(dTimeDelta);
		break;
	}
	return NO_EVENT;
}


_int CSubCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{
	if (nullptr == m_pMay)
		return EVENT_ERROR;
	if (m_fChangeCamModeTime >= 1.f)
	{
		_vector vScale, vRotQuat, vTrans;
		XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, m_pTransformCom->Get_WorldMatrix()); //계산 완료한 이번 프레임의 월드
		XMStoreFloat4(&m_PreWorld.vTrans, vTrans);
		XMStoreFloat4(&m_PreWorld.vRotQuat, vRotQuat);

		m_eCurCamMode = CamMode::Cam_Free;
		return NO_EVENT;
	}
	CTransform* pPlayerTransform = m_pMay->Get_Transform();
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //시작 매트릭스
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld); //목표 매트릭스
	
	_matrix matRev = XMMatrixRotationQuaternion(XMLoadFloat4(&m_PreWorld.vRotQuat)) *
		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos);


	_matrix matWorld = XMLoadFloat4x4(&m_matCurWorld);
	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

	matNext *= matRev;

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(matWorld, matNext, m_fChangeCamModeTime));

	return NO_EVENT;
}


_int CSubCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
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
		
		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal];
		
	}
	if (MouseMove = (/*65535 - */m_pGameInstance->Get_Pad_RStickY()) - 32767)
	{
		if (abs(MouseMove) < 2000)
			MouseMove = 0;
		else
			MouseMove = MouseMove / 2000;


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
	_vector vTargetPlayerUp =pPlayerTransform->Get_State(CTransform::STATE_UP);
	_vector vPlayerUp = XMLoadFloat4(&m_vPlayerUp);
	_vector vUpDir = (vTargetPlayerUp - vPlayerUp);
	if(XMVectorGetX(XMVector4Length(vUpDir)) > 0.01f)
	vPlayerUp += vUpDir * (_float)dTimeDelta * 5.f;
	XMStoreFloat4(&m_vPlayerUp, vPlayerUp);

	//_vector vPlayerUp = vTargetPlayerUp;
	//XMStoreFloat4(&m_vPlayerUp, vPlayerUp);


	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCurUp = XMVector3TransformNormal(XMVectorSetW(XMLoadFloat4(&m_vStartAt), 0.f), MH_RotationMatrixByUp(vPlayerUp));
	vCurPlayerPos += vCurUp;
	/*_vector vCalculateUp = XMVector3TransformNormal(XMVectorSetW(XMLoadFloat4(&m_vStartAt), 0.f), MH_RotationMatrixByUp(vPlayerUp));
	vCurPlayerPos += vCalculateUp;*/
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
		XMLoadFloat4x4(&m_matBeginWorld) * matQuat *MH_RotationMatrixByUp(m_pMay->Get_IsInGravityPipe() ? 
			XMVectorSet(0.f, 1.f, 0.f, 0.f) : vPlayerUp, vPlayerPos));

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
	//OffSetPhsX(matAffine, vPlayerPos, dTimeDelta, &vResultPos);

	m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vResultPos, vPlayerPos, vPlayerUp/*matAffine.r[2]*/));


	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	return _int();
}

_int CSubCamera::Tick_Cam_Free_RideSpaceShip_May(_double dTimeDelta)
{
	CTransform* pPlayerTransform = m_pMay->Get_Transform();
	//행성 중심에서 카메라까지가 up?
	CMoon* pMoon = static_cast<CMoon*>(DATABASE->Get_Mooon());
	if (nullptr == pMoon)
		return EVENT_ERROR;
	
	_vector vMoonPos = pMoon->Get_Position();
	_vector vAt = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	
	//_float fRadius = XMVectorGetX(XMVector3Length(vEye - vMoonPos));
	//_vector vLook = vAt - vEye;
	_vector vDirMoonWithAt = XMVector3Normalize(vAt - vMoonPos);
	/*
	_vector vQuatPlayer = XMVectorSetW(vDirMoonWithAt,0.f);
	_vector vAxis = XMVectorZero();
	_float fAngle = 0.f;
	XMQuaternionToAxisAngle(&vAxis, &fAngle, vQuatPlayer);
	_vector vCamRot = XMQuaternionRotationAxis(vAxis, fAngle - XMConvertToRadians(-40.f));
	_vector vRot = XMQuaternionSlerp(vQuatPlayer, vCamRot, 0.5f);
	_matrix matRot = XMMatrixRotationQuaternion(vRot);
	matRot.r[3] = vAt;
	m_pTransformCom->Set_WorldMatrix(matRot);*/

	_matrix matBegin = XMLoadFloat4x4(&m_matBeginWorld);
	_matrix matPlayer = pPlayerTransform->Get_WorldMatrix();
	_vector vEye = vAt - matPlayer.r[2] * 10.f + matPlayer.r[1] * 9.f;
	_vector vDirMoonWithEye = XMVector3Normalize(vEye - vMoonPos);
	_vector vLook = XMVector3Normalize(vAt - vEye);
	
	_vector vRight = XMVector3Normalize(XMVector3Cross(vDirMoonWithEye, vLook));
	
	_float fAngle = acosf(XMVectorGetX(XMVector3Dot(vDirMoonWithEye,vDirMoonWithAt)));
	
	

	_vector vAxis = XMQuaternionNormalize(XMQuaternionRotationAxis(XMVector3Normalize(vRight/*matPlayer.r[0]*/),-fAngle/* +  XMConvertToRadians(45.f)*/));
	//_vector vAxis = XMQuaternionNormalize(XMQuaternionRotationMatrix(matCamRot));
	_vector vAxisConj = XMQuaternionNormalize(XMQuaternionConjugate(vAxis));
	_vector vOrigin = XMQuaternionNormalize(XMQuaternionRotationMatrix(matBegin *matPlayer));
	vOrigin = XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(vAxis, vOrigin),vAxisConj));
	
	_matrix matRot = XMMatrixRotationQuaternion(vOrigin);
	matRot.r[3] = vEye;//pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_WorldMatrix(matRot);
	


	return NO_EVENT;
}

_int CSubCamera::Tick_CamHelperNone(_double dTimeDelta)
{
	
	_int iResult = NO_EVENT;
	switch (m_eCurCamMode)
	{
	case Client::CSubCamera::CamMode::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CSubCamera::CamMode::Cam_AutoToFree:
		iResult = Tick_Cam_AutoToFree(dTimeDelta);
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

_int CSubCamera::ReSet_Cam_FreeToAuto()
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

_bool CSubCamera::OffSetPhsX(_fmatrix matWorld, _fvector vAt, _double dTimeDelta, _vector * pOut)
{
	_bool isHit = false;

	_vector vDistanceFromCam = matWorld.r[3] - vAt;
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
