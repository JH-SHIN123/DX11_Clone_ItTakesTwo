#include "..\public\Transform.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_State(STATE eState) const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
}

_float CTransform::Get_Scale(STATE eSTate) const
{
	return XMVectorGetX(XMVector3Length(Get_State(eSTate)));
}

_matrix CTransform::Get_WorldMatrix() const
{
	return XMLoadFloat4x4(&m_WorldMatrix);
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_vector));
}

void CTransform::Set_Scale(_fvector vScale)
{
	_vector	vRight	= XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScale);
	_vector	vUp		= XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScale);
	_vector	vLook	= XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Set_RotateAxis(_fvector vAxis, _float fRadian)
{
	_vector	vRight	= XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_RIGHT);
	_vector	vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_UP);
	_vector	vLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(CTransform::STATE_LOOK);

	_matrix	RotateMatrix	= XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotateMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp, RotateMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotateMatrix));
}

void CTransform::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	CComponent::NativeConstruct_Prototype();

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORM_DESC));

	return S_OK;
}

void CTransform::Go_Straight(_double dTimeDelta)
{
	_vector vLook		= Get_State(CTransform::STATE_LOOK);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_double dTimeDelta)
{
	_vector vLook		= Get_State(CTransform::STATE_LOOK);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);;

	vPosition -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_double dTimeDelta)
{
	_vector vRight		= Get_State(CTransform::STATE_RIGHT);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);;

	vPosition += XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_double dTimeDelta)
{
	_vector vRight		= Get_State(CTransform::STATE_RIGHT);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);;

	vPosition -= XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_double dTimeDelta)
{
	_vector vUp			= Get_State(CTransform::STATE_UP);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);;

	vPosition += XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_double dTimeDelta)
{
	_vector vUp			= Get_State(CTransform::STATE_UP);
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);;

	vPosition -= XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Rotate_Axis(_fvector vAxis, _double dTimeDelta)
{
	_vector	vRight	= Get_State(CTransform::STATE_RIGHT);
	_vector	vUp		= Get_State(CTransform::STATE_UP);
	_vector	vLook	= Get_State(CTransform::STATE_LOOK);

	_matrix RotateMatrix	= XMMatrixRotationAxis(vAxis, (_float)m_TransformDesc.dRotationPerSec * (_float)dTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotateMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp, RotateMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotateMatrix));
}

void CTransform::Rotate_ToTarget(_fvector vTargetPos)
{
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);
	_vector vDirection	= vTargetPos - vPosition;

	_vector vLook	= XMVector3Normalize(vDirection) * Get_Scale(CTransform::STATE_LOOK);
	_vector vRight	= XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * Get_Scale(CTransform::STATE_RIGHT);
	_vector vUp		= XMVector3Normalize(XMVector3Cross(vLook, vRight)) * Get_Scale(CTransform::STATE_UP);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Rotate_ToTargetOnLand(_fvector vTargetPos)
{
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);
	_vector vDirection	= vTargetPos - vPosition;

	_vector vRight	= XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDirection)) * Get_Scale(CTransform::STATE_RIGHT);
	_vector vUp		= Get_State(CTransform::STATE_UP);
	_vector vLook	= XMVector3Normalize(XMVector3Cross(vRight, vUp)) * Get_Scale(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Move_ToTarget(_fvector vTargetPos, _double dTimeDelta)
{
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);
	_vector vDirection	= vTargetPos - vPosition;

	vPosition += XMVector3Normalize(vDirection) * (_float)dTimeDelta * (_float)m_TransformDesc.dSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPosition);
	Rotate_ToTargetOnLand(vTargetPos);
}

