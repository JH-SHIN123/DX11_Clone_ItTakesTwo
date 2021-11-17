#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "GameInstance.h"
#include"ControllableActor.h"

#include"Level.h"
#include"DataStorage.h"

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


	PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.height = 0.1f;
	CapsuleControllerDesc.radius = 0.5f;
	CapsuleControllerDesc.material = m_pGameInstance->Create_PxMaterial(0.5f, 0.5f, 0.5f);
	CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CapsuleControllerDesc.contactOffset = 0.02f;
	CapsuleControllerDesc.stepOffset = 0.5f;
	CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	CapsuleControllerDesc.slopeLimit = 0.707f;
	CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	////CapsuleControllerDesc.reportCallback = NULL;
	////CapsuleControllerDesc.behaviorCallback = NULL;
	//CapsuleControllerDesc.density = 10.f;
	//CapsuleControllerDesc.scaleCoeff = 0.8f;
	//CapsuleControllerDesc.invisibleWallHeight = 0.f;
	//CapsuleControllerDesc.maxJumpHeight = 10.f;
	//CapsuleControllerDesc.volumeGrowth = 1.5f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom, CapsuleControllerDesc, 0.f)), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_CamHelper"), TEXT("Com_CamHelper"), (CComponent**)&m_pCamHelper), E_FAIL);

	
	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	m_eCurCamMode = CamMode::Cam_AutoToFree;
	m_eCurCamEffect = CamEffect::CamEffect_None;
	m_bPhsXCollision = false;
	
	CDataStorage::GetInstance()->Set_MainCamPtr(this);

	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_SMALL], MakeViewMatrix(_float3(0.f, 3.f, -4.f), _float3(0.f, 1.f, 0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_MEDIUM], MakeViewMatrix(_float3(0.f, 8.f, -7.f), _float3(0.f, 2.f, 0.f)));
	XMStoreFloat4x4(&m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_LARGE], MakeViewMatrix(_float3(0.f, 7.f, -12.f), _float3(0.f, 3.f, 2.f)));

	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{

	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;


	//return CCamera::Tick(dTimeDelta);
	//Check

	Check_Player(dTimeDelta);

	_int iResult = NO_EVENT;
	

	switch ( m_pCamHelper->Tick(dTimeDelta,CFilm::LScreen))
	{
	case CCam_Helper::CamHelperState::Helper_None:
		m_fChangeCamModeTime <=1.f ? m_eCurCamMode = CamMode::Cam_AutoToFree : m_eCurCamMode = CamMode::Cam_Free;
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
	CCamera::Free();

	Safe_Release(m_pCamHelper);
	Safe_Release(m_pActorCom);

}

void CMainCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return;
	CCody* pTargetPlayer = dynamic_cast<CCody*>(m_pTargetObj);

	m_eCurPlayerSize = pTargetPlayer->Get_CurSize();

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
	CTransform* pPlayerTransform = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform();


	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_long MouseMove = 0;

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_fMouseRev[Rev_Holizontal] += (_float)MouseMove * dTimeDelta* m_fMouseRevSpeed[Rev_Holizontal];
		if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
			m_fMouseRev[Rev_Holizontal] = 0.f;
	}
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
		m_fMouseRev[Rev_Prependicul] += (_float)MouseMove* m_fMouseRevSpeed[Rev_Prependicul] * dTimeDelta;
		if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
			m_fMouseRev[Rev_Prependicul] = 0.f;
		else if (m_fMouseRev[Rev_Prependicul] > 60.f)
			m_fMouseRev[Rev_Prependicul] = 60.f;
		else if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}

	//CameraDist
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = XMLoadFloat4x4(&m_matPlayerSizeOffSetMatrix[m_eCurPlayerSize]).r[3];
	switch (m_eCurPlayerSize)
	{
	case Client::CCody::SIZE_SMALL:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.1f));
		break;
	case Client::CCody::SIZE_MEDIUM:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.12f));
		break;
	case Client::CCody::SIZE_LARGE:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - (vLook*(m_fMouseRev[Rev_Prependicul]) * 0.2f));
		break;
	}
	XMStoreFloat4x4(&m_matBeginWorld, m_pTransformCom->Get_WorldMatrix());

	//CamEffect
	switch (m_eCurCamEffect)
	{
	case Client::CMainCamera::CamEffect::CamEffect_None:
		break;
	case Client::CMainCamera::CamEffect::CamEffect_Shake:
		XMStoreFloat4x4(&m_matBeginWorld,Tick_CamEffect_ShakeCamera(dTimeDelta));
		break;
	}
	

	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;

	
