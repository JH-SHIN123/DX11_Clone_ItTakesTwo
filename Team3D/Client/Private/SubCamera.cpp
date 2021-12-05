#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"
#include "PlayerActor.h"
#include "May.h"
#include "CameraActor.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);

	CDataStorage::GetInstance()->Set_SubCamPtr(this);

	
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	
	m_PreWorld.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	m_PreWorld.vRotQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_PreWorld.vTrans = _float4(0.f, 0.f, 0.f, 1.f);

	m_vStartEye =	{0.f,7.f, -7.f, 1.f};
	m_vStartAt =	{0.f,1.5f, 0.f, 1.f };

	_matrix matStart = MakeViewMatrixByUp(m_vStartEye, m_vStartAt,XMVectorSet(0.f,1.f,0.f,0.f));
	XMStoreFloat4x4(&m_matBeforeSpringCam, matStart);

	m_matBeginWorld = m_matStart = m_matBeforeSpringCam;
	return S_OK;
}

_int CSubCamera::Tick(_double dTimeDelta)
{
	if (false == m_bStart)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetMay();
		if (m_pTargetObj)
		{
			CTransform* pPlayerTransform = static_cast<CMay*>(m_pTargetObj)->Get_Transform();
			XMStoreFloat4(&m_vPlayerPos, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vPlayerUp, pPlayerTransform->Get_State(CTransform::STATE_UP));
			m_pActorCom->Set_Scale(0.4f, 0.001f);
			Safe_AddRef(m_pTargetObj);
		}
		else
			return EVENT_ERROR;
		m_bStart = true;
	}


	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;

	//return CCamera::Tick(dTimeDelta);
	Check_Player(dTimeDelta);



	_int iResult = NO_EVENT;


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



void CSubCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return;
	CMay* pTargetPlayer = static_cast<CMay*>(m_pTargetObj);
	if (m_eCurCamMode == Cam_Free)
	{
		switch (m_eCurCamFreeOption)
		{
		case Client::CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
			pTargetPlayer->Get_Actor()->Get_IsOnGravityPath() ? Set_Zoom(2.f, dTimeDelta) : Set_Zoom(0.f, dTimeDelta);
			break;
		case Client::CSubCamera::CamFreeOption::Cam_Free_FreeMove:
			break;
		default:
			break;
		}
	
	}
}

void CSubCamera::Set_Zoom(_float fZoomVal, _double dTimeDelta)
{
	m_fCamZoomVal += (fZoomVal - m_fCamZoomVal) * (_float)dTimeDelta * 5.f;

	_matrix matStart = MakeViewMatrixByUp(m_vStartEye, m_vStartAt,XMVectorSet(0.f,1.f,0.f,0.f));

	matStart.r[3] += matStart.r[2] * m_fCamZoomVal;
	XMStoreFloat4x4(&m_matStart,matStart);
}



HRESULT CSubCamera::Start_Film(const _tchar * pFilmTag)
{
	m_pCamHelper->Start_Film(pFilmTag, CFilm::RScreen);
	return S_OK;
}

_int CSubCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;

	switch (m_eCurCamFreeOption)
	{
	case CSubCamera::CamFreeOption::Cam_Free_FollowPlayer:
		Tick_Cam_Free_FollowPlayer(dTimeDelta);
		break;
	case CSubCamera::CamFreeOption::Cam_Free_FreeMove:
		Tick_Cam_Free_FreeMode(dTimeDelta);
		break;
	}
	return NO_EVENT;
}


_int CSubCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
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
	CTransform* pPlayerTransform = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform();
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //시작 매트릭스
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
	_matrix matNext = XMLoadFloat4x4(&m_matStart); //목표 매트릭스

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
	CTransform* pPlayerTransform = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	if (true == m_bIsCollision)
		matWorld = XMLoadFloat4x4(&m_matBeforeSpringCam);


	_long MouseMove = 0;

