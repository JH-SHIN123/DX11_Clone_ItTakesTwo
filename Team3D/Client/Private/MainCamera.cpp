#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "GameInstance.h"
#include "Level.h"
#include "Cody.h"
#include "PhysX.h"
#include "CameraActor.h"

CMainCamera::CMainCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CMainCamera::CMainCamera(const CMainCamera & rhs)
	: CCamera(rhs)
{
	// ddd
}

HRESULT CMainCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMainCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	CControllableActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eCAMERA, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.fJumpGravity = 0.f;

	ArgDesc.CapsuleControllerDesc.setToDefault();
	ArgDesc.CapsuleControllerDesc.height = 0.1f;
	ArgDesc.CapsuleControllerDesc.radius = m_fCamRadius = 0.4f;
	ArgDesc.CapsuleControllerDesc.material = m_pGameInstance->Get_BasePxMaterial();
	ArgDesc.CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	ArgDesc.CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
	ArgDesc.CapsuleControllerDesc.contactOffset = 0.01f;
	ArgDesc.CapsuleControllerDesc.stepOffset = 0.707f;
	ArgDesc.CapsuleControllerDesc.slopeLimit = 0.f;
	ArgDesc.CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	ArgDesc.CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);

	m_pActorCom->Set_Scale(m_fCamRadius, 0.f);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);

	
	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	m_eCurCamMode = CamMode::Cam_AutoToFree;
	

	//CameraDesc.vEye = /*_float3(0.f, 8.f, -7.f);*/_float3(0.f, 8.f, -11.f);
	//CameraDesc.vAt = /*_float3(0.f, 0.f, 0.f);*/_float3(0.f, 4.5f, 0.f);

	CDataStorage::GetInstance()->Set_MainCamPtr(this);

	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_SMALL], MakeViewMatrix(_float3(0.f, 3.f, -4.f), _float3(0.f, 1.f, 0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM], MakeViewMatrix(_float3(0.f, 8.f, -7.f), _float3(0.f, 2.f, 0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_LARGE], MakeViewMatrix(_float3(0.f, 7.f, -12.f), _float3(0.f, 3.f, 0.f)));


	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{

	if (m_pTargetObj == nullptr)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetCody();
		if (m_pTargetObj)
			Safe_AddRef(m_pTargetObj);
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
	CCody* pTargetPlayer = dynamic_cast<CCody*>(m_pTargetObj);

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
		m_fChangeCamModeLerpSpeed = 10.f;
	}
	
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
	
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld); //목표 매트릭스
	_matrix matRev = XMLoadFloat4x4(&m_matPreRev);
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		_vector vPlayerPos = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
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
	if (m_fChangeCamModeTime >= 1.f)
	{
		m_eCurCamMode = CamMode::Cam_Free;
	}

	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{
	_matrix matRev = XMMatrixIdentity();
	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();


	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//마우스 체크
	_long MouseMove = 0;

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_fMouseRev[Rev_Holizontal] += (_float)(MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal]);
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
			m_fMouseRev[Rev_Holizontal] = 0.f;
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


	//카메라 회전에 따른 거리체크
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[m_eCurPlayerSize]).r[3];
	switch (m_eCurPlayerSize)
	{
	case CCody::SIZE_SMALL:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.1f));
		break;
	case CCody::SIZE_MEDIUM:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.12f));
		break;
	case CCody::SIZE_LARGE:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.2f));
		break;
	}
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	//CamEffect

	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld)))
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
		//XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	}

	//카메라 움직임이 끝나고 체크할것들

	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	matRev = matRevX * matRevY* matTrans;


#pragma region PhsyX Check
	//_vector vResultPos = XMVectorZero();
	//m_bPhsXCollision = OffSetPhsX(dTimeDelta, matRev , &vResultPos); //로컬에서 공전후에 충돌검사함
	//if (true == m_bPhsXCollision)
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPos);
	//	//m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vResultPos), XMVectorGetY(vResultPos), XMVectorGetZ(vResultPos)));
	//}
	//else
	//	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matBeginWorld));
#pragma endregion
	//Key_Check(dTimeDelta);

	//CamEffect


	XMStoreFloat4x4(&m_matPreRev, matRev);

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, m_pTransformCom->Get_WorldMatrix()))
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
		//XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	}
	KeyCheck(dTimeDelta);
	return NO_EVENT;
}



