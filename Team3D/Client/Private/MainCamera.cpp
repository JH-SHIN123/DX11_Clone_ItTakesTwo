#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "Cody.h"
#include "CameraActor.h"
#include "PlayerActor.h"
#include"Bridge.h"
#include"CutScenePlayer.h"
#include"UFORadarSet.h"
#include"UFORadarLever.h"
#include"UFORadarScreen.h"
#include"UI_Generator.h"
#include"PinBall.h"
#include"PinBall_Handle.h"
#include"UmbrellaBeam.h"
#include"May.h"
#include"LaserTennis_Manager.h"
#include"AlphaScreen.h"
#include"Script.h"
#include "SubCamera.h"
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
	

	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_SMALL] =	{ 0.f,1.f,-1.f,1.f};
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM] =	{ 0.f,7.f,-7.f,1.f };
	m_vSizeEye[CCody::PLAYER_SIZE::SIZE_LARGE] =	{ 0.f,8.f,-8.f,1.f };

	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_SMALL] = { 0.f,0.15f,0.0f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM] = { 0.f,1.5f, 0.0f,1.f };
	m_vSizeAt[CCody::PLAYER_SIZE::SIZE_LARGE] = { 0.f,5.5f,0.0f,1.f };

	_matrix matStart = MakeViewMatrixByUp(m_vSizeEye[CCody::PLAYER_SIZE::SIZE_MEDIUM], m_vSizeAt[CCody::PLAYER_SIZE::SIZE_MEDIUM]);
	XMStoreFloat4x4(&m_matBeginWorld, matStart);

	//For.BossRoom_MiniCody
	CFilm* pLine = m_pCamHelper->Get_Film(TEXT("Line_BossRoom_MiniCody"));
	if (nullptr == pLine)
	{
		return EVENT_ERROR;
	}
	m_CamNodes = *(pLine->Get_CamNodes());
	
	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (false == m_bStart)
	{
		m_pCody = static_cast<CCody*>(CDataStorage::GetInstance()->GetCody());
		if (m_pCody)
		{
			CTransform* pPlayerTransform = m_pCody->Get_Transform();
			XMStoreFloat4(&m_vPlayerPos, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vPlayerUp, pPlayerTransform->Get_State(CTransform::STATE_UP));
			Safe_AddRef(m_pCody);
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

	

	switch (m_pCamHelper->Tick(dTimeDelta,CFilm::LScreen))
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

_int CMainCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);



	return NO_EVENT;
}


_int CMainCamera::Check_Player(_double dTimeDelta)
{
	if (nullptr == m_pCody)
		return EVENT_ERROR;


	
	m_eCurPlayerSize = m_pCody->Get_Player_Size();
	if (m_eCurPlayerSize == CCody::PLAYER_SIZE::SIZE_LARGE)
	{

	}
	if (m_eCurPlayerSize != m_ePrePlayerSize)
	{
		m_dLerpToCurSizeTime = 0.0;
		m_ePrePlayerSize = m_eCurPlayerSize;
	}
	
	LerpToCurSize(m_eCurPlayerSize, dTimeDelta);

	if (m_bOpenThirdFloor&&m_fOpenThirdFloorTime == 0.f)
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_OpenThirdFloor;
		UI_Generator->Set_AllActivation(false);
	}

	if (m_pCody->Get_IsWarpNextStage() == true)
	{
		m_eCurCamMode = CamMode::Cam_Warp_WormHole;
	}
	if (m_pCody->Get_IsPinBall())
	{
		m_eCurCamMode = CamMode::Cam_PinBall_Cody;
	}
	if (DATABASE->Get_BigButtonPressed() && static_cast<CBridge*>(DATABASE->Get_Bridge())->Get_IsUppendede() == false)
	{
		m_eCurCamMode = CamMode::Cam_PressButton_Bridge;
	}
	if (m_pCody->Get_IsInArcadeJoyStick())	//레이져쏘기
	{
		m_eCurCamMode = CamMode::Cam_InJoyStick;
	}
	if (m_pCody->Get_IsPlayerInUFO() && m_eCurCamFreeOption != CamFreeOption::Cam_Free_OnBossMiniRoom_Cody)
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_OnBossMiniRoom_Cody;
		//m_pGameInstance->Set_ViewportInfo(XMVectorSet(0.f,0.f,0.6f,1.f), XMVectorSet(0.6f,0.f,0.4f,1.f));
		return	ReSet_Cam_Free_OnRail();
	}
	if (m_pCody->Get_IsControllJoyStick())
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_Umbrella_Laser;
	}
	if (m_pCody->Get_IsWallJump())
	{
		m_eCurCamMode = CamMode::Cam_WallJump;
	}
	if (m_bOpenThirdFloor && m_fOpenThirdFloorTime == 0.f)
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_OpenThirdFloor;
	if (CLaserTennis_Manager::GetInstance()->Get_StartGame() && m_eCurCamMode != CamMode::Cam_LaserTennis)
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 1.f, 1.f),1.5f);
		m_eCurCamMode = CamMode::Cam_LaserTennis;
	}

	if (m_pCody->Get_IsEnding())
		m_eCurCamMode = CamMode::Cam_Ending;
	
	return NO_EVENT;
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

HRESULT CMainCamera::Start_CamEffect(const _tchar * pEffectTag)
{
	m_pCamHelper->Start_CamEffect(pEffectTag,CFilm::LScreen);
	return S_OK;
}



_int CMainCamera::Tick_Cam_Free(_double dTimeDelta)
{
	if (nullptr == m_pCody)
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
	case CMainCamera::CamFreeOption::Cam_Free_OpenThirdFloor:
		iResult = Tick_Cam_Free_OpenThirdFloor(dTimeDelta);
		break;
	case CMainCamera::CamFreeOption::Cam_Free_OnBossMiniRoom_Cody:
		iResult = Tick_Cam_Free_OnBossMiniRoom_Cody(dTimeDelta);
		break;
	case CMainCamera::CamFreeOption::Cam_Free_Umbrella_Laser:
		iResult = Tick_Cam_Free_Umbrella_Laser(dTimeDelta);
		break;

	}
	return iResult;
}

