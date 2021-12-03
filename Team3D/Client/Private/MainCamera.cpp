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

	//CameraDesc.vEye = /*_float3(0.f, 8.f, -7.f);*/_float3(0.f, 8.f, -11.f);
	//CameraDesc.vAt = /*_float3(0.f, 0.f, 0.f);*/_float3(0.f, 4.5f, 0.f);

	CDataStorage::GetInstance()->Set_MainCamPtr(this);

	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_SMALL], MakeViewMatrix(_float3(0.f, 3.f, -4.f), _float3(0.f, 1.f, 0.f), _float3(0.f, 1.f, 0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM], MakeViewMatrix(_float3(0.f, 8.f, -7.f), _float3(0.f, 2.f, 0.f),_float3(0.f,1.f,0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_LARGE], MakeViewMatrix(_float3(0.f, 7.f, -12.f), _float3(0.f, 3.f, 0.f),_float3(0.f,1.f,0.f)));


	m_matBeginWorld = m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM];
	XMStoreFloat4x4(&m_matBeforeSpringCam, m_pTransformCom->Get_WorldMatrix());
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	

	m_PreWorld.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	m_PreWorld.vRotQuat = _float4(0.f, 0.f, 0.f, 1.f);
	m_PreWorld.vTrans = _float4(0.f, 0.f, 0.f, 1.f);



	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (false == m_bStart)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetCody();
		if (m_pTargetObj)
		{
			XMStoreFloat3(&m_vPlayerPos, static_cast<CCody*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
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
	

	switch ( m_pCamHelper->Tick(dTimeDelta,CFilm::LScreen))
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

void CMainCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return;
	CCody* pTargetPlayer = static_cast<CCody*>(m_pTargetObj);

	m_eCurPlayerSize = pTargetPlayer->Get_Player_Size();

	if (m_eCurPlayerSize != m_ePrePlayerSize)
	{
		m_ePrePlayerSize = m_eCurPlayerSize;
		_matrix matBegin = XMMatrixIdentity();
		switch (m_eCurPlayerSize)
		{
		case Client::CCody::SIZE_SMALL:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_SMALL]);
			break;
		case Client::CCody::SIZE_MEDIUM:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM]);
			break;
		case Client::CCody::SIZE_LARGE:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_LARGE]);
			break;
		}
		XMStoreFloat4x4(&m_matBeginWorld, matBegin);
		m_fChangeCamModeTime = 0.f;
		m_fChangeCamModeLerpSpeed = 8.f;
	}

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
		m_eCurCamMode = CamMode::Cam_Free;

	
	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();
	_vector vPlayerPos =XMVectorSetW(XMLoadFloat3(&m_vPlayerPos),1.f);
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld); //목표 매트릭스

	_matrix matRev = XMMatrixRotationQuaternion(XMLoadFloat4(&m_PreWorld.vRotQuat)) * 
		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP),vPlayerPos);
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		XMStoreFloat3(&m_vPlayerPos, vPlayerPos);
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //현재 매트릭스
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	_matrix matWorld = XMLoadFloat4x4(&m_matCurWorld);
	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);
	
	matNext *= matRev;

	_vector	  vPreRight		= matWorld.r[0],vNextRight	= matNext.r[0]
			, vPreUp		= matWorld.r[1],vNextUp		= matNext.r[1]
			, vPreLook		= matWorld.r[2],vNextLook	= matNext.r[2]
			, vPrePos		= matWorld.r[3],vNextPos	= matNext.r[3];

	_vector vCurRight = XMVectorLerp(vPreRight, vNextRight, m_fChangeCamModeTime),
			vCurUp	  = XMVectorLerp(vPreUp, vNextUp, m_fChangeCamModeTime),
			vCurLook  = XMVectorLerp(vPreLook, vNextLook, m_fChangeCamModeTime),
			vCurPos   = XMVectorLerp(vPrePos, vNextPos, m_fChangeCamModeTime);
	
	_matrix matCurWorld = XMMatrixIdentity();
	matCurWorld.r[0] = vCurRight;
	matCurWorld.r[1] = vCurUp;
	matCurWorld.r[2] = vCurLook;
	matCurWorld.r[3] = vCurPos;


	m_pTransformCom->Set_WorldMatrix(matCurWorld);
	
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{

	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	m_pActorCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION)); 
	if (true == m_bIsCollision)
		matWorld = XMLoadFloat4x4(&m_matBeforeSpringCam);


	//m_pTransformCom->Set_WorldMatrix(matWorld);
	_long MouseMove = 0;
	//이전 회전값
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
		{
			m_fMouseRev[Rev_Holizontal] = 0.f;
			m_fCurMouseRev[Rev_Holizontal] = 0.f;
		}
	}
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
		m_fMouseRev[Rev_Prependicul] += (_float)(MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta);
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		else if (m_fMouseRev[Rev_Prependicul] > 60.f)
			m_fMouseRev[Rev_Prependicul] = 60.f;
		else if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 20.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 20.f;


	//카메라 회전에 따른 거리체크
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matBeginWorld));
	_vector vLook	= XMLoadFloat4x4(&m_matBeginWorld).r[2];
	_vector vPos	= XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[m_eCurPlayerSize]).r[3];
	_vector vDir = XMVectorZero();
	switch (m_eCurPlayerSize)
	{
	case CCody::SIZE_SMALL:
		vDir =  (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.1f);
		break;
	case CCody::SIZE_MEDIUM:
		vDir =  (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.12f);
		break;
	case CCody::SIZE_LARGE:
		vDir =  (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.2f);
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - vDir);
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	//CamEffect

	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld))) //카메라의 원점 
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
		XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	}

	//카메라 움직임이 끝나고 체크할것들
	//SoftMoving
	_vector vPrePlayerPos = XMVectorSetW(XMLoadFloat3(&m_vPlayerPos),1.f);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//카메라와 플레이어의 실제 거리
	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos));
	_bool bIsTeleport = true;
	if (fDist < 10.f&&  fDist > 0.01f) //순간이동안했을때
	{
		vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos, 
			XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta);
		bIsTeleport = false;
	}

	XMStoreFloat3(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)

	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(
		m_fCurMouseRev[Rev_Prependicul]), XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);
	//Sehoon

	_matrix matQuat = XMMatrixRotationQuaternion(vCurQuartRot);
	
	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat = XMVectorZero(), vCurTrans = XMVectorZero();
	XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMLoadFloat4x4(&m_matBeginWorld) * matQuat *
		MH_RotationMatrixByUp(pPlayerTransform->Get_State(CTransform::STATE_UP), vPlayerPos)); //계산 완료한 이번 프레임의 월드


	
	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat); //이전프레임의 월드
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);

	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f/*XMVectorGetX(XMVector4Length(vPreQuat - vRotQuat) * dTimeDelta * 20.f)*/);
	vCurTrans = XMVectorLerp(vPreTrans, vTrans,0.5f /*XMVectorGetX(XMVector4Length(vPreTrans - vTrans) * dTimeDelta * 20.f)*/);


	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);

	

	m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSet(1.f,1.f,1.f,0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vCurRotQuat, vCurTrans));


	XMStoreFloat4x4(&m_matBeforeSpringCam, m_pTransformCom->Get_WorldMatrix());
