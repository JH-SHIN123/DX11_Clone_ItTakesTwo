#include "stdafx.h"
#include "..\public\Cam_Helper.h"
#include"Transform.h"
#include"GameInstance.h"
CCam_Helper::CCam_Helper(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context)
	:CComponent(pDevice, pDevice_Context)
{
}

CCam_Helper::CCam_Helper(const CCam_Helper & rhs)
	: CComponent(rhs)
{
}

HRESULT CCam_Helper::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();
#pragma region CAM_SHAKE_BASIC
	
	CCamEffect* pShake_Loc_Right = CCamEffect::Create(TEXT("Cam_Shake_Loc_Right"));
	{
		_double dDuration = 5.0;
		pShake_Loc_Right->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right].dMinForce = -1.0 / (i + 1) * 2.0;
	
			pShake_Loc_Right->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Loc_Right"), pShake_Loc_Right);
	
	CCamEffect* pShake_Loc_Up = CCamEffect::Create(TEXT("Cam_Shake_Loc_Up"));
	{
		_double dDuration = 5.0;
		pShake_Loc_Up->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMinForce = -1.0 / (i + 1);
	
			pShake_Loc_Up->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Loc_Up"), pShake_Loc_Up);
	
	CCamEffect* pShake_Loc_Look = CCamEffect::Create(TEXT("Cam_Shake_Loc_Look"));
	{
		_double dDuration = 5.0;
		pShake_Loc_Look->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look].dMaxForce = 0.3 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look].dMinForce = -0.3 / (i + 1);
	
			pShake_Loc_Look->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Loc_Look"), pShake_Loc_Look);
	
	CCamEffect* pShake_Rot_Right = CCamEffect::Create(TEXT("Cam_Shake_Rot_Right"));
	{
		_double dDuration = 5.0;
		pShake_Rot_Right->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right].dMaxForce = 0.1 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right].dMinForce = -0.1 / (i + 1);
	
			/*		pCycleDesc->bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Up].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Up].dMinForce = -1.0 / (i + 1);
	
			pCycleDesc->bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Look] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Look].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Look].dMinForce = -1.0 / (i + 1);*/
	
	
			pShake_Rot_Right->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Rot_Right"), pShake_Rot_Right);
	
	CCamEffect* pShake_Rot_Up = CCamEffect::Create(TEXT("Cam_Shake_Rot_Up"));
	{
		_double dDuration = 5.0;
		pShake_Rot_Up->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMaxForce = 0.1 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMinForce = -0.1 / (i + 1);
	
			pShake_Rot_Up->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Rot_Up"), pShake_Rot_Up);
	
	CCamEffect* pShake_Rot_Look = CCamEffect::Create(TEXT("Cam_Shake_Rot_Look"));
	{
		_double dDuration = 7.0;
		pShake_Rot_Look->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look].dMaxForce = 0.12 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look].dMinForce = -0.12 / (i + 1);
	
			pShake_Rot_Look->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Rot_Look"), pShake_Rot_Look);
	
	CCamEffect* pShake_Loc_Right_Warp_Potal = CCamEffect::Create(TEXT("Cam_Shake_Loc_Right_Warp_Potal"));
	{
		_double dDuration = 1.0;
		pShake_Loc_Right_Warp_Potal->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 0.25)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.125;
			pCycleDesc->dFinishTime = i + 0.25;

			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right].dMaxForce = 0.01;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Right].dMinForce = -0.01;

			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMaxForce = 0.01 / (i + 1.0);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMinForce = -0.01 / (i + 1.0);


			pShake_Loc_Right_Warp_Potal->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Loc_Right_Warp_Potal"), pShake_Loc_Right_Warp_Potal);


	CCamEffect* pShake_Rot_Look_Loc_Up = CCamEffect::Create(TEXT("Cam_Shake_Rot_Look_Loc_Up"));
	{
		_double dDuration = 5.0;
		pShake_Rot_Look_Loc_Up->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMinForce = -1.0 / (i + 1);
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look].dMaxForce = 0.1 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Look].dMinForce = -0.1 / (i + 1);
	
			pShake_Rot_Look_Loc_Up->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Rot_Look_Loc_Up"), pShake_Rot_Look_Loc_Up);
	
	
	CCamEffect* pShake_Rot_Look_Rot_Up = CCamEffect::Create(TEXT("Cam_Shake_Rot_Right_Rot_Up"));
	{
		_double dDuration = 5.0;
		pShake_Rot_Look_Rot_Up->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
	
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMaxForce = 0.1 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Up].dMinForce = -0.1 / (i + 1);
	
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right].dMaxForce = 0.1 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Rot_Right].dMinForce = -0.1 / (i + 1);
	
			pShake_Rot_Look_Rot_Up->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Rot_Right_Rot_Up"), pShake_Rot_Look_Rot_Up);
	
	CCamEffect* pShake_Loc_Look_Loc_Up = CCamEffect::Create(TEXT("Cam_Shake_Loc_Look_Loc_Up"));
	{
		_double dDuration = 5.0;
		pShake_Loc_Look_Loc_Up->Set_Duration(dDuration);
		for (_double i = 0.0; i <= dDuration; i += 1.0)
		{
			CCamEffect::CamShakeCycleDesc* pCycleDesc = new CCamEffect::CamShakeCycleDesc;
			pCycleDesc->dStartTime = i;
			pCycleDesc->dMiddleTime = i + 0.5;
			pCycleDesc->dFinishTime = i + 1.0;
	
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Up].dMinForce = -1.0 / (i + 1);
	
			pCycleDesc->bOnCamShakeOption[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look] = true;
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look].dMaxForce = 1.0 / (i + 1);
			pCycleDesc->tCamShakeDesc[(_uint)CCamEffect::CamShakeOption::CamShake_Loc_Look].dMinForce = -1.0 / (i + 1);
	
			pShake_Loc_Look_Loc_Up->Add_CamShakeCycleDesc(pCycleDesc);
		}
	}
	Add_CamEffect(TEXT("Cam_Shake_Loc_Look_Loc_Up"), pShake_Loc_Look_Loc_Up);