_int CMainCamera::Tick_Cam_AutoToFree(_double dTimeDelta)
{

	if (nullptr == m_pCody)
		return EVENT_ERROR;
	if (m_fChangeCamModeTime >= 1.f)
	{	
		m_eCurCamMode = CamMode::Cam_Free;
		return NO_EVENT;
	}
	CTransform* pPlayerTransform = m_pCody->Get_Transform();
	if (m_fChangeCamModeTime == 0.f) 
	{

		_matrix matWorld = m_pTransformCom->Get_WorldMatrix(); 
		XMStoreFloat4x4(&m_matCurWorld, matWorld);
		m_CameraDesc.fFovY = XMConvertToRadians(60.f);
	}

	m_fChangeCamModeTime += (_float)(dTimeDelta * m_fChangeCamModeLerpSpeed);

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(XMLoadFloat4x4(&m_matCurWorld), MakeViewMatrix_FollowPlayer(dTimeDelta), m_fChangeCamModeTime));

	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Ending(_double dTimeDelta)
{
	CTransform* pPlayerTransform = m_pCody->Get_Transform();

	_vector vLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//0.f ,-500.f ,0.f
	_vector vCamPos = XMVectorSet(0.f, XMVectorGetY(vPlayerPos) + 9.f, 0.f, 1.f); 
	_vector vAt = XMVectorSet(0.f, XMVectorGetY(vPlayerPos), 0.f,1.f);

	m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vCamPos, vAt,XMVectorSet(0.f,0.f,1.f,0.f)));
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Free_FollowPlayer(_double dTimeDelta)
{
	m_pTransformCom->Set_WorldMatrix(MakeViewMatrix_FollowPlayer(dTimeDelta));
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
_int CMainCamera::Tick_Cam_Free_OpenThirdFloor(_double dTimeDelta)
{
	m_fOpenThirdFloorTime += (_float)dTimeDelta;
	_float fDelay = 2.f;
	if (m_fOpenThirdFloorTime > fDelay *9.f)
	{
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
		ReSet_Cam_FreeToAuto(true ,false,1.f);
		UI_Generator->Set_AllActivation(true);
	}
	else if (m_fOpenThirdFloorTime > fDelay *8.f)
		SCRIPT->Render_Script(53, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay *7.f)
		SCRIPT->Render_Script(52, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay *6.f)
		SCRIPT->Render_Script(51, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay *5.f)
		SCRIPT->Render_Script(50, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime >fDelay * 4.f)
		SCRIPT->Render_Script(49, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay *3.f)
		SCRIPT->Render_Script(48, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay * 2.f)
		SCRIPT->Render_Script(47, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > fDelay)
		SCRIPT->Render_Script(46, CScript::SCREEN::HALF, fDelay);
	else if (m_fOpenThirdFloorTime > 0.f)
		SCRIPT->Render_Script(45, CScript::SCREEN::HALF, fDelay);

	_vector vCodyPos = m_pCody->Get_Position();
	_vector vUpPos = XMVectorSet(65.f, 218.f, 179.f, 1.f);
	_vector vMiddleDir = (vCodyPos - vUpPos);
	_vector vLook = XMVector3Normalize(vMiddleDir);
	_vector vMiddlePos = vCodyPos - vMiddleDir * 0.5f;
	_vector vTargetPos = vMiddlePos;

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_vector vEye = vCodyPos + XMVectorSet(XMVectorGetX(vUp)* 100.f, 0.f, XMVectorGetZ(vUp) * 100.f, 0.f);
	vEye = XMVectorSetY(vEye, XMVectorGetY(vMiddlePos));
	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vMiddlePos), (_float)dTimeDelta));

	
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
_float CMainCamera::Get_ZoomVal_OnRail(_uint iNodeIdx, _bool bCanDash)
{
	
	_float fZoomVal = 0.66f;
	if (iNodeIdx < 3)
		fZoomVal = 0.66f;
	else if (iNodeIdx < 4)
		fZoomVal = 0.69f;
	else if (iNodeIdx < 6)
		fZoomVal = 0.72f;
	else if  (iNodeIdx < 8)
		fZoomVal = 0.66f;
	else if (iNodeIdx < 11)
		fZoomVal = 0.8f;
	else if (iNodeIdx < 14)
		fZoomVal = 0.76f;
	else if (iNodeIdx < 16)
		fZoomVal = 0.6f;
	else if (iNodeIdx < 17)
		fZoomVal = 0.74f;
	else if (iNodeIdx < 19)
		fZoomVal = 0.73f;
	else if (iNodeIdx < 21)
		fZoomVal = 0.85f;
	else if (iNodeIdx < 23)
		fZoomVal = 0.9f;
	else if (iNodeIdx < 24)
		fZoomVal = 0.66f;
	else if (iNodeIdx < 26)
		fZoomVal = 0.64f;
	else if (iNodeIdx < 28)
		fZoomVal = 0.9f;
	else if (iNodeIdx < 29)
		fZoomVal = 0.7f;
	else if (iNodeIdx < 31)
		fZoomVal = 0.8f;
	else if (iNodeIdx < 32)
		fZoomVal = 0.67f;
	else if (iNodeIdx < 35)
		fZoomVal = 0.9f;
	else if (iNodeIdx < 36)
		fZoomVal = 0.76f;
	else if (iNodeIdx < 38)	//올라가는구간
		fZoomVal = 0.72f;
	else if (iNodeIdx < 53)
		fZoomVal = 0.62f;
	if (bCanDash)
	{
		_vector vCurCodyPos = m_pCody->Get_Position();
		_vector vPreCodyPos = XMVectorSetW(XMLoadFloat3(&m_vCurRailAt), 1.f);
		_float fLength = XMVectorGetX(XMVector3Length(vCurCodyPos - vPreCodyPos));
		XMStoreFloat3(&m_vCurRailAt, vCurCodyPos);
		return fZoomVal * fLength * 50.f;
	}
	else
		return fZoomVal;
}
_float CMainCamera::DotProgress(_float fOffSetDist)
{
	CFilm::CamMoveOption eCurOption = m_CamNodes[m_iNodeIdx[0]]->eEyeMoveOption;

	_vector vPlayerPos = m_pCody->Get_Position();
	
	_vector vCurNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[0]]->vEye), 1.f);
	_vector vNextNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[1]]->vEye), 1.f);

	_double dCurNodeTime = m_CamNodes[m_iNodeIdx[0]]->dTime;
	_double dNextNodeTime = m_CamNodes[m_iNodeIdx[1]]->dTime;
	_float fLength = (_float)(dNextNodeTime - dCurNodeTime);

	_vector vDirNextNodeWithCurNode = vNextNodePos - vCurNodePos;
	_vector vDirNextNodeWithPlayer = vNextNodePos - (vPlayerPos - XMVector3Normalize(vDirNextNodeWithCurNode) * fOffSetDist); //

	_float fDot = XMVectorGetX(XMVector3Dot(vDirNextNodeWithPlayer, vDirNextNodeWithCurNode))
		/ pow(XMVectorGetX(XMVector3Length(vDirNextNodeWithCurNode)), 2);

	return (_float)dCurNodeTime + (1.f-fDot)* fLength;
}
_float CMainCamera::DotProgress_Bezier(_float fOffSetDist)
{
	_vector vPlayerPos = m_pCody->Get_Position();
	_vector vCurNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[0]]->vEye), 1.f);
	_vector vSecondNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[1]]->vEye), 1.f);	//중앙의 노드
	_vector vThirdNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[2]]->vEye), 1.f);

	//_vector vDirTotal = vThirdNodePos - vCurNodePos;
	//_vector vDirTotalWithPlayerPos = (vThirdNodePos - vPlayerPos);
	////vDirTotalWithPlayerPos += XMVector3Normalize(vDirTotal);
	//fProgress = XMVectorGetX(XMVector3Dot(vDirTotalWithPlayerPos, vDirTotal))
	//	/ pow(XMVectorGetX(XMVector3Length(vDirTotal)), 2);

	_vector vDirSecondNodeWithCurNode = vSecondNodePos - vCurNodePos;
	_vector vDirThirdNodeWithSecondNode = vThirdNodePos - vSecondNodePos;


	_vector vDirSecondNodeWithPlayerPos = vSecondNodePos - (vPlayerPos - XMVector3Normalize(vDirSecondNodeWithCurNode)* fOffSetDist) ;
	_vector vDirThirdNodeWithPlayerPos = vThirdNodePos - (vPlayerPos - XMVector3Normalize(vDirThirdNodeWithSecondNode)* fOffSetDist);
	
	_float fFirstNodeProgress = XMVectorGetX(XMVector3Dot(vDirSecondNodeWithPlayerPos, vDirSecondNodeWithCurNode))
		/ pow(XMVectorGetX(XMVector3Length(vDirSecondNodeWithCurNode)), 2);
	_float fSecondNodeProgress = XMVectorGetX(XMVector3Dot(vDirThirdNodeWithPlayerPos, vDirThirdNodeWithSecondNode))
		/ pow(XMVectorGetX(XMVector3Length(vDirThirdNodeWithSecondNode)), 2);
	
	_float fLength = (_float)(m_CamNodes[m_iNodeIdx[2]]->dTime - m_CamNodes[m_iNodeIdx[0]]->dTime);
	return (_float)m_CamNodes[m_iNodeIdx[0]]->dTime +  fLength* (1.f -(fFirstNodeProgress + fSecondNodeProgress) * 0.5f);
}
_float CMainCamera::Find_Player_OnBossMiniRoom(_double dTimeDelta)
{
	_vector vPlayerPos = m_pCody->Get_Position();
	_float fFindProgress = 0.f;
	_uint iCurNodeIdx = m_iNodeIdx[0];
	
	if (iCurNodeIdx > 0 && iCurNodeIdx < 23)
	{
		for (_uint i = 0; i < 4; i++)
			m_iNodeIdx[i] = i;
		fFindProgress = DotProgress(0.6f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
		for (_uint i = 6; i < 10; i++)
			m_iNodeIdx[i - 6] = i;
		fFindProgress = DotProgress(0.6f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
	
		for (_uint i = 11; i < 15; i++)
			m_iNodeIdx[i - 11] = i;
		fFindProgress = DotProgress(0.6f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;

	}
	
		for (_uint i = 23; i < 27; i++)
			m_iNodeIdx[i - 23] = i;
		fFindProgress = DotProgress(0.5f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
		for (_uint i = 28; i < 32; i++)
			m_iNodeIdx[i-28] = i;
		fFindProgress = DotProgress(1.1f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
	
		for (_uint i = 31; i < 35; i++)
			m_iNodeIdx[i-31] = i;
		fFindProgress = DotProgress(0.8f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
		for (_uint i = 35; i < 39; i++)
			m_iNodeIdx[i-35] = i;
		fFindProgress = DotProgress(0.5f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	
		for (_uint i = 41; i < 45; i++)
			m_iNodeIdx[i-41] = i;
		fFindProgress = DotProgress(0.8f);
		if (fFindProgress >= m_CamNodes[m_iNodeIdx[0]]->dTime && fFindProgress < m_CamNodes[m_iNodeIdx[1]]->dTime)
			return fFindProgress;
	

	return 9999999999.f;
}
#pragma endregion
_int CMainCamera::Tick_Cam_Free_OnBossMiniRoom_Cody(_double dTimeDelta)
{
	if (m_pCody->Get_IsPlayerInUFO() == false)
	{
		ReSet_Cam_FreeToAuto();
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f),
			XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	}
	
	_bool bCurCodyRespawn = m_pCody->Get_RespawnCheck();

	if (m_bRespawn != bCurCodyRespawn)
	{
		if (m_bRespawn == false &&  bCurCodyRespawn == true)	
		{

		_float fFindProgress = Find_Player_OnBossMiniRoom(dTimeDelta);
			if (fFindProgress >= 9999999999.f)
				ReSet_Cam_Free_OnRail();
			else
				m_fRailProgressTime = fFindProgress;
			m_bRespawn = bCurCodyRespawn;
			return NO_EVENT;
		}
		else
			m_bRespawn = false;
	}

	if (false == m_bStartOnRail)
	{
		XMStoreFloat3(&m_vCurRailAt, m_pCody->Get_Position());
		m_bStartOnRail = true;
	}

	if (m_iNodeIdx[0] < 3)
	{
		_float fProgress = DotProgress(0.6f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if (m_iNodeIdx[0] < 6)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 8)
	{
		_float fProgress = DotProgress(0.6f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if (m_iNodeIdx[0] < 11)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 12)
	{
		_float fProgress = DotProgress(0.6f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if (m_iNodeIdx[0] < 17)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 19)
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_D))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 21)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 23)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);

	}
	else if (m_iNodeIdx[0] < 24)
	{

		_float fProgress = DotProgress(0.5f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
		//m_fRailProgressTime = DotProgress(0.5f);
	}
	else if (m_iNodeIdx[0] < 28)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);

	}
	else if (m_iNodeIdx[0] < 29)
	{

		_float fProgress = DotProgress(1.1f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if(m_iNodeIdx[0] <31)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);

	}
	else if (m_iNodeIdx[0] < 32)
	{

		_float fProgress = DotProgress(0.8f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if (m_iNodeIdx[0] < 35)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);

	}
	else if (m_iNodeIdx[0] < 36)
	{
		_float fProgress = DotProgress(0.5f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else if(m_iNodeIdx[0] < 38)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 39)	//올라가는 구간
	{
		_float fCurNodeY = m_CamNodes[m_iNodeIdx[0]]->vEye.y;
		_float fNextNodeY = m_CamNodes[m_iNodeIdx[1]]->vEye.y;
		_float fPlayerY = XMVectorGetY(m_pCody->Get_Position());

		_bool bIsStart = fCurNodeY > fPlayerY;
		_float fPlusProgressPercentage = bIsStart ?  0.01f : (fPlayerY - fCurNodeY) / (fNextNodeY - fCurNodeY);
		_float fLength = (_float)m_CamNodes[m_iNodeIdx[1]]->dTime - (_float)m_CamNodes[m_iNodeIdx[0]]->dTime;
		m_fRailProgressTime = (_float)m_CamNodes[m_iNodeIdx[0]]->dTime +  fPlusProgressPercentage * fLength /** dTimeDelta*/;
	}
	else if (m_iNodeIdx[0] < 40)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if (m_iNodeIdx[0] < 41)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	}
	else if(m_iNodeIdx[0] < 42)
	{
		_float fProgress = DotProgress(0.8f);
		m_fRailProgressTime += (fProgress - m_fRailProgressTime) * (_float)dTimeDelta * 3.f;
	}
	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_fRailProgressTime += (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);
		else if (m_pGameInstance->Key_Pressing(DIK_S))
			m_fRailProgressTime -= (_float)dTimeDelta * Get_ZoomVal_OnRail(m_iNodeIdx[0]);

	}

	_bool bIsFinishNode = false;
	_matrix matResult = m_pCamHelper->Get_CamNodeMatrix(m_CamNodes[m_iNodeIdx[0]], m_CamNodes[m_iNodeIdx[1]], m_CamNodes[m_iNodeIdx[2]], m_CamNodes[m_iNodeIdx[3]], m_fRailProgressTime,&bIsFinishNode);
	if (m_fRailProgressTime < m_CamNodes[m_iNodeIdx[0]]->dTime)
	{
		if (m_iNodeIdx[0] > 0)
		{
			CFilm::CamMoveOption eOption = m_CamNodes[m_iNodeIdx[0] - 1]->eEyeMoveOption;
			switch (eOption)
			{
			
			case Client::CFilm::CamMoveOption::Move_Straight:
				for (_uint i = 0; i < 4; i++)
				{
					m_iNodeIdx[i] -= 1;
				}
				break;
			case Client::CFilm::CamMoveOption::Move_Bezier_3:
				for (_uint i = 0; i < 4; i++)
				{
					m_iNodeIdx[i] -= 2;
				}
				break;
			}
		}
		else
			m_fRailProgressTime = 0.f;
	}
	if (bIsFinishNode)
	{
		CFilm::CamMoveOption eOption = m_CamNodes[m_iNodeIdx[0]]->eEyeMoveOption;
		switch (eOption)
		{
		case CFilm::CamMoveOption::Move_Straight:
			for (_uint i = 0; i < 4; i++) 
			{
				if(m_iNodeIdx[i]+1 <= m_CamNodes.size()-2)
					m_iNodeIdx[i]++;
			}
			break;
		case CFilm::CamMoveOption::Move_Bezier_3:
		{
			for (_uint i = 0; i < 4; i++)
			{
				if (m_iNodeIdx[i] + 2 <= m_CamNodes.size()-3)
				m_iNodeIdx[i] += 2;
			}
		}
			break;
		default:
			break;
		}
	}
	m_pCamHelper->Start_CamEffect(L"Cam_Shake_Boss_InUFO", CFilm::LScreen);

	if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, matResult))
		matResult = m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen);

	m_pTransformCom->Set_WorldMatrix(matResult);

	return NO_EVENT;
