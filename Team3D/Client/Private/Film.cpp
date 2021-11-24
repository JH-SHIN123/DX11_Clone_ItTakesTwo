#include "stdafx.h"
#include "Film.h"
#include"Graphic_Device.h"
#include"GameInstance.h"

CFilm::CFilm()
{
}

HRESULT CFilm::NativeConstruct(const _tchar * pFilmName)
{
	if (nullptr == pFilmName
		|| 0 == lstrlen(pFilmName))
		return E_FAIL;

	lstrcpy(m_szFilmName, pFilmName);
	
	for (_uint i = 0; i < 2; i++)
		ReSetFilm((ScreenType)i);
	return S_OK;
}



HRESULT CFilm::Add_Node(CamNode * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;
	
	m_CamNodes.emplace_back(pNode);

	return S_OK;
}

HRESULT CFilm::Tick_Film(_double dTimeDelta, ScreenType eScreenType)
{
	//카메라 매트릭스 갱신, 뷰포트 갱신
	m_dTime[eScreenType] += dTimeDelta;
	if (m_dTime[eScreenType] >= m_dDuration)
	{
		m_dTime[eScreenType] = m_dDuration;
		m_bIsEnd[eScreenType] = true;
	}
	else
		m_bIsEnd[eScreenType] = false;


	if (m_bIsEnd[eScreenType] == true)
	{
		//끝났으면 마지막노드로 매트릭스 만들기
		XMStoreFloat4x4(&m_matCam[eScreenType], MakeCurCamMatrix(dTimeDelta, m_CamNodes.back(), eScreenType));
	}
	else
	{

		CamNode* pFirst = m_CamNodes.front();
		CamNode* pLast = m_CamNodes.back();
		if (m_dTime[eScreenType] < pFirst->dTime)
		{
			XMStoreFloat4x4(&m_matCam[eScreenType], MakeCurCamMatrix(dTimeDelta, pFirst, eScreenType));
		}
		else if (m_dTime[eScreenType] >= pLast->dTime)
		{
			XMStoreFloat4x4(&m_matCam[eScreenType], MakeCurCamMatrix(dTimeDelta, pLast, eScreenType));
		}
		else
		{
			if (m_dTime[eScreenType] > m_CamNodes[m_iCurrentNode[eScreenType] + 1]->dTime)
			{
				m_iCurrentNode[eScreenType]++;
				for (_uint i = 0; i < 2; i++)
					m_bCurNodeEnd[eScreenType][i] = true; //새로운 노드.
			}
		
			//여기서 뷰포트 설정,매트릭스 만들기.
			CamNode* pCurNode = m_CamNodes[m_iCurrentNode[eScreenType]];
			
			m_eCurEye_CamMoveOption = pCurNode->eEyeMoveOption;
			m_eCurAt_CamMoveOption = pCurNode->eAtMoveOption;

			m_eCurViewPortOption = pCurNode->eViewPortOption;
			//뷰매트릭스 설정
			XMStoreFloat4x4(&m_matCam[eScreenType], MakeCurCamMatrix(dTimeDelta, pCurNode, eScreenType));

			//뷰포트 설정
			if (m_eCurViewPortOption != m_ePreViewPortOption)
			{
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				switch (m_eCurViewPortOption)
				{
				case Client::CFilm::ViewPortOption::LScreenType_Split_Immediate:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f),
						XMVectorSet(0.5f, 0.f, 0.5f, 1.f));
					break;
				case Client::CFilm::ViewPortOption::LScreenType_Split_Lerp:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
						XMVectorSet(0.f, 0.f, 1.f, 1.f));
					pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f),
						XMVectorSet(0.5f, 0.f, 0.5f, 1.f), pCurNode->fViewPortLerpSpeed);
					break;
				case Client::CFilm::ViewPortOption::LScreenType_Merge_Immediate:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
						XMVectorSet(0.f, 0.f, 1.f, 1.f));
					break;
				case Client::CFilm::ViewPortOption::LScreenType_Merge_Lerp:
					pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.f, 1.f),
						XMVectorSet(0.f, 0.f, 1.f, 1.f), pCurNode->fViewPortLerpSpeed);
					break;
				case Client::CFilm::ViewPortOption::RScreenType_Split_Immediate:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, pCurNode->fTargetViewPortCenterX, pCurNode->fTargetViewPortCenterY),
						XMVectorSet(pCurNode->fTargetViewPortCenterX, 0.f, pCurNode->fTargetViewPortCenterX, 1.f));
					break;
				case Client::CFilm::ViewPortOption::RScreenType_Split_Lerp:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
						XMVectorSet(1.f, 0.f, 1.f, 1.f));
					pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5f, 1.f),
						XMVectorSet(0.5, 0.f, 0.5f, 1.f), pCurNode->fViewPortLerpSpeed);
					break;
				case Client::CFilm::ViewPortOption::RScreenType_Merge_Immediate:
					pGameInstance->Set_ViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
						XMVectorSet(1.f, 0.f, 1.f, 1.f));
					break;
				case Client::CFilm::ViewPortOption::RScreenType_Merge_Lerp:
					pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 1.f, 1.f),
						XMVectorSet(1.f, 0.f, 1.f, 1.f), pCurNode->fViewPortLerpSpeed);
					break;
				}
				m_ePreViewPortOption = m_eCurViewPortOption;
			}

		}

	}
	return S_OK;
}