#pragma region PhsyX Check
	_vector vResultPos = XMVectorZero();
	if (false == bIsTeleport)
	{
		if (m_bIsCollision = OffSetPhsX(dTimeDelta, &vResultPos)) //SpringCamera
		{
			_float3 vEye, vAt;
			XMStoreFloat3(&vEye, vResultPos);
			XMStoreFloat3(&vAt,vPlayerPos);
			_matrix matCurWorld = MakeViewMatrix(vEye, vAt,_float3(0.f, 1.f, 0.f));
			m_pTransformCom->Set_WorldMatrix(matCurWorld);
		}
	}
	
#pragma endregion


	return NO_EVENT;
}

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

_int CMainCamera::ReSet_Cam_FreeToAuto()
{
	m_fChangeCamModeLerpSpeed = 6.f;
	m_fChangeCamModeTime = 0.f;
	for (_uint i = 0; i < Rev_End; i++)
	{
		m_fMouseRev[i] = 0.f;
		m_fCurMouseRev[i] = 0.f;
	}
	
	return NO_EVENT;
}

_bool CMainCamera::OffSetPhsX(_double dTimeDelta,_vector * pOut)
{
	
	if (nullptr == m_pActorCom)
		return false;
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_vector vPos = matWorld.r[3];
	_vector vActorPos = m_pActorCom->Get_Position();
	_vector vPlayerPos = XMVectorSetW(XMVectorSetY(XMLoadFloat3(&m_vPlayerPos),m_vPlayerPos.y + 2.f),1.f) ;

	_vector vDir = vPos - vActorPos;
	if (m_pActorCom->Move(vDir, XMVectorGetX(XMVector4Length(vDir)))) //이전프레임에서 현재 프레임의 포스로 이동
	{

		if (m_pActorCom->Get_Collision()) //플레이어와의 충돌은 무시
		{
			vDir = vPos - vPlayerPos;
			vPlayerPos += vDir * 0.5f;
			m_pActorCom->Set_Position(vPlayerPos);
			m_pActorCom->Move(vDir, 0.f /*XMVectorGetX(XMVector4Length(vDir))*/);
			*pOut = m_pActorCom->Get_Position();
			return true;
		}
		else return false;

			//_vector vRayDir =  vPos - vPlayerPos ;
			//_float fPlayerRadius = 0.5f;
			//switch (m_eCurPlayerSize)
			//{
			//case Client::CCody::SIZE_SMALL: fPlayerRadius = 0.05f;
			//	break;
			//case Client::CCody::SIZE_MEDIUM: fPlayerRadius = 0.5f;
			//	break;
			//case Client::CCody::SIZE_LARGE: fPlayerRadius = 2.f;
			//	break;
			//}
			//vPlayerPos += XMVector4Normalize(vRayDir) * fPlayerRadius * 1.1f;
			//vRayDir = vPos - vPlayerPos;
			//_float fDist = XMVectorGetX(XMVector4Length(vRayDir));
			//PxRaycastBuffer RaycastBuffer;
		
			//m_pGameInstance->Raycast(MH_PxVec3(vPlayerPos),
			//	MH_PxVec3(XMVector4Normalize(vRayDir)), 1000.f, RaycastBuffer, PxHitFlag::eDEFAULT);
			///*vPos += XMVector4Normalize(vRayDir);
			//*/	
			//if (RaycastBuffer.hasBlock)
			//{

			//	*pOut = MakeBlockPos(RaycastBuffer);
			//	return true;
			//}
			//else return false;

	
	}

	return false;
}