#pragma region DotProgress
	//CFilm::CamMoveOption eCurOption = m_CamNodes[m_iNodeIdx[0]]->eEyeMoveOption;

	//_matrix matResult = XMMatrixIdentity();
	//
	//_vector vPlayerPos = XMVectorSetW(XMLoadFloat3(&m_vCurRailAt) , 1.f);
	//_float fZoomVal = Get_ZoomVal_OnRail(m_iNodeIdx[0]);
	//if (XMVectorGetX(XMVector3Length(vPlayerPos - m_pCody->Get_Position())) > fZoomVal)
	//{
	//	vPlayerPos += XMVectorRound(dTimeDelta * (m_pCody->Get_Position() - vPlayerPos) * 100.f) / 100.f;
	//	XMStoreFloat3(&m_vCurRailAt, vPlayerPos);
	//}

	//switch (eCurOption)
	//{
	//case Client::CFilm::CamMoveOption::Move_Straight:
	//	{
	//		_vector vCurNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[0]]->vEye),1.f);
	//		_vector vNextNodePos = XMVectorSetW(XMLoadFloat3(&m_CamNodes[m_iNodeIdx[1]]->vEye), 1.f);
	//	
	//		_double dCurNodeTime = m_CamNodes[m_iNodeIdx[0]]->dTime;
	//		_double dNextNodeTime = m_CamNodes[m_iNodeIdx[1]]->dTime;

	//		_vector vDirNextNodeWithCurNode = vNextNodePos - vCurNodePos;
	//		_vector vDirNextNodeWithPlayer = vNextNodePos - (vPlayerPos); //
	//		
	//		_float fDot = XMVectorGetX(XMVector3Dot(vDirNextNodeWithPlayer,vDirNextNodeWithCurNode))
	//			/ pow(XMVectorGetX(XMVector3Length(vDirNextNodeWithCurNode)),2); 
	//		_float fTheta = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirNextNodeWithPlayer),
	//			XMVector3Normalize(vDirNextNodeWithCurNode)));


	//		_bool bIsFinish = false;
	//		matResult = m_pCamHelper->Get_CamNodeMatrix(m_CamNodes[m_iNodeIdx[0]], m_CamNodes[m_iNodeIdx[1]], m_CamNodes[m_iNodeIdx[2]], m_CamNodes[m_iNodeIdx[3]], 
	//			dCurNodeTime + (dNextNodeTime - dCurNodeTime)*(1.f - fDot),&bIsFinish);
	//		if (bIsFinish)//넘어감
	//		{
	//			for (_uint i = 0; i < 4; i++)
	//			{
	//				m_iNodeIdx[i] += 1;
	//			}
	//		}
	//		else if (XMVectorGetX(XMVector3Length(vDirNextNodeWithCurNode)) < XMVectorGetX(XMVector3Length(vDirNextNodeWithPlayer))
	//			&& fTheta > 0.f)
	//		{
	//			if ((_int)m_iNodeIdx[0] - 1 < 0)
	//				break;
	//			CFilm::CamMoveOption ePreOption = m_CamNodes[m_iNodeIdx[0] - 1]->eEyeMoveOption;
	//			for (_uint i = 0; i < 4; i++)
	//			{
	//				switch (ePreOption)
	//				{
	//				case Client::CFilm::CamMoveOption::Move_Jump:
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Straight:
	//					m_iNodeIdx[i] -= 1;
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Bezier_3:
	//					m_iNodeIdx[i] -= 2;
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Bezier_4:
	//					break;
	//				}
	//				
	//				
	//			}
	//		}
	//	}
	//	break;
	//case Client::CFilm::CamMoveOption::Move_Bezier_3:
	//{
	//	
	//
	//	_double dCurNodeTime = m_CamNodes[m_iNodeIdx[0]]->dTime;
	//	_double dNextNodeTime = m_CamNodes[m_iNodeIdx[1]]->dTime;


	//	_bool bIsFinish = false;
	//	matResult = m_pCamHelper->Get_CamNodeMatrix(m_CamNodes[m_iNodeIdx[0]], m_CamNodes[m_iNodeIdx[1]], m_CamNodes[m_iNodeIdx[2]], m_CamNodes[m_iNodeIdx[3]],
	//		dCurNodeTime + (dNextNodeTime - dCurNodeTime)*(1.f - m_fRailProgressTime), &bIsFinish);
	//	_vector vDirCurNodeWithPlayer = vPlayerPos - vCurNodePos;
	//	_vector vDirThirdNodeWithCurNode = vThirdNodePos - vCurNodePos;
	//	_float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirThirdNodeWithPlayerPos),
	//		XMVector3Normalize(vDirThirdNodeWithSecondNode)));
	//	if (bIsFinish || fDot < 0.f)//넘어감
	//	{
	//		for (_uint i = 0; i < 4; i++)
	//		{
	//			m_iNodeIdx[i] +=2;
	//		}
	//	}
	//	else if (XMVectorGetX(XMVector3Length(vDirSecondNodeWithCurNode)) < XMVectorGetX(XMVector3Length(vDirSecondNodeWithPlayerPos))
	//		&& m_fRailProgressTime <= 0.f)
	//	//if (XMVectorGetX(XMVector3Length(vDirTotal)) < XMVectorGetX(XMVector3Length(vDirTotalWithPlayerPos))
	//	//	&& fProgress >= 1.f)	//둔각.
	//		{
	//			if ((_int)m_iNodeIdx[0] - 1 < 0)
	//				break;
	//			CFilm::CamMoveOption ePreNodeOption = m_CamNodes[(_int)m_iNodeIdx[0] -1]->eEyeMoveOption;
	//			for (_uint i = 0; i < 4; i++)
	//			{
	//				
	//				switch (ePreNodeOption)
	//				{
	//				case Client::CFilm::CamMoveOption::Move_Jump:
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Straight:
	//					m_iNodeIdx[i] = -1;
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Bezier_3:
	//					m_iNodeIdx[i] -= 2;
	//					break;
	//				case Client::CFilm::CamMoveOption::Move_Bezier_4:
	//					break;
	//				}
	//				
	//			
	//			}
	//		}
	//}
	//	break;
	//case Client::CFilm::CamMoveOption::Move_Bezier_4:
	//	break;
	//}
