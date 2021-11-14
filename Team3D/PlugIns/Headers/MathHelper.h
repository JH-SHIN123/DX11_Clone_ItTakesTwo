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

	static _vector MH_ConvertToXMVector(PxExtendedVec3 vVector, _float fW)
	{
		return XMVectorSet((_float)vVector.x, (_float)vVector.y, (_float)vVector.z, fW);
	}

	static _vector MH_GetXMPosition(_float4x4 Matrix)
	{
		return XMVectorSet(Matrix._41, Matrix._42, Matrix._43, Matrix._44);
	}
	
// 	enum TRANSTORM_STATE {TS_RIGHT, TS_UP, TS_LOOK, TS_POSITION, TS_END};
// 	static _vector MH_GetState(_float4x4 Matrix, TRANSTORM_STATE eState)
// 	{
// 		return XMVectorSet(Matrix.m[eState][0], Matrix.m[eState][1], Matrix.m[eState][2], Matrix.m[eState][3]);
// 	}
}