#pragma endregion

		
	
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Begin_Game.dat"))))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Line_BossRoom_MiniCody.dat"),true)))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Active_GravityPath_01.dat"))))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Clear_Umbrella.dat"))))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Clear_Rail.dat"))))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Boss_Intro.dat"))))
		return E_FAIL;
	if (FAILED(Load_Film(TEXT("../Bin/Resources/Data/FilmData/Film_Eject_InUFO.dat"))))
		return E_FAIL;

	return S_OK;
}

CCam_Helper::CamHelperState CCam_Helper::Tick(_double TimeDelta, CFilm::ScreenType eScreenTypeIdx)
{
	if (m_bIsSeeCamNode[eScreenTypeIdx])
		m_eState[eScreenTypeIdx] = CamHelperState::Helper_SeeCamNode;
	else if (m_bIsPlayingAct[eScreenTypeIdx])
	{
		if (!m_pCurFilm[eScreenTypeIdx]->Get_IsEnd(eScreenTypeIdx))
			m_eState[eScreenTypeIdx] = CamHelperState::Helper_Act;
		else
		{
			m_bIsPlayingAct[eScreenTypeIdx] = false;
			m_eState[eScreenTypeIdx] = CamHelperState::Helper_None;
		}
	}
	else
		m_eState[eScreenTypeIdx] = CamHelperState::Helper_None;

	//CamEffect
	if (m_pCurEffect[eScreenTypeIdx])
	{
		if (m_pCurEffect[eScreenTypeIdx]->IsFinish(eScreenTypeIdx))
		{
			m_bIsPlayingCamEffect[eScreenTypeIdx] = false;
		}
	}
	else 	m_bIsPlayingCamEffect[eScreenTypeIdx] = false;
	
	return m_eState[eScreenTypeIdx];
}