#pragma endregion
}
_int CMainCamera::Tick_Cam_Free_Umbrella_Laser(_double dTimeDelta)
{
	if (m_pCody->Get_IsControllJoyStick() == false)
	{
		ReSet_Cam_FreeToAuto(true);
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	}
	CUmbrellaBeam* pUmbrellaBeam = static_cast<CUmbrellaBeam*>(DATABASE->Get_UmbrellaBeam());
	CTransform* pBeamTransform = pUmbrellaBeam->Get_Transform();

	_vector vBeamLook = pBeamTransform->Get_State(CTransform::STATE_LOOK);
	_vector vBeamPos = pBeamTransform->Get_State(CTransform::STATE_POSITION);
	_vector vBeamUp = pBeamTransform->Get_State(CTransform::STATE_UP);
	_vector vEye = vBeamPos - vBeamLook * 7.f + vBeamUp * 7.f;
	vBeamPos += vBeamLook * 18.f;
	//OffSetPhsX(vEye, vBeamPos, dTimeDelta, &vEye);

	m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vEye, vBeamPos),(_float)dTimeDelta * 2.f));

	return NO_EVENT;
}
_int CMainCamera::Tick_Cam_Warp_WormHole(_double dTimeDelta)
{
	if (nullptr == m_pCody)
		return EVENT_ERROR;
	
	
	CTransform* pPlayerTransform = m_pCody->Get_Transform();
	if (m_dWarpTime < 2.0) //카메라가 게이트 전방으로,페이드인.
	{
		_matrix matPortal =XMLoadFloat4x4(&m_matStartPortal);
		_float4 vTargetEye, vTargetAt;
		XMStoreFloat4(&vTargetEye, matPortal.r[3] + 8 * XMVector3Normalize(matPortal.r[2]));
		XMStoreFloat4(&vTargetAt, matPortal.r[3]);
		vTargetEye.y += 7.0f;
		vTargetAt.y += 6.5f;
		if (false == m_bIsFading && m_dWarpTime > 0.1)
		{
			UI_CreateOnlyOnce(Cody, WhiteScreenFadeInOut);
			UI_Generator->Set_FadeInSpeed(Player::Cody, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);
			
			m_bIsFading = true;
		}

		m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vTargetEye, vTargetAt),(_float)m_dWarpTime));
		m_dWarpTime += dTimeDelta;
	}
	else if (m_pCody->Get_IsWarpNextStage() && m_pCody->Get_IsWarpDone()) //게이트안에서,페이드아웃
	{
		if (m_bIsFading)
		{
			UI_Generator->Set_FadeOut(Player::Cody, UI::WhiteScreenFadeInOut);
			UI_Generator->Set_FadeOutSpeed(Player::Cody, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);
			m_bIsFading = false;
		}
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
		_vector vCamPos = vPlayerPos - vPlayerLook * 3.f  + vPlayerUp * 1.5f;
		_float4 vEye, vAt;
		XMStoreFloat4(&vEye, vCamPos);
		XMStoreFloat4(&vAt, vPlayerPos + vPlayerUp* 1.5f);
		_matrix matResult = MakeViewMatrixByUp(vEye, vAt);
		m_pCamHelper->Start_CamEffect(TEXT("Cam_Shake_Loc_Right_Warp_Potal"), CFilm::LScreen);
		m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, matResult);
		m_pTransformCom->Set_WorldMatrix(m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen));
	}
	else if (m_pCody->Get_IsWarpDone())
	{
		//페이드인
		if (!m_bIsFading)
		{
			UI_CreateOnlyOnce(Cody, WhiteScreenFadeInOut);
			UI_Generator->Set_FadeInSpeed(Player::Cody, UI::WhiteScreenFadeInOut, (_float)dTimeDelta * 255.f);
			m_bIsFading = true;
		}
	}
	else
	{
		//페이드아웃
		if (m_bIsFading)
		{
			UI_Generator->Set_FadeOut(Player::Cody, UI::WhiteScreenFadeInOut);
			UI_Generator->Set_FadeOutSpeed(Player::Cody, UI::WhiteScreenFadeInOut, (_float)dTimeDelta*2.f * 255.f);
			m_bIsFading = false;
		}
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
		
		m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vPlayerPos - vPlayerLook * 2.f,vPlayerPos));
		m_dWarpTime = 0.0;
		ReSet_Cam_FreeToAuto(true);
		m_fChangeCamModeLerpSpeed = 4.f;
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
		m_fBridgeUppendTime += (_float)dTimeDelta;
	}
	for (_uint i = 0; i < 2; i++)
	{
		if (fabs(fTargetRev[i] - m_fCurMouseRev[i]) < 1.f)
			bIsFinishRev[i] = true;
		m_fCurMouseRev[i] += (fTargetRev[i] - m_fCurMouseRev[i])* (_float)dTimeDelta;
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
	
	}
	m_pTransformCom->Set_WorldMatrix(matResult);
	
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_InJoystick(_double dTimeDelta)
{
	if (m_pCody->Get_IsInArcadeJoyStick() == false)
	{
		ReSet_Cam_FreeToAuto();
	}
	CUFORadarSet* pRaderSet =  static_cast<CUFORadarSet*>(DATABASE->Get_UFORadarSet());
	CTransform* pRaderScreenTransform = pRaderSet->Get_RaderScreen()->Get_Transform();
	CTransform* pRaderLeverTransform =	pRaderSet->Get_RadarLever()->Get_Transform();
	CTransform* pPlayerTransform = m_pCody->Get_Transform();


	_vector vRaderLeverPos = pRaderLeverTransform->Get_State(CTransform::STATE_POSITION);
	
	_vector vRaderScreenPos = pRaderScreenTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
	_vector vRaderScreenLook = XMVector3Normalize(pRaderScreenTransform->Get_State(CTransform::STATE_LOOK));

	_vector vEye = vRaderScreenPos - vRaderScreenLook * 2.2f + XMVectorSet(0.f,0.2f,-0.8f,0.f);

	_matrix matResult = MakeViewMatrixByUp(vEye, vRaderScreenPos);
	m_pTransformCom->Set_WorldMatrix(matResult);

	m_CameraDesc.fFovY = XMConvertToRadians(30.f);
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_PinBall_Cody(_double dTimeDelta)
{
	//코디랑 공들어오면
	CPinBall* pPinBall = static_cast<CPinBall*>(DATABASE->Get_Pinball());
	CPinBall_Handle* pPinBallHandle = static_cast<CPinBall_Handle*>(DATABASE->Get_Pinball_Handle());
	CTransform* pPinBallTransform = pPinBall->Get_Transform();
	
	_vector vPinBallPos = pPinBallTransform->Get_State(CTransform::STATE_POSITION);
	
	if (false == pPinBallHandle->Get_PlayerMove() && false == pPinBall->Get_StartGame()
		&& false == pPinBall->Get_Failed() && m_pCody->Get_IsPinBall() &&false == pPinBallHandle->Get_Goal())	//코디가 들어가고,메이가 잡기전
	{
		
		_vector vStartPos =	 vPinBallPos + XMVectorSet(2.f, 1.f, 0.f,0.f);
		_vector vSecondPos = vPinBallPos + XMVectorSet(2.f, 1.3f, 2.f, 0.f);
		_vector vTargetPos = vPinBallPos + XMVectorSet(0.f, 1.5f, 2.5f, 0.f);

		_vector vAt = vPinBallPos + XMVectorSet(0.f,0.75f,0.f,0.f);

		_float3 vBezier1, vBezier2, vBezier3;
		XMStoreFloat3(&vBezier1, vStartPos);
		XMStoreFloat3(&vBezier2, vSecondPos);
		XMStoreFloat3(&vBezier3, vTargetPos);

		m_fStartPinBallBezierTime += (_float)dTimeDelta;
		_vector vCurEye = XMVectorSetW(XMLoadFloat3(&m_pCamHelper->MakeBezier3(vBezier1, vBezier2, vBezier3, m_fStartPinBallBezierTime)),1.f);
		_matrix matResult = MakeViewMatrixByUp(vCurEye, vAt);
		m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(),matResult,m_fStartPinBallBezierTime*2.f));
	}
	else if (false == pPinBall->Get_Failed() && false == pPinBallHandle->Get_Goal() && 
		false == pPinBall->Get_StartGame() && m_pCody->Get_IsPinBall())	//쏘기전,메이가 핸들 잡음
	{
		if (m_fStartPinBallBezierTime > 0.f)
		{
			m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.65f, 1.f), XMVectorSet(0.65f, 0.f, 0.35f, 1.f),1.3f);
			m_fStartPinBallBezierTime = 0.f;
		}
		_vector vTargetPos = vPinBallPos + XMVectorSet(0.f, 1.5f, 2.8f, 0.f);
		_vector vAt = vPinBallPos + XMVectorSet(0.f, 0.75f, 0.f, 0.f);
		if(XMVectorGetX(XMVector3Length(vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) > 0.01f)
			m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vTargetPos, vAt), (_float)dTimeDelta * 2.f));
	}
	else if (false == pPinBall->Get_Failed() && false ==  pPinBallHandle->Get_Goal() && m_pCody->Get_IsPinBall())	//가는중
	{
		_vector vTargetPos = vPinBallPos + XMVectorSet(0.f, 1.5f, 2.8f, 0.f);
		_vector vAt = vPinBallPos + XMVectorSet(0.f, 0.75f, 0.f, 0.f);

		m_pTransformCom->Set_WorldMatrix(MakeViewMatrixByUp(vTargetPos, vAt));

	}
	else if (pPinBall->Get_Failed() && m_pCody->Get_IsPinBall())	//실패
	{
		if (0.f == m_fStartPinBallBezierTime)
		{
			m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
		}
		m_fStartPinBallBezierTime += (_float)dTimeDelta;
		if (m_fStartPinBallBezierTime >= 0.2f)
		{
			if (UI_Generator->Get_EmptyCheck(Player::Cody, UI::BlackScreenFadeInOut))
			{
				UI_CreateOnlyOnce(Cody, BlackScreenFadeInOut);
				UI_Generator->Set_FadeInSpeed(Player::Cody, UI::BlackScreenFadeInOut,5.f);
			}
		}
	}
	else if(pPinBallHandle->Get_Goal() && m_pCody->Get_IsPinBall())//성공
	{
		_vector vFinishEye = XMVectorSet(-672.537f,760.996f, -147.991f,1.f);
		_vector vFinishAt = XMVectorSet(-672.547f, 760.868f,-148.982f, 1.f);
		m_fStartPinBallBezierTime += (_float)dTimeDelta;
		_float fWatingTime = 0.5f;
		if(m_fStartPinBallBezierTime > fWatingTime)
			m_pTransformCom->Set_WorldMatrix(MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(), MakeViewMatrixByUp(vFinishEye, vFinishAt),
				m_fStartPinBallBezierTime - fWatingTime));
		if (m_fStartPinBallBezierTime > 2.5f && UI_Generator->Get_EmptyCheck(Player::Cody, UI::BlackScreenFadeInOut))
		{
			UI_CreateOnlyOnce(Cody, BlackScreenFadeInOut);
			UI_Generator->Set_FadeInSpeed(Player::Cody, UI::BlackScreenFadeInOut, 5.f);
		}
	}
	else
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
		
		ReSet_Cam_FreeToAuto(true);
		m_fStartPinBallBezierTime = 0.f;
		//끝남
	}
	
	
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_WallJump(_double dTimeDelta)
{
	if (m_pCody->Get_IsWallJump() == false)
	{
		ReSet_Cam_FreeToAuto(true);
	}
	CTransform* pPlayerTransform = m_pCody->Get_Transform();

	_matrix matFacetoWall = m_pCody->Get_CameraTrigger_Matrix();
	_vector vProgressDir = matFacetoWall.r[1];
	_vector vTriggerPos = m_pCody->Get_CamTriggerPos();

	_float fAxisX = XMVectorGetX(vProgressDir);
	_float fAxisY = XMVectorGetY(vProgressDir);
	_float fAxisZ = XMVectorGetZ(vProgressDir);

	_float fMax = fmax(fmax(fAxisX, fAxisY), fAxisZ);
	_vector vPlayerPos = m_pCody->Get_Position();
	_vector vPlayerScale = XMVectorSet(pPlayerTransform->Get_Scale(CTransform::STATE_RIGHT),
		pPlayerTransform->Get_Scale(CTransform::STATE_UP), pPlayerTransform->Get_Scale(CTransform::STATE_LOOK), 0.f);
	_vector vEye = matFacetoWall.r[3];
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

_int CMainCamera::Tick_Cam_LaserTennis(_double dTimeDelta)
{

	if (CLaserTennis_Manager::GetInstance()->Get_StartGame() == false)
	{
		if (CLaserTennis_Manager::GetInstance()->Get_Winner() == CLaserTennis_Manager::TARGET_MAY)
		{
			if (UI_Generator->Get_EmptyCheck(Player::Cody, UI::BlackScreenFadeInOut))
			{
				UI_CreateOnlyOnce(Cody, BlackScreenFadeInOut);
				UI_Generator->Set_FadeInSpeed(Player::Cody, UI::BlackScreenFadeInOut, 5.f);
			}
			if (static_cast<CAlphaScreen*>(UI_Generator->Get_UIObject(Player::Cody, UI::BlackScreenFadeInOut))->Get_Alpha() >= 1.f)
			{
				ReSet_Cam_FreeToAuto(true);
				((CSubCamera*)(DATABASE->Get_SubCam()))->ReSet_Cam_FreeToAuto(true);
			}
		}
		return NO_EVENT;
	}
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	_vector vCodyPos = XMVectorSetY(m_pCody->Get_Position(), 730.f);
	_vector vMayPos = XMVectorSetY(pMay->Get_Position(), 730.f);
	_vector vTargetPos = XMVectorSet(60.479f,753.281f,984.f,1.f);

	_vector vMiddlePos = vCodyPos - (vCodyPos - vMayPos)*0.5f;

	vMiddlePos = XMVectorSetY(vMiddlePos, 730.f);
	vMiddlePos = XMVectorSetZ(vMiddlePos, 995.f);

	
	vTargetPos = XMVectorSetX(vTargetPos, XMVectorGetX(vMiddlePos));
	m_pTransformCom->Set_WorldMatrix(
		MakeLerpMatrix(m_pTransformCom->Get_WorldMatrix(),MakeViewMatrixByUp(vTargetPos,vMiddlePos),(_float)dTimeDelta));
	return NO_EVENT;
}

_int CMainCamera::Tick_Cam_Boss_HitRocket(_double dTimeDelta)
{
	return NO_EVENT;
}


_int CMainCamera::ReSet_Cam_FreeToAuto(_bool bCalculatePlayerLook, _bool bIsCalculateCamLook, _float fLerpSpeed)
{

	m_fChangeCamModeLerpSpeed = fLerpSpeed;
	m_fChangeCamModeTime = 0.f;
	m_eCurCamMode = CamMode::Cam_AutoToFree;
	m_eCurPlayerSize = m_pCody->Get_Player_Size();
	for (_uint i = 0; i < Rev_End; i++)
	{
		m_fMouseRev[i] = 0.f;
		m_fCurMouseRev[i] = 0.f;
	}
	if (bCalculatePlayerLook)
	{
		_vector vOriginAxis =
			bIsCalculateCamLook ? m_pTransformCom->Get_State(CTransform::STATE_LOOK) : XMVectorSet(0.f, 0.f, 1.f, 0.f);
		_vector vRotAxis = XMVector3Normalize(m_pCody->Get_Transform()->Get_State(CTransform::STATE_LOOK));
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

_int CMainCamera::ReSet_Cam_Free_OnRail()
{
	for (_uint i = 0; i < 4; i++)
		m_iNodeIdx[i] = i;
	m_vCurRailAt = m_CamNodes.front()->vEye;
	m_fRailProgressTime = 0.f;
	m_bStartOnRail = false;
	
	return NO_EVENT;
}

_bool CMainCamera::OffSetPhsX(_fvector vEye, _fvector vAt, _double dTimeDelta,_vector * pOut)
{

	_bool isHit = false;


	_vector vDistanceFromCam = vEye - vAt;
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

_fmatrix CMainCamera::MakeViewMatrix_FollowPlayer(_double dTimeDelta)
{
	CTransform* pPlayerTransform = m_pCody->Get_Transform();

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

	_vector vTargetPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);
	_vector vPlayerUp = XMLoadFloat4(&m_vPlayerUp);
	_vector vUpDir = (vTargetPlayerUp - vPlayerUp);
	if (XMVectorGetX(XMVector4Length(vUpDir)) > 0.01f)
		vPlayerUp += vUpDir * (_float)dTimeDelta * 5.f;
	vPlayerUp = (m_pCody->Get_IsInGravityPipe() || m_pCody->Get_IsInRocket())? XMVectorSet(0.f, 1.f, 0.f, 0.f) : vPlayerUp;

	XMStoreFloat4(&m_vPlayerUp, vPlayerUp);

	_vector vPrePlayerPos = XMLoadFloat4(&m_vPlayerPos);
	if (XMVectorGetX(XMVectorIsNaN(vPrePlayerPos)))
		vPrePlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCurPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vCurPlayerPos = XMVectorSetY(vCurPlayerPos, XMVectorGetY(vCurPlayerPos) + m_vSizeAt[m_eCurPlayerSize].y);

	_vector vPlayerPos = vCurPlayerPos;
	_float fDist = fabs(XMVectorGetX(XMVector4Length(vPrePlayerPos - vCurPlayerPos)));

	vPlayerPos = fDist > 10.f ? vCurPlayerPos : XMVectorLerp(vPrePlayerPos, vCurPlayerPos, XMVectorGetX(XMVector4Length(vCurPlayerPos - vPrePlayerPos))*(_float)dTimeDelta * 10.f);



	XMStoreFloat4(&m_vPlayerPos, vPlayerPos);

	//회전 보간(마우스)

	_vector vCurQuartRot = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fCurMouseRev[Rev_Prependicul]),
		XMConvertToRadians(m_fCurMouseRev[Rev_Holizontal]), 0.f);


	_vector vScale, vRotQuat, vTrans;
	_vector  vCurRotQuat, vCurTrans;



	
		XMMatrixDecompose(&vScale, &vRotQuat, &vTrans, XMLoadFloat4x4(&m_matBeginWorld) *
			XMMatrixRotationQuaternion(vCurQuartRot)*MH_RotationMatrixByUp(vPlayerUp, vPlayerPos));
	
	

	_vector vPreQuat = XMLoadFloat4(&m_PreWorld.vRotQuat);
	_vector vPreTrans = XMLoadFloat4(&m_PreWorld.vTrans);

	vCurRotQuat = XMQuaternionSlerp(vPreQuat, vRotQuat, 0.5f);
	vCurTrans = XMVectorLerp(vPreTrans, vTrans, 0.5f);

	XMStoreFloat4(&m_PreWorld.vRotQuat, vCurRotQuat);
	XMStoreFloat4(&m_PreWorld.vTrans, vCurTrans);

	_matrix matAffine = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		vCurRotQuat, vCurTrans);


	if (m_pCamHelper->Get_IsCamEffectPlaying(CFilm::LScreen))
	{
		if (m_pCamHelper->Tick_CamEffect(CFilm::LScreen, dTimeDelta, matAffine))
			matAffine = m_pCamHelper->Get_CurApplyCamEffectMatrix(CFilm::LScreen);
		vPlayerUp = XMVector3TransformNormal(vPlayerUp, matAffine);

	}

	_vector vResultPos = matAffine.r[3];
	OffSetPhsX(matAffine.r[3], vPlayerPos, dTimeDelta, &vResultPos);

	
	return MakeViewMatrixByUp(vResultPos, vPlayerPos, vPlayerUp/*matAffine.r[2]*/);
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
	/*if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
	{
		m_pCamHelper->Start_CamEffect(L"Cam_Shake_MissileBoom", CFilm::LScreen);
		return NO_EVENT;
	}*/
	/*if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_pCamHelper->Start_Film(L"Film_Clear_Umbrella",CFilm::LScreen);
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 1.f, 1.f));
		return NO_EVENT;
	}*/
	
	//if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	//{
	//	CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Eject_InUFO"));
	//	Start_Film(L"Film_Eject_InUFO");
	//	//CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Boss_Intro");
	//	return NO_EVENT;
	//}

	/*if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		CCutScenePlayer::GetInstance()->Stop_CutScene();
		return NO_EVENT;
	}*/
	if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
	{
		CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_GotoMoon"));

		//CCutScenePlayer::GetInstance()->Start_CutScene(L"CutScene_Boss_Intro");
		return NO_EVENT;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 1.f, 1.f));
		m_eCurCamFreeOption = CamFreeOption::Cam_Free_FreeMove;
	}

