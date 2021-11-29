#include "stdafx.h"
#include "..\Public\Character.h"
#include "SpaceRail.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{
}

void CCharacter::Set_SpaceRail(CSpaceRail* pRail)
{
	if (nullptr == pRail) return;

	m_vecRideOnRails.push_back(pRail);
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

void CCharacter::Find_SpaceRailTarget()
{
	if (nullptr == m_pTransformCom) return;

	_vector vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRailPosition = XMVectorZero();
	_float fDist = FLT_MAX;
	for (auto& pRail : m_vecRideOnRails)
	{
		if(nullptr == pRail) continue;
		vRailPosition = pRail->Get_Position();

		_float fToRailDist = XMVectorGetX(XMVector3Length(vPlayerPosition - vRailPosition));
		if (fDist > fToRailDist)
		{
			m_pTargetSpaceRail = pRail;
			fDist = fToRailDist;
		}
	}

	// 등록된 스페이스 레일 비워주기
	m_vecRideOnRails.clear();
}

void CCharacter::Free()
{
	CGameObject::Free();
}