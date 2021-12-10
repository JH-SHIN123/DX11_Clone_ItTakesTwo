#include "stdafx.h"
#include "..\Public\Character.h"
#include "PlayerActor.h"

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

_fmatrix CCharacter::Get_WorldMatrix()
{
	if (nullptr == m_pTransformCom)
		return XMMatrixIdentity();

	return m_pTransformCom->Get_WorldMatrix();
}

_fvector CCharacter::Get_CenterPosition()
{
	NULL_CHECK_RETURN(m_pActorCom, _fvector());

	return MH_ConvertToXMVector(m_pActorCom->Get_Controller()->getPosition(), 1.f);
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

void CCharacter::Free()
{
	CGameObject::Free();
}