void CMainCamera::ChangeViewPort()
{
	CFilm::CamNode tDesc;
	tDesc.fTargetViewPortCenterX = 0.5f;
	tDesc.fTargetViewPortCenterY = 1.f;
	tDesc.fViewPortLerpSpeed = 1.f;
	////case ViewPortOption::LScreen_Split_Immediate:
	//if (m_pGameInstance->Key_Down(DIK_1))
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, tDesc.fTargetViewPortCenterX, tDesc.fTargetViewPortCenterY),
	//		XMVectorSet(tDesc.fTargetViewPortCenterX, 0.f, tDesc.fTargetViewPortCenterX, 1.f));

	//////case ViewPortOption::LScreen_Split_Lerp:
	//if (m_pGameInstance->Key_Down(DIK_2))
	//{
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
	//		XMVectorSet(0.f, 0.f, 1.f, 1.f));
	//	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, tDesc.fTargetViewPortCenterX, tDesc.fTargetViewPortCenterY),
	//		XMVectorSet(tDesc.fTargetViewPortCenterX, 0.f, tDesc.fTargetViewPortCenterX, 1.f), tDesc.fViewPortLerpSpeed);
	//}

	//////case ViewPortOption::LScreen_Merge_Immediate:
	//if (m_pGameInstance->Key_Down(DIK_3))
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
	//		XMVectorSet(0.f, 0.f, 1.f, 1.f));

	//////case ViewPortOption::LScreen_Merge_Lerp:
	//if (m_pGameInstance->Key_Down(DIK_4))
	//{

	//	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
	//		XMVectorSet(0.f, 0.f, 1.f, 1.f), tDesc.fViewPortLerpSpeed);
	//}

	//////case ViewPortOption::RScreen_Split_Immediate:
	//if (m_pGameInstance->Key_Down(DIK_5))
	//{
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, tDesc.fTargetViewPortCenterX, tDesc.fTargetViewPortCenterY),
	//		XMVectorSet(tDesc.fTargetViewPortCenterX, 0.f, tDesc.fTargetViewPortCenterX, 1.f));
	//}

	//////case ViewPortOption::RScreen_Split_Lerp:
	//if (m_pGameInstance->Key_Down(DIK_6))
	//{
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
	//		XMVectorSet(1.f, 0.f, 1.f, 1.f));
	//	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f),
	//		XMVectorSet(0.5, 0.f, 0.5f, 1.f), tDesc.fViewPortLerpSpeed);
	//}

	//////case ViewPortOption::RScreen_Merge_Immediate:
	//if (m_pGameInstance->Key_Down(DIK_7))
	//	m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
	//		XMVectorSet(1.f, 0.f, 1.f, 1.f));

	//////case ViewPortOption::RScreen_Merge_Lerp:
	//if (m_pGameInstance->Key_Down(DIK_8))
	//{
	//	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
	//			XMVectorSet(1.f, 0.f, 1.f, 1.f), tDesc.fViewPortLerpSpeed);

	//}
}

void CMainCamera::KeyCheck(_double dTimeDelta)
{
	//if (m_pGameInstance->Key_Pressing(DIK_W))
	//{
	//	m_pTransformCom->Go_Straight(dTimeDelta);
	//}
	//if (m_pGameInstance->Key_Pressing(DIK_A))
	//{
	//	m_pTransformCom->Go_Left(dTimeDelta);

	//}
	//if (m_pGameInstance->Key_Pressing(DIK_S))
	//{
	//	m_pTransformCom->Go_Backward(dTimeDelta);

	//}
	//if (m_pGameInstance->Key_Pressing(DIK_D))
	//{
	//	m_pTransformCom->Go_Right(dTimeDelta);

	//}
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

	XMStoreFloat4x4(&m_matPreRev,XMMatrixIdentity());
	m_fChangeCamModeLerpSpeed = 6.f;
	m_fChangeCamModeTime = 0.f;
	m_fMouseRev[Rev_Holizontal] = 0.f;
	m_fMouseRev[Rev_Prependicul] = 0.f;

	return NO_EVENT;
}

