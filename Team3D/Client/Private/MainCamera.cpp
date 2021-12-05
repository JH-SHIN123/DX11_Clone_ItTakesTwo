#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "Cody.h"
#include "CameraActor.h"
#include "PlayerActor.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom,&ArgDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);


	CDataStorage::GetInstance()->Set_MainCamPtr(this);

	m_eCurCamMode = CamMode::Cam_AutoToFree;
	

	m_PreWorld.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	m_PreWorld.vRotQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_PreWorld.vTrans = _float4(0.f, 0.f, 0.f, 1.f);
	
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_SMALL] =	{ 0.f,1.5f,-1.5f,1.f};
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM] =	{ 0.f,7.f,-7.f,1.f };
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_LARGE] =	{ 0.f,11.f,	-8.f,1.f };

	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_SMALL] =		{ 0.f,0.1f,0.02f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM] =	{ 0.f,1.5f, 0.4f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_LARGE] =		{ 0.f,3.f,1.1f,1.f };

	_matrix matStart = MakeViewMatrixByUp(m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM], m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM]);
	XMStoreFloat4x4(&m_matBeforeSpringCam, matStart);
	
	m_matBeginWorld = m_matStart = m_matBeforeSpringCam;
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

	//return CCamera::Tick(dTimeDelta);
	//Check

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
		switch (m_eCurPlayerSize)
		{
		case Client::CCody::SIZE_SMALL:
			m_pActorCom->Set_Scale(0.02f,0.001f);
			break;
		case Client::CCody::SIZE_MEDIUM:
			m_pActorCom->Set_Scale(0.4f,0.001f);
			break;
		case Client::CCody::SIZE_LARGE:
			m_pActorCom->Set_Scale(0.4f ,0.001f);
			break;
		}
	}
	m_bIsLerpToCurSize = LerpToCurSize(m_eCurPlayerSize, dTimeDelta);
	

}

void CMainCamera::Set_Zoom(_float fZoomVal, _double dTimeDelta)
{
	m_fCamZoomVal += (fZoomVal - m_fCamZoomVal) * (_float)dTimeDelta;
	//현재크기에서 줌인아웃
	_matrix matStart = MakeViewMatrixByUp(m_vSizeEye[m_eCurPlayerSize], m_vSizeAt[m_eCurPlayerSize],XMVectorSet(0.f,1.f,0.f,0.f));
	matStart.r[3] += matStart.r[2] * m_fCamZoomVal;
	XMStoreFloat4x4(&m_matStart, matStart);
}

_bool CMainCamera::LerpToCurSize(CCody::PLAYER_SIZE eSize,_double dTimeDelta)
{
	if (m_dLerpToCurSizeTime > 1.f)
		return false;
	_matrix CurStartMatrix = XMLoadFloat4x4(&m_matStart);
	_matrix TargetMatrix = MakeViewMatrixByUp(m_vSizeEye[eSize], m_vSizeAt[eSize]);

	m_dLerpToCurSizeTime += dTimeDelta * 5.f;

	XMStoreFloat4x4(&m_matStart, MakeLerpMatrix(CurStartMatrix, TargetMatrix, (_float)m_dLerpToCurSizeTime));
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


	switch (m_eCurCamFreeOption)
	{
	case CMainCamera::CamFreeOption::Cam_Free_FollowPlayer:
		Tick_Cam_Free_FollowPlayer(dTimeDelta);
		break;
	case CMainCamera::CamFreeOption::Cam_Free_FreeMove:
		Tick_Cam_Free_FreeMode(dTimeDelta);
		break;
	}
	return NO_EVENT;
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

_int CMainCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{


	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	//m_pActorCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION)); 
	if (true == m_bIsCollision)
		matWorld = XMLoadFloat4x4(&m_matBeforeSpringCam);


	//m_pTransformCom->Set_WorldMatrix(matWorld);
	_long MouseMove = 0;
	//이전 회전값
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
		
	}
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
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
	


	//카메라 회전에 따른 거리체크

	_vector vLook = XMLoadFloat4x4(&m_matBeginWorld).r[2];
	_vector vStartPos = XMLoadFloat4x4(&m_matStart).r[3];
	_vector vDir = XMVectorZero();

	//카메라 수직이동에따른 거리조절
	switch (m_eCurPlayerSize)
	{
	case Client::CCody::SIZE_SMALL:
		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.02f);
		break;
	case Client::CCody::SIZE_MEDIUM:
		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.065f);
		break;
	case Client::CCody::SIZE_LARGE:
		vDir = (vLook*(m_fCurMouseRev[Rev_Prependicul]) * 0.08f);
		break;
	}

	memcpy(&m_matBeginWorld._41, &(vStartPos - vDir), sizeof(_float4));


	//CamEffect

	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld))) //카메라의 원점 
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
		XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	}

	//카메라 움직임이 끝나고 체크할것들
	//For.GravityUp



	//SoftMoving
	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vCurPlayerPos += XMVectorSetW(XMLoadFloat4(&m_vSizeAt[m_eCurPlayerSize]),0.f);
	//카메라와 플레이어의 실제 거리
	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));
	_bool bIsTeleport = false;
	if (fDist < 10.f && fDist > 0.01f) //순간이동안했을때
	{
		vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos, 
			XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 10.f);
	}
	else if(fDist > 10.f)
		bIsTeleport = true;
	
	
	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)

	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]) ,
		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal] ) ,0.f);
	//Sehoon

	_matrix matQuat = XMMatrixRotationQuaternion(vCurQuartRot);

	
	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat,vCurTrans;
	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMLoadFloat4x4(&m_matBeginWorld) * matQuat * 
		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos)); //계산 완료한 이번 프레임의 월드



	/*vCurRotQuat = vRotQuat;
	vCurTrans = vTrans;*/
	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);


	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f/*XMVectorGetX(XMVector4Length(vPreQuat - vRotQuat) * dTimeDelta * 20.f)*/);
	vCurTrans = XMVectorLerp(vPreTrans, vTrans,0.5f /*XMVectorGetX(XMVector4Length(vPreTrans - vTrans) * dTimeDelta * 20.f)*/);


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
			_matrix matCurWorld = MakeViewMatrixByUp(vEye, vAt);
			matAffine = matCurWorld;
		}
	}
	m_pTransformCom->Set_WorldMatrix(matAffine);

#pragma endregion

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

}
#pragma endregion
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
		*pOut = XMVectorSetW(m_pActorCom->Get_Position()/*Get_Position()*/,1.f);
		return true;
	}

	return false;
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
#ifdef _DEBUG

	//if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	//{
	//	CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Intro");
	//	//m_pCamHelper->Start_Film(L"Film_Begin_Game", CFilm::LScreen);
	//	return NO_EVENT;
	//}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FreeMove;
	}
	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	}
#endif
	//ChangeViewPort();


	_int iResult = NO_EVENT;
	switch (m_eCurCamMode)
	{
	case Client::CMainCamera::CamMode::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_AutoToFree:
		iResult = Tick_Cam_AutoToFree(dTimeDelta);

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
	Safe_Release(m_pActorCom);


	CCamera::Free();

}
