#include "stdafx.h"
#include "..\public\Cam_Helper.h"
#include"Transform.h"


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

#ifdef _DEBUG
	CFilm* pFilm_Eye_Straight = CFilm::Create(TEXT("Eye_Straight"));

	_float3 vAt = { 0.f,4.f,0.f };
	CFilm::CamNode* pNode = new CFilm::CamNode;
	pNode->vEye = { 10.f,4.f, 0.f };
	pNode->vAt = vAt;
	pNode->dTime = 0.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Straight;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Straight->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { 10.f,4.f, 10.f };
	pNode->vAt = vAt;
	pNode->dTime = 1.5;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Straight;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Straight->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { 0.f,4.f, 10.f };
	pNode->vAt = vAt;
	pNode->dTime = 3.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Straight;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Straight->Add_Node(pNode);

	Add_Film(TEXT("Eye_Straight"), pFilm_Eye_Straight, 3.0);

	CFilm* pFilm_Eye_Bezier3 = CFilm::Create(TEXT("Eye_Bezier3"));

	
	pNode = new CFilm::CamNode;
	pNode->vEye = { 10.f,4.f, 0.f };
	pNode->vAt = vAt;
	pNode->dTime = 0.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_3;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Bezier3->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { 10.f,4.f, 10.f };
	pNode->vAt = vAt;
	pNode->dTime = 1.5;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_3;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Bezier3->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { 0.f,4.f, 10.f };
	pNode->vAt = vAt;
	pNode->dTime = 3.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_3;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Straight;
	pFilm_Eye_Bezier3->Add_Node(pNode);

	Add_Film(TEXT("Eye_Bezier3"), pFilm_Eye_Bezier3, 3.0);

	CFilm* pFilm_Eye_Bezier4 = CFilm::Create(TEXT("Eye_Bezier4"));

	pNode = new CFilm::CamNode;
	pNode->vEye = { 10.f,4.f, -10.f };
	pNode->vAt = vAt;
	pNode->dTime = 0.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_4;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Jump;
	pFilm_Eye_Bezier4->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { -30.f,4.f, 30.f };
	pNode->vAt = vAt;
	pNode->dTime = 1.5;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_4;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Jump;
	pFilm_Eye_Bezier4->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { 30.f,4.f, 30.f };
	pNode->vAt = vAt;
	pNode->dTime = 3.0;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_4;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Jump;
	pFilm_Eye_Bezier4->Add_Node(pNode);

	pNode = new CFilm::CamNode;
	pNode->vEye = { -10.f,4.f, -10.f };
	pNode->vAt = vAt;
	pNode->dTime = 4.5;
	pNode->eEyeMoveOption = CFilm::CamMoveOption::Move_Bezier_4;
	pNode->eAtMoveOption = CFilm::CamMoveOption::Move_Jump;
	pFilm_Eye_Bezier4->Add_Node(pNode);

	Add_Film(TEXT("Eye_Bezier4"), pFilm_Eye_Bezier4, 4.5);
#endif


	
	return S_OK;
}

CCam_Helper::CamHelperState CCam_Helper::Tick(_double TimeDelta, CFilm::ScreenType eScreenTypeIdx)
{
	if (m_bIsPlayingAct[eScreenTypeIdx])
	{
		if (!m_pCurFilm[eScreenTypeIdx]->Get_IsEnd(eScreenTypeIdx))
			m_eState[eScreenTypeIdx] = CamHelperState::Helper_Act;
		else
		{
			m_bIsPlayingAct[eScreenTypeIdx] = false;
			m_eState[eScreenTypeIdx] = CamHelperState::Helper_None;
		}
	}
	else if (m_bIsSeeCamNode[eScreenTypeIdx])
		m_eState[eScreenTypeIdx] = CamHelperState::Helper_SeeCamNode;
	else
		m_eState[eScreenTypeIdx] = CamHelperState::Helper_None;
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

_fmatrix CCam_Helper::Tick_Film(_double dTimeDelta, CFilm::ScreenType eScreenTypeIdx)
{
	if (nullptr == m_pCurFilm[eScreenTypeIdx])
	{
		MSG_BOX("pFilm is nullptr");
		return XMMatrixIdentity();
	}
	m_pCurFilm[eScreenTypeIdx]->Tick_Film(dTimeDelta, eScreenTypeIdx);

	return m_pCurFilm[eScreenTypeIdx]->Get_CurNodeMatrix(eScreenTypeIdx);
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
	case CFilm::ScreenType_End:MSG_BOX("ScreenTypeIdx is End");
		return;
	default:MSG_BOX("ScreenTypeIdx is Out of Range");
		return;
	}

}




void CCam_Helper::SeeCamNode(CFilm::CamNode * pCamNode, CFilm::ScreenType eScreenTypeIdx)
{
	XMStoreFloat4x4(&m_matCamNode[eScreenTypeIdx], MakeViewMatrix(pCamNode->vEye,pCamNode->vAt));
	m_fCamNodeLerpTime[eScreenTypeIdx] = 0.f;
	m_bIsSeeCamNode[eScreenTypeIdx] = true;
}

_fmatrix CCam_Helper::Get_CamNodeMatrix(CTransform* pCamTransform, _double dTimeDelta, CFilm::ScreenType eScreenTypeIdx)
{
	m_fCamNodeLerpTime[eScreenTypeIdx] += dTimeDelta;
	_matrix matNode = XMLoadFloat4x4(&m_matCamNode[eScreenTypeIdx]);
	_vector vNodePos = matNode.r[3];
	m_fCamNodeLerpTime[eScreenTypeIdx] += dTimeDelta;
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


	if (m_fCamNodeLerpTime[eScreenTypeIdx] >= 1.f)
	{
		m_bIsSeeCamNode[eScreenTypeIdx] = false;
	}
	return matCurWorld;
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




CFilm * CCam_Helper::Find_Film(const _tchar * pFilm)
{
	auto& iter = find_if(m_Films.begin(), m_Films.end(), CTagFinder(pFilm));
	if (iter == m_Films.end())
		return nullptr;

	return iter->second;
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

}