#pragma region CamMove
	_vector vResultPos = XMVectorZero();
	m_bPhsXCollision = OffSetPhsX(dTimeDelta, matRev , &vResultPos); //로컬에서 공전후에 충돌검사함
	if (true == m_bPhsXCollision)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPos);
		m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(vResultPos), XMVectorGetY(vResultPos), XMVectorGetZ(vResultPos)));
	}
	else
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matBeginWorld));
#pragma endregion

	XMStoreFloat4x4(&m_matPreRev, matRev);

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

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
	m_fChangeCamModeTime += dTimeDelta * m_fChangeCamModeLerpSpeed;
	
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
	matWorld *= matRev; 
	_vector vPos = matWorld.r[3];
	_vector vPhsXPos = XMVectorSet(m_pActorCom->Get_Controller()->getPosition().x, m_pActorCom->Get_Controller()->getPosition().y, m_pActorCom->Get_Controller()->getPosition().z, 1.f);
	//공전 시킨후 카메라쪽으로 피직스 움직임.
	_vector vDir = XMVector4Normalize(vPos - vPhsXPos);
	_float fCamWithPhsXDist =XMVectorGetX(XMVector4Length(vPos - vPhsXPos));
	if (fCamWithPhsXDist < 0.001f)
		return false;
	//PxControllerState tState;
	//m_pActorCom->Get_Controller()->getState(tState);
	if (m_pActorCom->Get_Controller()->move(MH_PxVec3(vDir), 0.f, dTimeDelta, PxControllerFilters()) & PxControllerCollisionFlag::eCOLLISION_DOWN) // phsX -> Cam if Collision
	{
		if (nullptr == m_pTargetObj)
			return false;
		_vector vPlayerPos = dynamic_cast<CCody*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vPhsXPos = XMVectorSet(m_pActorCom->Get_Controller()->getPosition().x, m_pActorCom->Get_Controller()->getPosition().y, m_pActorCom->Get_Controller()->getPosition().z, 1.f);
		PxRaycastBuffer tBuffer;
		if (false == CPhysX::GetInstance()->Raycast(tBuffer, vPlayerPos, vPhsXPos, 1000)) //플레이어에서 광선쏴서 처음충돌하는 위치
			return false;
		/*else
		{
			
			m_pActorCom->Get_Controller()->setPosition(PxExtendedVec3(tBuffer.block.position.x, tBuffer.block.position.y, tBuffer.block.position.z));
		}*/

		_vector vResultPos = XMVectorSet(tBuffer.block.position.x, tBuffer.block.position.y, tBuffer.block.position.z, 1.f);

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

void CMainCamera::StopCamEffect()
{
	m_eCurCamEffect = CamEffect::CamEffect_None;

	m_dCamEffectTime = 0.0;
	m_dCamEffectDuration = 0.0;
	//For.ReSetCamEffect_Shake
	m_fShakeCycleLength = 0.0f;
	m_fShakeMaxHeight = 0.0f;
	m_fShakeMaxWidth = 0.0f;
}

void CMainCamera::StartCamEffect_Shake(CamEffect eCamEffect, _double dDuration, _float fCycleLength, _float fMaxWidth, _float fMaxHeight)
{
	if (m_eCurCamEffect == eCamEffect)
		return;
	m_dCamEffectTime = 0.0;
	m_dCamEffectDuration = dDuration;
	m_eCurCamEffect = eCamEffect;
	m_fShakeCycleLength = fCycleLength;
	m_fShakeMaxHeight = fMaxHeight;
	m_fShakeMaxWidth = fMaxWidth;

}

_fmatrix CMainCamera::Tick_CamEffect_ShakeCamera(_double dTimeDelta)
{
	return _matrix();
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