HRESULT CCam_Helper::Add_Film(const _tchar * pFilmName, CFilm * pFilm, _double dDuration)
{
	if (pFilm == nullptr ||
		pFilmName == nullptr ||
		nullptr != Find_Film(pFilmName))
		return E_FAIL;

	pFilm->Set_Duration(dDuration);
	for (_uint i = 0; i < 2; i++)
		pFilm->ReSetFilm((CFilm::ScreenType)i);
	m_Films.emplace(FILMS::value_type(pFilmName, pFilm));
	return S_OK;
}

HRESULT CCam_Helper::Add_CamNode(const _tchar * pFilmName, CFilm::CamNode * pNode)
{
	if (nullptr == pNode)
	{
		MSG_BOX("Add Node is Nullptr");
		return E_FAIL;
	}
	CFilm* pFilm = Find_Film(pFilmName);
	if (nullptr == pFilm)
	{
		MSG_BOX("Film is Nullptr");
		return E_FAIL;
	}
	pFilm->Add_Node(pNode);
	return S_OK;
}

CFilm * CCam_Helper::Get_Film(const _tchar * pFilmName)
{
	return Find_Film(pFilmName);
}

_fmatrix CCam_Helper::Tick_Film(_double dTimeDelta, CFilm::ScreenType eScreenTypeIdx, _float* fOutFovY)
{
	if (nullptr == m_pCurFilm[eScreenTypeIdx])
	{
		MSG_BOX("pFilm is nullptr");
		return XMMatrixIdentity();
	}

	m_pCurFilm[eScreenTypeIdx]->Tick_Film(dTimeDelta/*CCutScenePlayer::GetInstance()->Get_TimeDelta() * 0.63f*/, eScreenTypeIdx,fOutFovY);

	return m_pCurFilm[eScreenTypeIdx]->Get_CurNodeMatrix(eScreenTypeIdx	,fOutFovY);
}

void CCam_Helper::Start_Film(const _tchar * pFilmName, CFilm::ScreenType eScreenTypeIdx)
{

	CFilm* pFilm = Find_Film(pFilmName);
	if (nullptr == pFilm)
	{
		MSG_BOX("Not have Film");
		return;
	}
	switch (eScreenTypeIdx)
	{
	case CFilm::LScreen:
	case CFilm::RScreen:
		m_pCurFilm[eScreenTypeIdx] = Find_Film(pFilmName);
		m_pCurFilm[eScreenTypeIdx]->ReSetFilm(eScreenTypeIdx);
		m_bIsPlayingAct[eScreenTypeIdx] = true;
		m_dTime[eScreenTypeIdx] = 0.0;
		break;
	case CFilm::Screen_End:MSG_BOX("ScreenTypeIdx is End");
		return;
	default:MSG_BOX("ScreenTypeIdx is Out of Range");
		return;
	}

}




void CCam_Helper::SeeCamNode(CFilm::CamNode * pCamNode, CFilm::ScreenType eScreenTypeIdx)
{
	if (m_bIsSeeCamNode[eScreenTypeIdx] == true)
		return;
	XMStoreFloat4x4(&m_matCamNode[eScreenTypeIdx], MakeViewMatrix(pCamNode->vEye,pCamNode->vAt));
	m_fCamNodeLerpTime[eScreenTypeIdx] = 0.f;
	m_bIsSeeCamNode[eScreenTypeIdx] = true;
}