#endif
	
	if (m_eCurCamMode != m_ePreCamMode)
	{
		switch (m_eCurCamMode)
		{
		case Client::CMainCamera::CamMode::Cam_Free:
			if (false == UI_Generator->Get_EmptyCheck(Player::Cody, UI::BlackScreenFadeInOut))
			{
				UI_Generator->Set_FadeOut(Player::Cody, UI::BlackScreenFadeInOut);
				UI_Generator->Set_FadeOutSpeed(Player::Cody, UI::BlackScreenFadeInOut, 6.f);
			}
			break;
		case Client::CMainCamera::CamMode::Cam_AutoToFree:
			switch (m_ePreCamMode)
			{
			case Client::CMainCamera::CamMode::Cam_PinBall_Cody:
				if (false == UI_Generator->Get_EmptyCheck(Player::Cody, UI::BlackScreenFadeInOut))
				{
					UI_Generator->Set_FadeOut(Player::Cody, UI::BlackScreenFadeInOut);
					UI_Generator->Set_FadeOutSpeed(Player::Cody, UI::BlackScreenFadeInOut, 6.f);
				}
				m_pTransformCom->Set_WorldMatrix(MakeViewMatrix_FollowPlayer(dTimeDelta));
				break;
			}
			break;
		case Client::CMainCamera::CamMode::Cam_Warp_WormHole:
			break;
		case Client::CMainCamera::CamMode::Cam_PressButton_Bridge:
			break;
		case Client::CMainCamera::CamMode::Cam_InJoyStick:
			break;
		case Client::CMainCamera::CamMode::Cam_PinBall_Cody:
			break;
		}
		m_ePreCamMode = m_eCurCamMode;
	}
	if (m_eCurCamFreeOption != m_ePreCamFreeOption)
	{
		switch (m_eCurCamFreeOption)
		{
		case Client::CMainCamera::CamFreeOption::Cam_Free_FollowPlayer:
			ReSet_Cam_FreeToAuto();
			break;
		case Client::CMainCamera::CamFreeOption::Cam_Free_FreeMove:
			break;
		case Client::CMainCamera::CamFreeOption::Cam_Free_OnBossMiniRoom_Cody:
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
	case Client::CMainCamera::CamMode::Cam_Ending:
		iResult = Tick_Cam_Ending(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_Warp_WormHole:
		iResult = Tick_Cam_Warp_WormHole(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_PressButton_Bridge:
		iResult = Tick_Cam_PressButton_Bridge(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_InJoyStick:
		iResult = Tick_Cam_InJoystick(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_PinBall_Cody:
		iResult = Tick_Cam_PinBall_Cody(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_WallJump:
		iResult = Tick_Cam_WallJump(dTimeDelta);
		break;
	case Client::CMainCamera::CamMode::Cam_LaserTennis:
		iResult = Tick_Cam_LaserTennis(dTimeDelta);
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

	m_CamNodes.clear();
	Safe_Release(m_pCody);
	Safe_Release(m_pCamHelper);

	CCamera::Free();

}