void CTransform::RotateYawDirectionOnLand(const _fvector & vMoveDir, const _double TimeDelta, const _double dAcceleration, CNavigation * pNavigation)
{
	if (0.0 == m_TransformDesc.dSpeedPerSec) return;

	_vector vMoveDir_Norm = XMVector3Normalize(vMoveDir);

	_vector	vUp, vLook;

	vLook = Get_State(STATE_LOOK);
	vUp = Get_State(STATE_UP);
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);

	_float fCosValue = XMVectorGetX(XMVector3Dot(vMoveDir_Norm, vLook));

	// 회전 각도 제한
	_float fAcosValue = 0.f;
	if (fCosValue <= -1.f) // 
		fAcosValue = (_float)3.141592;
	else if (fCosValue > 1.f) // 
		fAcosValue = 0.f;
	else
		fAcosValue = acos(fCosValue); // Dir - vLook (Radian)

									  // 외적해서, y축 부호 시계방향, 반시계방향 정하기
	_float fCCW = XMVectorGetX(XMVector3Dot(vUp, XMVector3Cross(vMoveDir_Norm, vLook)));

	_float fDeltaDegree = XMConvertToDegrees(fAcosValue);
	if (fCCW > 0)	// ccw가 양수이면 반시계로 돌아야함
		fDeltaDegree *= -1;

	// Transform
	RotateYaw(TimeDelta * XMConvertToRadians(fDeltaDegree) * 2.f);
}

void CTransform::RotatePitchDirectionOnLand(const _fvector & vMoveDir, const _double TimeDelta, const _double dAcceleration, CNavigation * pNavigation)
{
	if (0.0 == m_TransformDesc.dSpeedPerSec) return;

	_vector vMoveDir_Norm = XMVector3Normalize(vMoveDir);

	_vector	vUp, vLook;

	vLook = Get_State(STATE_LOOK);
	vUp = Get_State(STATE_UP);
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);

	_float fCosValue = XMVectorGetX(XMVector3Dot(vMoveDir_Norm, vLook));

	// 회전 각도 제한
	_float fAcosValue = 0.f;
	if (fCosValue <= -1.f) // 
		fAcosValue = (_float)3.141592;
	else if (fCosValue > 1.f) // 
		fAcosValue = 0.f;
	else
		fAcosValue = acos(fCosValue); // Dir - vLook (Radian)

									  // 외적해서, y축 부호 시계방향, 반시계방향 정하기
	_float fCCW = XMVectorGetX(XMVector3Dot(vUp, XMVector3Cross(vMoveDir_Norm, vLook)));

	_float fDeltaDegree = XMConvertToDegrees(fAcosValue);
	if (fCCW > 0)	// ccw가 양수이면 반시계로 돌아야함
		fDeltaDegree *= -1;

	// Transform
	RotatePitch(TimeDelta * XMConvertToRadians(fDeltaDegree) * 2.f);
}

void CTransform::MoveDirectionOnLand(const _fvector & vMoveDir, const _double TimeDelta, const _double dAcceleration, CNavigation * pNavigation)
{
	if (0.0 == m_TransformDesc.dSpeedPerSec) return;

	_vector vMoveDir_Norm = XMVector3Normalize(vMoveDir);

	_vector	vUp, vLook;

	vLook = Get_State(STATE_LOOK);
	vUp = Get_State(STATE_UP);
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);

	_float fCosValue = XMVectorGetX(XMVector3Dot(vMoveDir_Norm, vLook));

	// 회전 각도 제한
	_float fAcosValue = 0.f;
	if (fCosValue <= -1.f) // 
		fAcosValue = (_float)3.141592;
	else if (fCosValue > 1.f) // 
		fAcosValue = 0.f;
	else
		fAcosValue = acos(fCosValue); // Dir - vLook (Radian)

									  // 외적해서, y축 부호 시계방향, 반시계방향 정하기
	_float fCCW = XMVectorGetX(XMVector3Dot(vUp, XMVector3Cross(vMoveDir_Norm, vLook)));

	_float fDeltaDegree = XMConvertToDegrees(fAcosValue);
	if (fCCW > 0)	// ccw가 양수이면 반시계로 돌아야함
		fDeltaDegree *= -1;

	// Transform
	RotateYaw(TimeDelta * XMConvertToRadians(fDeltaDegree) * 2.f);
	Go_Straight(TimeDelta * m_TransformDesc.dSpeedPerSec * dAcceleration);
}

void CTransform::RotateYaw(const _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(vUp), (_float)(TimeDelta * 5.f));

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotateMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotateMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotateMatrix));
}

void CTransform::RotatePitch(const _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(vRight), (_float)(TimeDelta * 5.f));

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotateMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotateMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotateMatrix));
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform* pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone_Component(void * pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	CComponent::Free();
}
