#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "GameInstance.h"
#include"DataBase.h"
#include"Player.h"
#include"ControllableActor.h"

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
	CapsuleControllerDesc.height = 0.5f;
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
	XMStoreFloat4x4(&m_matCur, m_pTransformCom->Get_WorldMatrix());

	m_eCurCamMode = Cam_Free;

	
	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCamHelper)
		return EVENT_ERROR;

	_int iResult = NO_EVENT;
	

	switch ( m_pCamHelper->Tick(dTimeDelta,CFilm::LScreen))
	{
	case CCam_Helper::CamHelperState::Helper_None:
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matCur));
		m_fChangeCamModeTime <=1.f ? m_eCurCamMode = CamMode::Cam_AutoToFree : m_eCurCamMode = CamMode::Cam_Free;
		iResult = Tick_CamHelperNone(dTimeDelta);
		OffSetPhsX(dTimeDelta);
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
	Safe_Release(m_pTargetObj);
}

_int CMainCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;
	CTransform* pPlayerTransform = dynamic_cast<CPlayer*>(m_pTargetObj)->Get_Transform();


	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

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
		if (m_fMouseRev[Rev_Prependicul] > 30.f)
			m_fMouseRev[Rev_Prependicul] = 30.f;
		if (m_fMouseRev[Rev_Prependicul] < -90.f)
			m_fMouseRev[Rev_Prependicul] = -90.f;
	}
#ifdef _DEBUG
	if (m_pGameInstance->Key_Pressing(DIK_Z))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_X))
		m_pTransformCom->Go_Backward(dTimeDelta);
#endif
	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;
	XMStoreFloat4x4(&m_matPreRev, matRev);

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{

	if (nullptr == m_pTargetObj)
		return EVENT_ERROR;

	_vector vPlayerPos = dynamic_cast<CPlayer*>(m_pTargetObj)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_fChangeCamModeTime += dTimeDelta;
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); //현재 매트릭스
	_matrix matNext = XMLoadFloat4x4(&m_matBeginWorld); //목표 매트릭스
	//이전에 있던 공전매트릭스를 디폴트공전매트릭스로
	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	_matrix matRevY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;
	XMStoreFloat4x4(&m_matPreRev, matRev);

	
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
		m_eCurCamMode = Cam_Free;
	}

	return NO_EVENT;
}

_int CMainCamera::ReSet_Cam_FreeToAuto()
{

	XMStoreFloat4x4(&m_matPreRev,XMMatrixIdentity());
	m_fChangeCamModeTime = 0.f;
	m_fMouseRev[Rev_Holizontal] = 0.f;
	m_fMouseRev[Rev_Prependicul] = 0.f;

	return NO_EVENT;
}

void CMainCamera::OffSetPhsX(_double dTimeDelta)
{
	XMStoreFloat4x4(&m_matCur, m_pTransformCom->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vPrePhsXPos = XMVectorSet(m_pActorCom->Get_Controller()->getPosition().x, m_pActorCom->Get_Controller()->getPosition().y, m_pActorCom->Get_Controller()->getPosition().z, 1.f);

	if (m_pActorCom->Get_Controller()->move(MH_PxVec3(XMVector4Normalize(vPos - vPrePhsXPos)), 0.001f, dTimeDelta, PxControllerFilters()) & PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_pActorCom->Update_Cam(dTimeDelta);
	}
	
}

#pragma region Cam_Helper
_int CMainCamera::Tick_CamHelperNone(_double dTimeDelta)
{
	if (m_pTargetObj == nullptr)
	{
		m_pTargetObj = CDataStorage::GetInstance()->Get_Player();
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
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{

	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
	{

	}


	/*if (m_eCurCamMode != m_ePreCamMode)
	{
		m_ePreCamMode = m_eCurCamMode;
		m_fChangeCamModeTime = 0.f;
	}*/


	_int iResult = NO_EVENT;
	switch (m_eCurCamMode)
	{
	case Client::CMainCamera::Cam_Free:
		iResult = Tick_Cam_Free(dTimeDelta);
		break;
	case Client::CMainCamera::Cam_AutoToFree:
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