_fmatrix CCam_Helper::Get_CamNodeMatrix(CTransform* pCamTransform, _double dTimeDelta, CFilm::ScreenType eScreenTypeIdx)
{

	if (m_fCamNodeLerpTime[eScreenTypeIdx] >= 1.f)
	{
		m_bIsSeeCamNode[eScreenTypeIdx] = false;
		return XMLoadFloat4x4(&m_matCamNode[eScreenTypeIdx]);
	}
	m_fCamNodeLerpTime[eScreenTypeIdx] += (_float)dTimeDelta;
	_matrix matNode = XMLoadFloat4x4(&m_matCamNode[eScreenTypeIdx]);
	_vector vNodePos = matNode.r[3];
	_matrix matWorld = pCamTransform->Get_WorldMatrix();
	_matrix matNext = matNode;

	
	_vector	  vPreRight = matWorld.r[0], vNextRight = matNext.r[0]
		, vPreUp = matWorld.r[1], vNextUp = matNext.r[1]
		, vPreLook = matWorld.r[2], vNextLook = matNext.r[2]
		, vPrePos = matWorld.r[3], vNextPos = matNext.r[3];

	_vector vCurRight = XMVectorLerp(vPreRight, vNextRight, m_fCamNodeLerpTime[eScreenTypeIdx]),
		vCurUp = XMVectorLerp(vPreUp, vNextUp, m_fCamNodeLerpTime[eScreenTypeIdx]),
		vCurLook = XMVectorLerp(vPreLook, vNextLook, m_fCamNodeLerpTime[eScreenTypeIdx]),
		vCurPos = XMVectorLerp(vPrePos, vNextPos, m_fCamNodeLerpTime[eScreenTypeIdx]);

	_matrix matCurWorld = XMMatrixIdentity();
	matCurWorld.r[0] = vCurRight;
	matCurWorld.r[1] = vCurUp;
	matCurWorld.r[2] = vCurLook;
	matCurWorld.r[3] = vCurPos;


	return matCurWorld;
}

_fmatrix CCam_Helper::Get_CamNodeMatrix(CFilm::CamNode * pCamNode1, CFilm::CamNode * pCamNode2, CFilm::CamNode * pCamNode3, CFilm::CamNode * pCamNode4, _double dTime, _bool* pIsFinishedNode)
{
	if (dTime < pCamNode1->dTime)
		return MakeViewMatrix(pCamNode1->vEye, pCamNode1->vAt);
	

	CFilm::CamMoveOption eEyeMoveOption = pCamNode1->eEyeMoveOption;
	CFilm::CamMoveOption eAtMoveOption = pCamNode1->eAtMoveOption;
	_float fProgress = (_float)((dTime - pCamNode1->dTime) / (pCamNode2->dTime - pCamNode1->dTime));
	if (fProgress >= 1.f)
		*pIsFinishedNode = true;
	_float3 vCurEye;
	_float3 vCurAt ;
	switch (eEyeMoveOption)
	{
	case Client::CFilm::CamMoveOption::Move_Jump:
		vCurEye = pCamNode2->vEye;
		break;
	case Client::CFilm::CamMoveOption::Move_Straight:
		vCurEye = VectorLerp(pCamNode1->vEye, pCamNode2->vEye, fProgress);
		break;
	case Client::CFilm::CamMoveOption::Move_Bezier_3:
		vCurEye = MakeBezier3(pCamNode1->vEye, pCamNode2->vEye, pCamNode3->vEye, fProgress);
		break;
	case Client::CFilm::CamMoveOption::Move_Bezier_4:
		vCurEye = MakeBezier4(pCamNode1->vEye, pCamNode2->vEye, pCamNode3->vEye, pCamNode4->vEye, fProgress);
		break;
	}
	switch (eAtMoveOption)
	{
	case Client::CFilm::CamMoveOption::Move_Jump:
		vCurAt = pCamNode2->vAt;
		break;
	case Client::CFilm::CamMoveOption::Move_Straight:
		vCurAt = VectorLerp(pCamNode1->vAt, pCamNode2->vAt, fProgress);
		break;
	case Client::CFilm::CamMoveOption::Move_Bezier_3:
		vCurAt = MakeBezier3(pCamNode1->vAt, pCamNode2->vAt, pCamNode3->vAt, fProgress);
		break;
	case Client::CFilm::CamMoveOption::Move_Bezier_4:
		vCurAt = MakeBezier4(pCamNode1->vAt, pCamNode2->vAt, pCamNode3->vAt, pCamNode4->vAt, fProgress);
		break;
	}
	

	return MakeViewMatrix(vCurEye,vCurAt);
}

_fmatrix CCam_Helper::MakeViewMatrix(_float3 Eye, _float3 At)
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

