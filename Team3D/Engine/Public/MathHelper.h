#pragma once
namespace Engine
{
	static _float4x4 MH_XMFloat4x4Identity()
	{
		return XMFLOAT4X4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	static PxTransform	MH_PxTransform(_fvector vRotQuat, _fvector vPosition)
	{
		PxTransform Transform;

		Transform.q.x = XMVectorGetX(vRotQuat);
		Transform.q.y = XMVectorGetY(vRotQuat);
		Transform.q.z = XMVectorGetZ(vRotQuat);
		Transform.q.w = XMVectorGetW(vRotQuat);

		Transform.p.x = XMVectorGetX(vPosition);
		Transform.p.y = XMVectorGetY(vPosition);
		Transform.p.z = XMVectorGetZ(vPosition);

		return Transform;
	}

	static _float3 MH_XMFloat3(PxVec3 vPxVector)
	{
		_float3 vVector;

		vVector.x = vPxVector.x;
		vVector.y = vPxVector.y;
		vVector.z = vPxVector.z;

		return vVector;
	}

	static PxVec3 MH_PxVec3(_fvector vVector)
	{
		PxVec3 vPxVector;

		vPxVector.x = XMVectorGetX(vVector);
		vPxVector.y = XMVectorGetY(vVector);
		vPxVector.z = XMVectorGetZ(vVector);

		return vPxVector;
	}

	static PxVec3 MH_PxVec3(_float3 vVector)
	{
		PxVec3 vPxVector;

		vPxVector.x = vVector.x;
		vPxVector.y = vVector.y;
		vPxVector.z = vVector.z;

		return vPxVector;
	}

	static PxVec3 MH_PxVec3(_float3 vVector, _float fX)
	{
		PxVec3 vPxVector;

		vPxVector.x = vVector.x * fX;
		vPxVector.y = vVector.y * fX;
		vPxVector.z = vVector.z * fX;

		return vPxVector;
	}

	static PxVec3 MH_PxVec3(PxExtendedVec3 vVector)
	{
		PxVec3 vPxVector;

		vPxVector.x = (_float)vVector.x;
		vPxVector.y = (_float)vVector.y;
		vPxVector.z = (_float)vVector.z;

		return vPxVector;
	}

	static PxQuat MH_PxQuat(_fvector vVector, _float fW)
	{
		PxQuat vPxVector;

		vPxVector.x = XMVectorGetX(vVector);
		vPxVector.y = XMVectorGetY(vVector);
		vPxVector.z = XMVectorGetZ(vVector);
		vPxVector.w = fW;

		return vPxVector;
	}

	static _vector MH_XMVec3(PxVec3 vPx)
	{
		return XMVectorSet(vPx.x, vPx.y, vPx.z, 0.f);
	}

	static _vector MH_XMVec3(PxExtendedVec3 vPx)
	{
		return XMVectorSet((_float)vPx.x, (_float)vPx.y, (_float)vPx.z, 0.f);
	}

	static PxExtendedVec3 MH_PxExtendedVec3(_fvector vVector)
	{
		PxExtendedVec3 vPxVector;

		vPxVector.x = (_double)XMVectorGetX(vVector);
		vPxVector.y = (_double)XMVectorGetY(vVector);
		vPxVector.z = (_double)XMVectorGetZ(vVector);

		return vPxVector;
	}

	static PxExtendedVec3 MH_PxExtendedVec3(PxVec3 vVector)
	{
		PxExtendedVec3 vPxVector;

		vPxVector.x = (_double)vVector.x;
		vPxVector.y = (_double)vVector.y;
		vPxVector.z = (_double)vVector.z;

		return vPxVector;
	}

	static _vector MH_ConvertToXMVector(PxExtendedVec3 vVector, _float fW)
	{
		return XMVectorSet((_float)vVector.x, (_float)vVector.y, (_float)vVector.z, fW);
	}

	static _vector MH_ConvertToXMVector(PxVec3 vVector, _float fW)
	{
		return XMVectorSet(vVector.x, vVector.y, vVector.z, fW);
	}

	static _vector MH_GetXMPosition(_float4x4 Matrix)
	{
		return XMVectorSet(Matrix._41, Matrix._42, Matrix._43, Matrix._44);
	}

	static _matrix MH_XMMatrix(PxMat44 PxMatrix)
	{
		_matrix XmMatrix;

		memcpy(&XmMatrix, &PxMatrix, sizeof(_matrix));

		return XmMatrix;
	}

	static _float MH_Dist(_fvector vSrc, _fvector vDst)
	{
		return XMVectorGetX(XMVector3Length(vSrc - vDst));
	}

	static _float	GetRandomFloat(_float lowBound, _float highBound)
	{
		if (lowBound >= highBound) // bad input
			return lowBound;

		// get random float in [0, 1] interval
		_float f = (rand() % 10000) * 0.0001f;

		// return float in [lowBound, highBound] interval. 
		return (f * (highBound - lowBound)) + lowBound;
	}

	static _matrix MH_GetQuaternion(_fvector v0, _fvector v1)
	{
		_vector vCross = XMVector3Cross(v0, v1);

		_float fS = sqrtf((1.f + XMVectorGetX(XMVector3Dot(v0, v1))) * 2.f);

		_float4 vQuat;
		vQuat.x = XMVectorGetX(vCross) / fS;
		vQuat.y = XMVectorGetY(vCross) / fS;
		vQuat.z = XMVectorGetZ(vCross) / fS;
		vQuat.w = XMVectorGetW(vCross) / 2.f;
	
		return XMMatrixRotationQuaternion(XMLoadFloat4(&vQuat));
	}

	/* @Return Radian */
	static _float3 MH_GetRoatationAnglesToMatrix(const _float4x4& fRotateMatrix)
	{
		/*
		Rotate Matrix
		| r11 r12 r13  0 |
		| r21 r22 r23  0 |
		| r31 r32 r33  0 |
		|  0   0   0   1 |
		*/

		return{
			atan2(fRotateMatrix._23, fRotateMatrix._33),
			atan2(-fRotateMatrix._13, sqrt(pow(fRotateMatrix._23, 2) + pow(fRotateMatrix._33, 2))),
			atan2(fRotateMatrix._12, fRotateMatrix._11)
		};
	}

	/* @Return Radian */
	static _float3 MH_GetRoatationAnglesToMatrix(const _fmatrix fRotateMatrix)
	{
		/*
		Rotate Matrix
		| r11 r12 r13  0 |
		| r21 r22 r23  0 |
		| r31 r32 r33  0 |
		|  0   0   0   1 |
		*/
		_float3 vRight, vUp, vLook;
		XMStoreFloat3(&vRight, fRotateMatrix.r[0]);
		XMStoreFloat3(&vUp, fRotateMatrix.r[1]);
		XMStoreFloat3(&vLook, fRotateMatrix.r[2]);

		return{
			atan2(vUp.z, vLook.z),
			atan2(-vRight.z, sqrt(pow(vUp.z, 2) + pow(vLook.z, 2))),
			atan2(vRight.y, vRight.z)
		};
	}
}

