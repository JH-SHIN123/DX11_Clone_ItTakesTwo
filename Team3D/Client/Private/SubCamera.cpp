#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include"PlayerActor.h"
#include "May.h"
#include"CameraActor.h"
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

	CPlayerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eSUBCAMERA, this);
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


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CameraActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &ArgDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);


	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matQuternionRev, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_matStart, m_pTransformCom->Get_WorldMatrix());

	m_eCurCamMode = CamMode::Cam_Free;


	//CameraDesc.vEye = /*_float3(0.f, 8.f, -7.f);*/_float3(0.f, 8.f, -11.f);
	//CameraDesc.vAt = /*_float3(0.f, 0.f, 0.f);*/_float3(0.f, 4.5f, 0.f);

	CDataStorage::GetInstance()->Set_SubCamPtr(this);

	return S_OK;
}

_int CSubCamera::Tick(_double dTimeDelta)
{
	if (false == m_bStart)
	{
		m_pTargetObj = CDataStorage::GetInstance()->GetMay();
		if (m_pTargetObj)
		{
			XMStoreFloat3(&m_vPlayerPos, static_cast<CMay*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION));
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

void CSubCamera::Check_Player(_double dTimeDelta)
{
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

	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld); //목표 매트릭스
	_matrix matRev = XMLoadFloat4x4(&m_matPreRev);
	if (m_fChangeCamModeTime == 0.f) //처음 들어왓으면 한번만 공전매트릭스 구하고
	{
		_vector vPlayerPos = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //현재 매트릭스
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
	}

	_matrix matWorld = XMLoadFloat4x4(&m_matCurWorld);
	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

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
		m_eCurCamMode = CamMode::Cam_Free;
	}

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{
	_matrix matRev = XMMatrixIdentity();
	CTransform* pPlayerTransform = dynamic_cast<CMay*>(m_pTargetObj)->Get_Transform();


	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);


	//마우스 체크
	_long MouseMove = 0;

	if (/*MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X)*/m_pGameInstance->Get_Pad_RStickX() - 32767)
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


		m_fMouseRev[Rev_Prependicul] += (_float)MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * (_float)dTimeDelta;
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		if (m_fMouseRev[Rev_Prependicul] > 30.f)
			m_fMouseRev[Rev_Prependicul] = 30.f;
		if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 14.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 14.f;
#else
	if (MouseMove = m_pGameInstance->Get_Pad_RStickX() - 32767)
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
	if (MouseMove = (65535 - m_pGameInstance->Get_Pad_RStickY()) - 32767)
	{
		//if (abs(MouseMove) < 2000)
		//	MouseMove = 0;
		//else
		//	MouseMove = MouseMove / 2000;


		m_fMouseRev[Rev_Prependicul] += (_float)MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * (_float)dTimeDelta;
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		if (m_fMouseRev[Rev_Prependicul] > 30.f)
			m_fMouseRev[Rev_Prependicul] = 30.f;
		if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
	m_fCurMouseRev[Rev_Holizontal] += (m_fMouseRev[Rev_Holizontal] - m_fCurMouseRev[Rev_Holizontal]) * (_float)dTimeDelta * 14.f;
	m_fCurMouseRev[Rev_Prependicul] += (m_fMouseRev[Rev_Prependicul] - m_fCurMouseRev[Rev_Prependicul]) * (_float)dTimeDelta * 14.f;
#endif
	//카메라 회전에 따른 거리체크
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = XMLoadFloat4x4(&m_matStart).r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.12f));
	
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	//CamEffect

	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::RScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::RScreen, dTimeDelta, XMLoadFloat4x4(&m_matBeginWorld)))
			m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::RScreen));
		//XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());
	}

	//카메라 움직임이 끝나고 체크할것들

	//SoftMoving
	_vector vPrePlayerPos = XMVectorSetW(XMLoadFloat3(&m_vPlayerPos), 1.f);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//카메라와 플레이어의 실제 거리
	_vector vPlayerPos = vCurPlayerPos;
	if (XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)) < 10.f) //순간이동안했을때
		vPlayerPos = XMVectorLerp(vPrePlayerPos, vCurPlayerPos, XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 2.f);
	XMStoreFloat3(&m_vPlayerPos, vPlayerPos);

	//회전 보간
	_vector vPreQuarternionRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]), XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);
	_vector vCurQuarternionRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fMouseRev[Rev_Prependicul]), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]), 0.f);

	_matrix QuarRev = XMMatrixRotationQuaternion(
		XMQuaternionSlerp(vPreQuarternionRot, vCurQuarternionRot,
			(_float)dTimeDelta * XMVectorGetX(XMVector4Length(vPreQuarternionRot - vCurQuarternionRot))));


	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));


	matRev = QuarRev * matTrans;



#pragma region PhsyX Check
	//_vector vResultPos = XMVectorZero();
	//if (OffSetPhsX(dTimeDelta, matRev, &vResultPos))
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPos);
	//	//m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vResultPos), XMVectorGetY(vResultPos), XMVectorGetZ(vResultPos)));
	//}
#pragma endregion
	//Key_Check(dTimeDelta);
	XMStoreFloat4x4(&m_matPreRev, matRev);

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

	return NO_EVENT;
}

_int CSubCamera::Tick_Cam_Free_FreeMode(_double dTimeDelta)
{
	return _int();
}

_int CSubCamera::Tick_CamHelperNone(_double dTimeDelta)
{
	//외부에서 상태 설정 구간
#ifdef _DEBUG

	//if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	//{
	//	CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Intro");
	//	//m_pCamHelper->Start_Film(L"Film_Begin_Game", CFilm::RScreen);
	//	return NO_EVENT;
	//}

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FreeMove;
	}
	if (m_pGameInstance->Key_Down(DIK_P))
	{
		_matrix matBegin = XMLoadFloat4x4(&m_matStart);
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
	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());
	m_fChangeCamModeLerpSpeed = 6.f;
	m_fChangeCamModeTime = 0.f;
	for (_uint i = 0; i < Rev_End; i++)
	{
		m_fMouseRev[i] = 0.f;
		m_fCurMouseRev[i] = 0.f;
	}

	return NO_EVENT;
}

_bool CSubCamera::OffSetPhsX(_double dTimeDelta, _fmatrix matRev, _vector * pOut)
{
	return _bool();
}

_fmatrix CSubCamera::MakeViewMatrix(_float3 Eye, _float3 At)
{
	return _fmatrix();
}
