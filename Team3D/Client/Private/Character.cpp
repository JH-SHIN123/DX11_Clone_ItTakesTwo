#include "stdafx.h"
#include "..\Public\Character.h"
#include "SpaceRail.h"
#include "SpaceRail_Node.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{
}

void CCharacter::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CCharacter::Set_Position(_fvector WorldPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldPos);
}
_fvector CCharacter::Get_Position()
{
	if (nullptr == m_pTransformCom)
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);

	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

_float CCharacter::Compute_Distance(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vTargetPos - vPlayerPos;
	_float vDist = fabs(XMVectorGetX(XMVector3Length(vDir)));

	return vDist;
}

_float CCharacter::Compute_Degree(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = pDstTransform->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vPlayerPos - vTargetPos;
	vDir = XMVector3Normalize(vDir);
	_float Cos = acosf(XMVectorGetX(XMVector3Dot(vTargetLook, vDir)));
	_float fDegree = XMConvertToDegrees(Cos);

	//Cross 
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vOriginUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vUp = XMVector3Cross(vLook, vDir);
	_float CCW = XMVectorGetX(XMVector3Dot(vOriginUp, vUp));

	if (CCW < 0.f)
		m_fClockWise = -1.f;
	else
		m_fClockWise = 1.f;

	return fDegree;
}

_float CCharacter::Compute_Degree_On_Land(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = pDstTransform->Get_State(CTransform::STATE_LOOK);
	vTargetLook = XMVectorSetY(vTargetLook, 0.f);

	_vector vDir = vPlayerPos - vTargetPos;
	vDir = XMVector3Normalize(vDir);
	vDir = XMVectorSetY(vDir, 0.f);

	_float Cos = acosf(XMVectorGetX(XMVector3Dot(vTargetLook, vDir)));
	_float fDegree = XMConvertToDegrees(Cos);

	//Cross 
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vOriginUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vUp = XMVector3Cross(vLook, vDir);
	_float CCW = XMVectorGetX(XMVector3Dot(vOriginUp, vUp));

	if (CCW < 0.f)
		m_fClockWise = -1.f;
	else
		m_fClockWise = 1.f;

	return fDegree;
}

HRESULT CCharacter::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCharacter::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int CCharacter::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CCharacter::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CCharacter::Render(RENDER_GROUP::Enum eGroup)
{
	return CGameObject::Render(eGroup);
}

HRESULT CCharacter::Render_ShadowDepth()
{
	return CGameObject::Render_ShadowDepth();
}

#pragma region WonTaek_Path
void CCharacter::Set_SpaceRailNode(CSpaceRail_Node* pRail)
{
	if (nullptr == pRail) return;
	m_vecTargetRailNodes.push_back(pRail);
}
void CCharacter::Find_TargetSpaceRail()
{
	if (nullptr == m_pTransformCom) return;
	if (false == m_bSearchToRail) // 레일타기 키 눌렸을때
	{
		m_vecTargetRailNodes.clear();
		m_bSearchToRail = false;
		return;
	}

	if (m_vecTargetRailNodes.empty()) return; // 키가 눌렸지만, 충돌한 레일 트리거가 존재하지 않을때

	// 거리가 아닌, 플레이어 Look 벡터와의 각도차로 계산
	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	_vector vToTarget = XMVectorZero();
	_vector vNodePosition = XMVectorZero();
	_float fMinDegree = 360.f;
	for (auto& pNode : m_vecTargetRailNodes)
	{
		if (nullptr == pNode) continue;
		vNodePosition = pNode->Get_Position();

		/* 각도 계산  */
		vToTarget = XMVector3Normalize(vNodePosition - vPlayerPosition);
		_float fDegree = XMConvertToDegrees(XMVectorGetX(XMVector3Dot(vPlayerLook, vToTarget)));
		if (fDegree > 90.f) continue; /* 일정각도(45) 이상인 노드는 제외한다. */

		/* 가장 각도가 적은 타겟노드 찾기 */
		if (fMinDegree > fDegree)
		{
			m_pTargetRailNode = pNode;
			fMinDegree = fDegree;
		}
	}

	m_vecTargetRailNodes.clear();
	m_bSearchToRail = false;
	m_bMoveToRail = true;
	
}
void CCharacter::MoveToTargetRail(_uint eState, _double dTimeDelta)
{
	if (false == m_bMoveToRail) return;
	if (nullptr == m_pTargetRailNode) return;

	_float fMoveToSpeed = 20.f;
	_float fDist = m_pTransformCom->Move_ToTargetRange(m_pTargetRailNode->Get_Position(), 0.1f, dTimeDelta * fMoveToSpeed);
	if (fDist < 0.2f)
	{
		m_pTargetRail = (CSpaceRail*)DATABASE->Get_SpaceRail(m_pTargetRailNode->Get_RailTag()); // 타야할 Path 지정
		m_pTargetRail->Start_Path((CPath::STATE)(eState), m_pTargetRailNode->Get_FrameIndex()); // W : Forward / S : Backward
		m_pTargetRailNode = nullptr;

		m_bSearchToRail = false;
		m_bMoveToRail = false;
		m_bOnRail = true;
	}
}
_bool CCharacter::TakeRail(_double dTimeDelta, _matrix& WorldMatrix)
{
	if (nullptr == m_pTargetRail) return false;
	if (false == m_bOnRail) return false;

	return m_pTargetRail->Take_Path(dTimeDelta, WorldMatrix);
}
#pragma endregion

void CCharacter::Free()
{
	m_pTargetRail = nullptr;
	m_pTargetRailNode = nullptr;
	m_vecTargetRailNodes.clear();

	CGameObject::Free();
}