_float3 CCam_Helper::VectorLerp(_float3 vDst, _float3 vSrc, _float fT)
{
	if (fT >= 1.f)
		return vSrc;
		_float3 vResult;
	XMStoreFloat3(&vResult,XMVectorLerp(XMLoadFloat3(&vDst), XMLoadFloat3(&vSrc), fT));
	return vResult;
}

_float3 CCam_Helper::MakeBezier3(_float3 & v1, _float3 & v2, _float3 & v3, _double dTime)
{
	if (dTime >= 1.0)
		return v3;
	_float3 vResult = {
		(_float)(pow((1.0 - dTime),2)*v1.x + 2 * dTime*(1.0 - dTime)*v2.x + pow(dTime,2)*v3.x),
		(_float)(pow((1.0 - dTime),2)*v1.y + 2 * dTime*(1.0 - dTime)*v2.y + pow(dTime,2)*v3.y),
		(_float)(pow((1.0 - dTime),2)*v1.z + 2 * dTime*(1.0 - dTime)*v2.z + pow(dTime,2)*v3.z)
	};
	/*v3.x *pow(fTime, 2) + v2.x *(2 * fTime *(1 - fTime)) + v1.x * pow((1 - fTime),2),
	v3.y *pow(fTime, 2) + v2.y *(2 * fTime *(1 - fTime)) + v1.y * pow((1 - fTime),2),
	v3.z *pow(fTime, 2) + v2.z *(2 * fTime *(1 - fTime)) + v1.z * pow((1 - fTime),2)*/
	return vResult;
}

_float3 CCam_Helper::MakeBezier4(_float3 & v1, _float3 & v2, _float3 & v3, _float3 & v4, _double dTime)
{
	if (dTime >= 1.f)
		return v4;

	_float3 vResult = {
		(_float)(v1.x*pow((1.0 - dTime), 3) + 3 * v2.x*dTime*pow(1.0 - dTime, 2) + 3 * v3.x *pow(dTime, 2)*(1.f - dTime) + v4.x *pow(dTime, 3)),
		(_float)(v1.y*pow((1.0 - dTime), 3) + 3 * v2.y*dTime*pow(1.0 - dTime, 2) + 3 * v3.y *pow(dTime, 2)*(1.f - dTime) + v4.y *pow(dTime, 3)),
		(_float)(v1.z*pow((1.0 - dTime), 3) + 3 * v2.z*dTime*pow(1.0 - dTime, 2) + 3 * v3.z *pow(dTime, 2)*(1.f - dTime) + v4.z *pow(dTime, 3))
	};
	return vResult;
}


_bool CCam_Helper::Tick_CamEffect(CFilm::ScreenType eScreen, _double dTimeDelta, _fmatrix matIn)
{
	if (m_pCurEffect[eScreen] == nullptr)
		return false;
	else
		m_pCurEffect[eScreen]->Tick_CamEffect(eScreen, dTimeDelta, matIn);
	return true;
}

HRESULT CCam_Helper::Add_CamEffect(const _tchar * pCamEffectTag, CCamEffect * pCamEffect)
{
	if (nullptr == pCamEffect ||
		nullptr == pCamEffectTag ||
		nullptr != Find_CamEffect(pCamEffectTag))
		return E_FAIL;

	m_CamEffects.emplace(CAMEFFECTS::value_type(pCamEffectTag, pCamEffect));

	return S_OK;
}

HRESULT CCam_Helper::Add_CamShakeCycleDesc(const _tchar * pCamEffectTag, CCamEffect::CamShakeCycleDesc * pDesc)
{
	if (nullptr == pCamEffectTag ||
		nullptr == pDesc)
		return E_FAIL;
	CCamEffect* pCamEffect = Find_CamEffect(pCamEffectTag);
	if (nullptr == pCamEffect)
		return E_FAIL;
	pCamEffect->Add_CamShakeCycleDesc(pDesc);
	return S_OK;
}