#ifdef __CONTROL_MAY_KEYBOARD
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X)/*m_pGameInstance->Get_Pad_RStickX() - 32767*/)
	{
		//if (abs(MouseMove) < 2000)
		//	MouseMove = 0;
		//else
		//	MouseMove = MouseMove / 800;


		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal];
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
		{
			m_fMouseRev[Rev_Holizontal] = 0.f;
			m_fCurMouseRev[Rev_Holizontal] = 0.f;
		}
	}

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y)/*(65535 - m_pGameInstance->Get_Pad_RStickY()) - 32767*/)
	{
		//if (abs(MouseMove) < 2000)
		//	MouseMove = 0;
		//else
		//	MouseMove = MouseMove / 2000;


		_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);

		if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
			m_fMouseRev[Rev_Prependicul] = 40.f;
		else if (m_fMouseRev[Rev_Prependicul] + fVal < -85.f)
			m_fMouseRev[Rev_Prependicul] = -85.f;
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
			MouseMove = MouseMove / 800;


		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * (_float)dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal];
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
		{
			m_fMouseRev[Rev_Holizontal] = 0.f;
			m_fCurMouseRev[Rev_Holizontal] = 0.f;
		}
	}
	if (MouseMove = (65535 - m_pGameInstance->Get_Pad_RStickY()) - 32767)
	{
		if (abs(MouseMove) < 2000)
			MouseMove = 0;
		else
			MouseMove = MouseMove / 2000;


		_float fVal = (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);

		if (m_fMouseRev[Rev_Prependicul] + fVal > 40.f)
			m_fMouseRev[Rev_Prependicul] = 40.f;
		else if (m_fMouseRev[Rev_Prependicul] + fVal < -85.f)
			m_fMouseRev[Rev_Prependicul] = -85.f;
		else
			m_fMouseRev[Rev_Prependicul] += fVal;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;
#endif


																	
	//카메라 원점상태(공전없을때)
	_vector vLook = XMLoadFloat4x4(&m_matBeginWorld).r[2];
	_vector vStartPos = XMLoadFloat4x4(&m_matStart).r[3];
	_vector vDir = XMVectorZero();

	//카메라 수직이동에따른 거리조절
	vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.065f);
	memcpy(&m_matBeginWorld._41, &(vStartPos - vDir), sizeof(_float4));

	//CamEffect

	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::RScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::RScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld)))
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::RScreen));
	}

	//카메라 움직임이 끝나고 체크할것들

	//카메라 움직임이 끝나고 체크할것들
	//SoftMoving
	_vector vTargetPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	//_vector vPlayerUp = XMLoadFloat4(&m_vPlayerUp);
	//_vector vUpDir = (vTargetPlayerUp - vPlayerUp);
	//if(XMVectorGetX(XMVector4Length(vUpDir)) > 0.01f)
	//vPlayerUp += vUpDir* dTimeDelta /** 10.f*/;
	//XMStoreFloat4(&m_vPlayerUp, vPlayerUp);

	_vector vPlayerUp = vTargetPlayerUp;
	XMStoreFloat4(&m_vPlayerUp, vPlayerUp);
	
	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	
	_vector vCalculateUp = XMVector3TransformNormal(XMVectorSetW(XMLoadFloat4(&m_vStartAt), 0.f), MH_RotationMatrixByUp(vPlayerUp));
	vCurPlayerPos += vCalculateUp;
	//카메라와 플레이어의 실제 거리
	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));
	_bool bIsTeleport = false;
	if (fDist < 10.f && fDist > 0.01f) //순간이동안했을때
	{
		vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos,
			XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 2.f);
	} else if(fDist > 10.f)
		bIsTeleport = true;

	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)
	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),
		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);
	//Sehoon

	_matrix matQuat = XMMatrixRotationQuaternion(vCurQuartRot);

	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat, vCurTrans;


	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, 
		XMLoadFloat4x4(&m_matBeginWorld) * matQuat *MH_RotationMatrixByUp(static_cast<CMay*>(m_pTargetObj)->Get_IsInGravityPipe() ? 
			XMVectorSet(0.f, 1.f, 0.f, 0.f) : vPlayerUp, vPlayerPos));

	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);


	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f/*XMVectorGetX(XMVector4Length(vPreQuat - vRotQuat) * dTimeDelta * 20.f)*/);
	vCurTrans = XMVectorLerp(vPreTrans, vTrans, 0.5f /*XMVectorGetX(XMVector4Length(vPreTrans - vTrans) * dTimeDelta * 20.f)*/);

	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);


	_matrix matAffine = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		vCurRotQuat, vCurTrans);

	XMStoreFloat4x4(&m_matBeforeSpringCam, matAffine);
#pragma region PhsyX Check
	_vector vResultPos = XMVectorZero();
	if (false == bIsTeleport)
	{
		if (m_bIsCollision = OffSetPhsX(matAffine, dTimeDelta, &vResultPos)) //SpringCamera
		{
			_float4 vEye, vAt;
	
			XMStoreFloat4(&vEye, vResultPos);
			XMStoreFloat4(&vAt, vPlayerPos);
			_matrix matCurWorld = MakeViewMatrixByUp(vEye, vAt,vPlayerUp);
			matAffine = matCurWorld;
		}
	}
	m_pTransformCom->Set_WorldMatrix(matAffine);

#pragma endregion
	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	return _int();
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

_bool CSubCamera::OffSetPhsX(_fmatrix matWorld, _double dTimeDelta, _vector * pOut)
{
	if (nullptr == m_pActorCom)
		return false;

	_vector vPos = matWorld.r[3];
	_vector vDir = XMVectorZero();
	/*_vector vActorPos = m_pActorCom->Get_Position();
	vDir = vPos - vActorPos;
	if (!m_pActorCom->Move(vDir, dTimeDelta))
	return false;*/

	_vector vPlayerPos = XMLoadFloat4(&m_vPlayerPos);

	vDir = vPos - vPlayerPos;

	m_pActorCom->Set_Position(vPlayerPos);
	if (m_pActorCom->Move(vDir, dTimeDelta))
	{
		*pOut = XMVectorSetW(m_pActorCom->Get_Position()/*Get_Position()*/, 1.f);
		return true;
	}
	return false;
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
	Safe_Release(m_pCamHelper);
	Safe_Release(m_pActorCom);


	CCamera::Free();
}