_fmatrix CFilm::Get_CurNodeMatrix(ScreenType eScreenType)
{
	return XMLoadFloat4x4(&m_matCam[eScreenType]);
}

_fmatrix CFilm::MakeCurCamMatrix(_double dTimeDelta, CamNode * pCurNode, ScreenType eScreenType)
{
	//처음노드면 바로이동
	_uint iCurrentNode = m_iCurrentNode[eScreenType];
	_uint iLastNode = (_uint)m_CamNodes.size();

	//if (iCurrentNode == 0) //처음꺼
	//	return MakeViewMatrix(pCurNode->GetCamNodeDesc().vEye, pCurNode->GetCamNodeDesc().vAt);

	_bool bIsLastNode = false;
	if (iCurrentNode + 1 >= iLastNode)
		bIsLastNode = true;


	//다음노드 있으면 다음 노드 없으면 현재꺼 중복
	CamNode tCurNodeDesc = *pCurNode,tNextNodeDesc = bIsLastNode ? *pCurNode : *m_CamNodes[iCurrentNode + 1];


	_float3 vCurNodeEye = tCurNodeDesc.vEye, vCurNodeAt = tCurNodeDesc.vAt
		, vNextNodeEye = tNextNodeDesc.vEye, vNextNodeAt = tNextNodeDesc.vAt;


	if (m_bCurNodeEnd[eScreenType][(_uint)CamNodeVectorType::Eye]) //새로운 노드,변수 초기화
	{
		if (FAILED(Check_CamNodeProgress(eScreenType, m_eCurEye_CamMoveOption, CamNodeVectorType::Eye, iCurrentNode, iLastNode)))
			return MakeViewMatrix(vCurNodeEye, vCurNodeAt);
	}
	if (m_bCurNodeEnd[eScreenType][(_uint)CamNodeVectorType::At])
	{
		if (FAILED(Check_CamNodeProgress(eScreenType, m_eCurAt_CamMoveOption, CamNodeVectorType::At, iCurrentNode, iLastNode)))
			return MakeViewMatrix(vCurNodeEye, vCurNodeAt);
	}

	switch (m_eCurEye_CamMoveOption) //Eye
	{
	case CamMoveOption::Move_Jump:
		vNextNodeEye = vCurNodeEye;
		break;
	case CamMoveOption::Move_Straight:
	{
		vNextNodeEye = VectorLerp(vCurNodeEye, vNextNodeEye, (_float)m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Straight]);
		_double dPlusTime = dTimeDelta / (tNextNodeDesc.dTime - tCurNodeDesc.dTime);
		m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Straight] += dPlusTime;
	}
	break;
	case CamMoveOption::Move_Bezier_3:
	{
		CamNode tNode1 = *m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_1]];
		CamNode tNode2 = *m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_2]];
		CamNode tNode3 = *m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_3]];
		vNextNodeEye = MakeBezier3(tNode1.vEye, tNode2.vEye, tNode3.vEye, m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3]);
		if (m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3] >= 1.f)
			m_bCurEye_StartBezier[eScreenType] = false;
		_double dPlusTime = dTimeDelta / (tNode3.dTime - tNode1.dTime);
		m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3] += dPlusTime;
	}
	break;
	case CamMoveOption::Move_Bezier_4:
	{
		CamNode tNode1 =*m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_1]];
		CamNode tNode2 =*m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_2]];
		CamNode tNode3 =*m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_3]];
		CamNode tNode4 =*m_CamNodes[m_iCurEye_BezierNode[eScreenType][Bezier_4]];
		vNextNodeEye = MakeBezier4(tNode1.vEye, tNode2.vEye, tNode3.vEye, tNode4.vEye, m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4]);
		if (m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4] >= 1.f)
			m_bCurEye_StartBezier[eScreenType] = false;
		m_dCamMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4] += dTimeDelta / (tNode4.dTime - tNode1.dTime);
	}
	break;
	}

	switch (m_eCurAt_CamMoveOption) //At
	{
	case CamMoveOption::Move_Jump:
		vNextNodeAt = vCurNodeAt;
		break;
	case CamMoveOption::Move_Straight:
	{
		vNextNodeAt = VectorLerp(vCurNodeAt, vNextNodeAt, (_float)m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Straight]);
		m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Straight] += dTimeDelta / (tNextNodeDesc.dTime - tCurNodeDesc.dTime);
	}
	break;
	case CamMoveOption::Move_Bezier_3:
	{
		CamNode tNode1 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_1]];
		CamNode tNode2 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_2]];
		CamNode tNode3 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_3]];
		vNextNodeAt = MakeBezier3(tNode1.vAt, tNode2.vAt, tNode3.vAt, m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3]);
		if (m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3] >= 1.f)
			m_bCurAt_StartBezier[eScreenType] = false;
		m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_3] += dTimeDelta / (tNode3.dTime - tNode1.dTime);
	}
	break;
	case CamMoveOption::Move_Bezier_4:
	{
		CamNode tNode1 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_1]];
		CamNode tNode2 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_2]];
		CamNode tNode3 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_3]];
		CamNode tNode4 = *m_CamNodes[m_iCurAt_BezierNode[eScreenType][Bezier_4]];
		vNextNodeAt = MakeBezier4(tNode1.vAt, tNode2.vAt, tNode3.vAt, tNode4.vAt, m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4]);
		if (m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4] >= 1.f)
			m_bCurAt_StartBezier[eScreenType] = false;
		m_dCamAtMoveTime[eScreenType][(_uint)CamMoveOption::Move_Bezier_4] += dTimeDelta / (tNode4.dTime - tNode1.dTime);
	}
	break;
	}

	return  MakeViewMatrix(vNextNodeEye, vNextNodeAt);

}