_bool CMainCamera::OffSetPhsX(_double dTimeDelta, _fmatrix matRev,_vector * pOut)
{
	//카메라 공전 전의 월드
	_matrix matWorld =	XMLoadFloat4x4(&m_matBeginWorld);
	matWorld *= matRev; //현재 월드
	_vector vPos = matWorld.r[3];

	PxMat44 matPhsX = PxMat44(m_pActorCom->Get_Actor()->getGlobalPose());
	_vector vPhsXPos = XMVectorSet(matPhsX.column3.x, matPhsX.column3.y, matPhsX.column3.z, 1.f);
	//공전 시킨후 카메라쪽으로 피직스 움직임.

	_vector vDir = vPos - vPhsXPos;
	
	PxControllerCollisionFlags eCollisionFlag = m_pActorCom->Get_Controller()->move(MH_PxVec3(vDir), 0.f, PxF32(dTimeDelta), PxControllerFilters());
	if (eCollisionFlag & PxControllerCollisionFlag::eCOLLISION_DOWN ||
		eCollisionFlag & PxControllerCollisionFlag::eCOLLISION_UP||
		eCollisionFlag & PxControllerCollisionFlag::eCOLLISION_SIDES) // MainPhsX -> Cam if Collision
	{
		_vector vPlayerPos = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		//m_pSubActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPlayerPos), XMVectorGetX(vPlayerPos), XMVectorGetX(vPlayerPos), ))

		//if (nullptr == m_pTargetObj)
		//	return false;
		//_vector vPlayerPos = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		//switch (m_eCurPlayerSize)
		//{
		//case Client::CCody::SIZE_SMALL: vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + 1.f);
		//	break;
		//case Client::CCody::SIZE_MEDIUM: vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + 2.f);
		//	break;
		//case Client::CCody::SIZE_LARGE: vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + 3.f);
		//	break;
		//}
		//PxMat44 matPhsX = PxMat44(m_pActorCom->Get_Actor()->getGlobalPose());
		//_vector vPhsXPos = XMVectorSet(matPhsX.column3.x, matPhsX.column3.y, matPhsX.column3.z, 1.f);
		//PxRaycastBuffer tBuffer;
		//if(CPhysX::Raycast())
		//_vector vPhsXResult
		//	= XMVectorSet(tBuffer.block.position.x, tBuffer.block.position.y, tBuffer.block.position.z, 1.f) + m_fCamRadius * XMVector4Normalize(vPlayerPos - vPhsXPos); /*+XMVector4Normalize(vPhsXPos - vPlayerPos) * m_fCamRadius*/;
		//
		while (m_pActorCom->Get_Controller()->move(MH_PxVec3(matWorld.r[2]), 0.f, 0.f, PxControllerFilters()));

		//m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vPhsXResult), XMVectorGetY(vPhsXResult), XMVectorGetZ(vPhsXResult)));
		_vector vResultPos = XMVectorSet(matPhsX.column3.x, matPhsX.column3.y, matPhsX.column3.z, 1.f);

		*pOut = XMVector3TransformCoord(vResultPos, XMMatrixInverse(nullptr, matRev));
	
		return true;
	}
	return false;
	
}

_fmatrix CMainCamera::MakeViewMatrix(_float3 Eye, _float3 At)
{
	_vector	vEye = XMVectorSetW(XMLoadFloat3(&Eye), 1.f);
	_vector	vAt = XMVectorSetW(XMLoadFloat3(&At), 1.f);
	_vector	vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector	vLook = XMVector3Normalize(vAt - vEye);
	_vector	vRight = XMVector3Normalize(XMVector3Cross(vAxisY, vLook));
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));


	_matrix matWorld = XMMatrixIdentity();
	matWorld.r[0] = vRight;
	matWorld.r[1] = vUp;
	matWorld.r[2] = vLook;
	matWorld.r[3] = vEye;

	return matWorld;
}





#pragma region Cam_Helper
_int CMainCamera::Tick_CamHelperNone(_double dTimeDelta)
{
	if (m_pTargetObj == nullptr)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetCody();
		if (m_pTargetObj)
			Safe_AddRef(m_pTargetObj);
	}
	//외부에서 상태 설정 구간
#ifdef _DEBUG

	if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	{
		m_pCamHelper->Start_Film(L"Eye_Bezier3", CFilm::LScreen);
		return NO_EVENT;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_pCamHelper->Start_Film(L"Eye_Bezier4", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		m_pCamHelper->Start_Film(L"Eye_Straight", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Loc_Right", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Loc_Up", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Loc_Look", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Rot_Right", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Rot_Up", CFilm::LScreen);
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_Rot_Look", CFilm::LScreen);
		return NO_EVENT;
	}
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
	ChangeViewPort();
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
	m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Tick_Film(dTimeDelta, CFilm::LScreen));
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