HRESULT CCam_Helper::Start_CamEffect(const _tchar * pEffectName, CFilm::ScreenType eScreen)
{
	if (m_bIsPlayingCamEffect[eScreen])
		return E_FAIL;
	CCamEffect* pCamEffect = Find_CamEffect(pEffectName);
	if (nullptr == pCamEffect)
		return E_FAIL;
	m_pCurEffect[eScreen] = pCamEffect;
	pCamEffect->ReSet_Effect(eScreen);
	m_bIsPlayingCamEffect[eScreen] = true;
	return S_OK;
}

_fmatrix CCam_Helper::Get_CurApplyCamEffectMatrix(CFilm::ScreenType eScreen)
{
	return m_pCurEffect[eScreen]->Get_ApplyEffectMatrix(eScreen);
}

CCamEffect * CCam_Helper::Find_CamEffect(const _tchar * pCamEffectName)
{
	auto& pCamEffect = find_if(m_CamEffects.begin(), m_CamEffects.end(), CTagFinder(pCamEffectName));
	if (pCamEffect == m_CamEffects.end())
		return nullptr;
	return pCamEffect->second;
}


CFilm * CCam_Helper::Find_Film(const _tchar * pFilm)
{
	auto& iter = find_if(m_Films.begin(), m_Films.end(), CTagFinder(pFilm));
	if (iter == m_Films.end())
		return nullptr;

	return iter->second;
}

HRESULT CCam_Helper::Load_Film(const _tchar * pDataPath, _bool bMakeUpNodesTimeByfar)
{

		HANDLE hFile = CreateFile(pDataPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR szLoadFilmName[MAX_PATH] = L"";
		ReadFile(hFile, szLoadFilmName, dwStrByte, &dwByte, nullptr);
		CFilm* pLoadFilm = CFilm::Create(szLoadFilmName);
		_double dDuration = 0.0;
		ReadFile(hFile, &dDuration, sizeof(_double), &dwByte, nullptr);
		pLoadFilm->Set_Duration(dDuration);
		while (true) //ReadNode
		{
			CFilm::CamNode tDesc;
			ReadFile(hFile, &tDesc.dTime, sizeof(_double), &dwByte, nullptr);

			ReadFile(hFile, &tDesc.vEye, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &tDesc.vAt, sizeof(_float3), &dwByte, nullptr);

			ReadFile(hFile, &tDesc.eEyeMoveOption, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &tDesc.eAtMoveOption, sizeof(_uint), &dwByte, nullptr);

			ReadFile(hFile, &tDesc.fTargetViewPortCenterX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &tDesc.fTargetViewPortCenterY, sizeof(_float), &dwByte, nullptr);

			ReadFile(hFile, &tDesc.fViewPortLerpSpeed, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &tDesc.eViewPortOption, sizeof(_uint), &dwByte, nullptr);

			ReadFile(hFile, &tDesc.fFovY, sizeof(_float), &dwByte, nullptr);


			if (0 == dwByte)
				break;

		

			pLoadFilm->Add_Node(new CFilm::CamNode(tDesc));
		}
		if (FAILED(Add_Film(pLoadFilm->Get_Name(),pLoadFilm,pLoadFilm->Get_Duration())))
		{
			Safe_Release(pLoadFilm);
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (true == bMakeUpNodesTimeByfar)
		{
			MakeUpNodesTimeByFar(pLoadFilm);
		}
		CloseHandle(hFile);
	
	return S_OK;
}

void CCam_Helper::MakeUpNodesTimeByFar(CFilm * pFilm)
{
	pFilm->MakeUpNodesTimeByFar();
}

CCam_Helper * CCam_Helper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context)
{
	CCam_Helper* pInstance = new CCam_Helper(pDevice, pDevice_Context);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Create CCamHelper");
	}
	return pInstance;
}

CComponent * CCam_Helper::Clone_Component(void * pArg)
{
	Safe_AddRef(this);
	return this;
}

void CCam_Helper::Free()
{
	for (auto& rPair : m_Films)
		Safe_Release(rPair.second);
	m_Films.clear();
	for (auto& rPair : m_CamEffects)
		Safe_Release(rPair.second);
	m_CamEffects.clear();

	CComponent::Free();
}