void CFilm::ReSetFilm(ScreenType eScreenType)
{
	m_dTime[eScreenType] = 0.0;
	XMStoreFloat4x4(&m_matCam[eScreenType], XMMatrixIdentity());
	m_iCurrentNode[eScreenType] = 0;
	m_bIsEnd[eScreenType] = false;
	for (_uint i = 0; i < (_uint)CamNodeVectorType::End; i++)
		m_bCurNodeEnd[eScreenType][i] = true;
	for (_uint i = 0; i < 4; i++)
	{
		m_dCamAtMoveTime[eScreenType][i] = 0.0;
		m_dCamMoveTime[eScreenType][i] = 0.0;
	}
}


CFilm * CFilm::Create(const _tchar * pFilmName)
{
	CFilm* pInstance = new CFilm;
	if (FAILED(pInstance->NativeConstruct(pFilmName)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Create Film");
	}
	return pInstance;
}

void CFilm::Free()
{
	for (auto& pCamNodes : m_CamNodes)
		Safe_Delete(pCamNodes);
	m_CamNodes.clear();
}

_fmatrix CFilm::MakeViewMatrix(_float3 Eye, _float3 At)
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

_fmatrix CFilm::MakeMatrixLerp(_fmatrix matDst, _fmatrix matSour, _float fLerpTime)
{
	if (fLerpTime >= 1.f)
		return matSour;
	_vector vDstRight = matDst.r[0], vSourRight = matSour.r[0],
		vDstUp = matDst.r[1], vSourUp = matSour.r[1],
		vDstLook = matDst.r[2], vSourLook = matSour.r[2],
		vDstPos = matDst.r[3], vSourPos = matSour.r[3];

	_matrix matRes(XMVectorLerp(vDstRight, vSourRight, fLerpTime),
		XMVectorLerp(vDstUp, vSourUp, fLerpTime),
		XMVectorLerp(vDstLook, vSourLook, fLerpTime),
		XMVectorLerp(vDstPos, vSourPos, fLerpTime));
	return matRes;
}


_float3 CFilm::VectorLerp(_float3& vDst, _float3& vSour, _float t)
{
	if (t >= 1.f)
		return vSour;
	_vector vDest = XMLoadFloat3(&vDst);
	_vector vSource = XMLoadFloat3(&vSour);
	_vector vResult = XMVectorLerp(vDest, vSource, t);
	_float3 vReturn;
	XMStoreFloat3(&vReturn, vResult);
	return vReturn;
}

void CFilm::ReSet_CamNodeTime_Progress_End(ScreenType eScreenType, CamMoveOption eOption, CamNodeVectorType eType)
{
	switch (eType)
	{
	case CFilm::CamNodeVectorType::Eye:
	{
		m_dCamMoveTime[eScreenType][(_uint)eOption] = 0.0;
		for (_uint i = 0; i < Bezier_End; i++)
			m_iCurEye_BezierNode[eScreenType][i] = -1;
		m_bCurEye_StartBezier[eScreenType] = false;
	}
	break;
	case CFilm::CamNodeVectorType::At:
		m_dCamAtMoveTime[eScreenType][(_uint)eOption] = 0.0;
		for (_uint i = 0; i < Bezier_End; i++)
			m_iCurAt_BezierNode[eScreenType][i] = -1;
		m_bCurAt_StartBezier[eScreenType] = false;
		break;
	}
}

HRESULT CFilm::Check_CamNodeProgress(ScreenType eScreenType, CamMoveOption eOption, CamNodeVectorType eType, _uint iCurrentNode, _uint iLastNode)
{
	switch (eOption)
	{
	case CamMoveOption::Move_Jump:
		ReSet_CamNodeTime_Progress_End(eScreenType, eOption, eType);
		m_bCurNodeEnd[eScreenType][(_uint)eType] = false;
		break;
	case CamMoveOption::Move_Straight:
		ReSet_CamNodeTime_Progress_End(eScreenType, eOption, eType);
		m_bCurNodeEnd[eScreenType][(_uint)eType] = false;
		break;
	case CamMoveOption::Move_Bezier_3:
		if (!m_bCurEye_StartBezier[eScreenType]) //베지어 시작
		{
			ReSet_CamNodeTime_Progress_End(eScreenType, eOption, eType);
			m_bCurEye_StartBezier[eScreenType] = true;
			if (iCurrentNode + 2 > iLastNode)
			{
				MSG_BOX("Not Enough Bezier Node(3)");
				return E_FAIL;
			}
			switch (eType)
			{
			case CFilm::CamNodeVectorType::Eye:
				for (_uint i = 0; i < Bezier_4; i++)
					m_iCurEye_BezierNode[eScreenType][i] = iCurrentNode + i;
				break;
			case CFilm::CamNodeVectorType::At:
				for (_uint i = 0; i < Bezier_4; i++)
					m_iCurAt_BezierNode[eScreenType][i] = iCurrentNode + i;
				break;
			}
			m_bCurNodeEnd[eScreenType][(_uint)eType] = false;
		}
		break;
	case CamMoveOption::Move_Bezier_4:
		if (!m_bCurEye_StartBezier[eScreenType]) //베지어 시작
		{
			ReSet_CamNodeTime_Progress_End(eScreenType, eOption, eType);
			m_bCurEye_StartBezier[eScreenType] = true;
			if (iCurrentNode + 3 > iLastNode)
			{
				MSG_BOX("Not Enough Bezier Node(4)");
				return E_FAIL;
			}
			switch (eType)
			{
			case CFilm::CamNodeVectorType::Eye:
				for (_uint i = 0; i < Bezier_End; i++)
					m_iCurEye_BezierNode[eScreenType][i] = iCurrentNode + i;
				break;
			case CFilm::CamNodeVectorType::At:
				for (_uint i = 0; i < Bezier_End; i++)
					m_iCurAt_BezierNode[eScreenType][i] = iCurrentNode + i;
				break;
			}
			m_bCurNodeEnd[eScreenType][(_uint)eType] = false;
		}
		break;
	}
	return S_OK;
}

_float3 CFilm::MakeBezier3(_float3 & v1, _float3 & v2, _float3 & v3, _double dTime)
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

_float3 CFilm::MakeBezier4(_float3 & v1, _float3 & v2, _float3 & v3, _float3 & v4, _double dTime)
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


