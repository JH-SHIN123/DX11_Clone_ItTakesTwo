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

	static PxVec3 MH_PxVec3(_fvector vVector)
	{
		PxVec3 vPxVector;

		vPxVector.x = XMVectorGetX(vVector);
		vPxVector.y = XMVectorGetY(vVector);
		vPxVector.z = XMVectorGetZ(vVector);

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
}