_fmatrix CMainCamera::MakeViewMatrix(_float3 Eye, _float3 At,_float3 vAxisY)
{
	_vector	vEye = XMVectorSetW(XMLoadFloat3(&Eye), 1.f);
	_vector	vAt = XMVectorSetW(XMLoadFloat3(&At), 1.f);
	_vector	vLook = XMVector3Normalize(vAt - vEye);
	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vAxisY), vLook));
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));


	_matrix matWorld = XMMatrixIdentity();
	matWorld.r[0] = vRight;
	matWorld.r[1] = vUp;
	matWorld.r[2] = vLook;
	matWorld.r[3] = vEye;

	return matWorld;
}

_fvector CMainCamera::MakeBlockPos(PxRaycastBuffer tBuffer)
{
	return XMVectorSet(tBuffer.block.position.x, tBuffer.block.position.y, tBuffer.block.position.z, 1.f);
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
		_matrix matBegin = XMMatrixIdentity();
		switch (m_eCurPlayerSize)
		{
		case CCody::SIZE_SMALL:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_SMALL]);
			break;
		case CCody::SIZE_MEDIUM:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM]);
			break;
		case CCody::SIZE_LARGE:
			matBegin = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_LARGE]);
			break;
		}
		XMStoreFloat4x4(&m_matBeginWorld, matBegin);
		m_fChangeCamModeTime = 0.f;
		m_fChangeCamModeLerpSpeed = 10.f;
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	}
#endif
	//ChangeViewPort();
	m_fChangeCamModeTime <= 1.f ? m_eCurCamMode = CamMode::Cam_AutoToFree : m_eCurCamMode = CamMode::Cam_Free;

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
