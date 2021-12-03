#include "stdafx.h"
#include "GrindLine.h"
#include"Graphic_Device.h"


CGrindLine::CGrindLine()
{
}

HRESULT CGrindLine::NativeConstruct(const _tchar * pGrindLineName)
{
	if (nullptr == pGrindLineName
		|| 0 == lstrlen(pGrindLineName))
		return E_FAIL;

	lstrcpy(m_szGrindLineName, pGrindLineName);

	for (_uint i = 0; i < 2; i++)
		ReSetGrindLine((ScreenType)i);



	return S_OK;
}

HRESULT CGrindLine::Add_Node(GrindLineNode * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_GrindLineNodes.emplace_back(pNode);

	return S_OK;
}

HRESULT CGrindLine::Tick_GrindLine(_double dTimeDelta, ScreenType eScreenType)
{
	//카메라 매트릭스 갱신, 뷰포트 갱신
	m_dProgressTime[eScreenType] += dTimeDelta;
	if (m_dProgressTime[eScreenType] >= m_dDuration)
	{
		m_dProgressTime[eScreenType] = m_dDuration;
		m_bIsEnd[eScreenType] = true;
	}
	else
		m_bIsEnd[eScreenType] = false;


	if (m_bIsEnd[eScreenType] == true)
	{
		//끝났으면 마지막노드로 매트릭스 만들기
		XMStoreFloat4x4(&m_matCurProgress[eScreenType], MakeCurProgressMatrix(dTimeDelta, m_GrindLineNodes.back(), eScreenType));
	}
	else
	{

		GrindLineNode* pFirst = m_GrindLineNodes.front();
		GrindLineNode* pLast = m_GrindLineNodes.back();
		if (m_dProgressTime[eScreenType] < pFirst->dTime)
		{
			XMStoreFloat4x4(&m_matCurProgress[eScreenType], MakeCurProgressMatrix(dTimeDelta, pFirst, eScreenType));
		}
		else if (m_dProgressTime[eScreenType] >= pLast->dTime)
		{
			XMStoreFloat4x4(&m_matCurProgress[eScreenType], MakeCurProgressMatrix(dTimeDelta, pLast, eScreenType));
		}
		else
		{
			if (m_dProgressTime[eScreenType] > m_GrindLineNodes[m_iCurrentNode[eScreenType] + 1]->dTime)
			{
				m_iCurrentNode[eScreenType]++;
				m_bCurNodeEnd[eScreenType] = true; //새로운 노드.
			}

			GrindLineNode* pCurNode = m_GrindLineNodes[m_iCurrentNode[eScreenType]];


			//진행에따른 매트릭스 설정
			XMStoreFloat4x4(&m_matCurProgress[eScreenType], MakeCurProgressMatrix(dTimeDelta, pCurNode, eScreenType));
		}

	}
	return S_OK;
}


_fmatrix CGrindLine::Get_CurNodeMatrix(ScreenType eScreenType)
{
	return XMLoadFloat4x4(&m_matCurProgress[eScreenType]);
}

_fmatrix CGrindLine::MakeCurProgressMatrix(_double dTimeDelta, GrindLineNode * pCurNode, ScreenType eScreenType)
{
	//처음노드면 바로이동
	_uint iCurrentNode = m_iCurrentNode[eScreenType];
	_uint iLastNode = (_uint)m_GrindLineNodes.size();


	_bool bIsLastNode = false;
	if (iCurrentNode + 1 >= iLastNode)
		bIsLastNode = true;


	//다음노드 있으면 다음 노드 없으면 현재꺼 중복
	GrindLineNode tCurNodeDesc = *pCurNode, tNextNodeDesc = bIsLastNode ? *pCurNode : *m_GrindLineNodes[iCurrentNode + 1];


	_float3 vCurNodePos = tCurNodeDesc.vPos, vCurNodeLook = tCurNodeDesc.vLook
		, vNextNodePos = tNextNodeDesc.vPos, vNextNodeLook = tNextNodeDesc.vLook;


	if (m_bCurNodeEnd[eScreenType]) //새로운 노드,변수 초기화
	{
		if (FAILED(Check_GrindNodeProgress(eScreenType, iCurrentNode, iLastNode)))
			return MakeMatrix(vCurNodePos, vCurNodeLook);
	}


	//Pos
	vNextNodePos = VectorLerp(vCurNodePos, vNextNodePos, (_float)m_dMoveTime[eScreenType]);

	//Look
	vNextNodeLook = VectorLerp(vCurNodeLook, vNextNodeLook, (_float)m_dMoveTime[eScreenType]);
	
	_double dPlusTime = dTimeDelta / (tNextNodeDesc.dTime - tCurNodeDesc.dTime);
	m_dMoveTime[eScreenType] += dPlusTime;
	
	return  MakeMatrix(vNextNodePos, vNextNodeLook);

}

void CGrindLine::ReSetGrindLine(ScreenType eScreenType)
{
	m_dProgressTime[eScreenType] = 0.0;
	XMStoreFloat4x4(&m_matCurProgress[eScreenType], XMMatrixIdentity());
	m_iCurrentNode[eScreenType] = 0;
	m_bIsEnd[eScreenType] = false;
	m_bCurNodeEnd[eScreenType] = true;
	m_dMoveTime[eScreenType] = 0.0;
}


CGrindLine * CGrindLine::Create(const _tchar * pGrindLineName)
{
	CGrindLine* pInstance = new CGrindLine;
	if (FAILED(pInstance->NativeConstruct(pGrindLineName)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Create GrindLine");
	}
	return pInstance;
}

void CGrindLine::Free()
{
	for (auto& pCamNodes : m_GrindLineNodes)
		Safe_Delete(pCamNodes);
	m_GrindLineNodes.clear();
}

_fmatrix CGrindLine::MakeMatrix(_float3 Pos, _float3 Look)
{
	_vector	vEye = XMVectorSetW(XMLoadFloat3(&Pos), 1.f);
	_vector	vAt = XMVectorSetW(XMLoadFloat3(&Look), 1.f);
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



_float3 CGrindLine::VectorLerp(_float3& vDst, _float3& vSour, _float t)
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

HRESULT CGrindLine::Check_GrindNodeProgress(ScreenType eScreenType, _uint iCurrentNode, _uint iLastNode)
{
	m_dMoveTime[eScreenType] = 0.0;
	m_bCurNodeEnd[eScreenType] = false;
	return S_OK;
}




