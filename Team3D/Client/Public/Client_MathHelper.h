#pragma once

namespace Client
{
	static _matrix MH_RotateByUp(_fmatrix WorldMatrix, _fvector vUp)
	{
		_matrix Result;

		_float fScaleRight = XMVectorGetX(XMVector3Length(WorldMatrix.r[0]));
		_float fScaleUp = XMVectorGetX(XMVector3Length(WorldMatrix.r[1]));
		_float fScaleLook = XMVectorGetX(XMVector3Length(WorldMatrix.r[2]));

		_vector	vLook = XMVectorSetW(XMVector3Normalize(WorldMatrix.r[2]), 0.f);
		_vector	vRight = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vUp, vLook)), 0.f);
		vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

		Result.r[0] = vRight * fScaleRight;
		Result.r[1] = XMVector3Normalize(vUp) * fScaleUp;
		Result.r[2] = vLook * fScaleLook;
		Result.r[3] = WorldMatrix.r[3];

		return Result;
	}

	static _matrix MH_RotationMatrixByUp(_fvector vUp, _fvector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f))
	{
		_matrix Result;

		_vector vNormalizedUp = XMVectorSetW(XMVector3Normalize(vUp), 0.f);

		//_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		//_vector vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));
		//vLook = XMVector3Normalize(XMVector3Cross(vRight, vNormalizedUp));

		//Result.r[0] = vRight;
		//Result.r[1] = vNormalizedUp;
		//Result.r[2] = vLook;
		//Result.r[3] = vPos;

		_float fX = abs(XMVectorGetX(vNormalizedUp));
		_float fY = abs(XMVectorGetY(vNormalizedUp));
		_float fZ = abs(XMVectorGetZ(vNormalizedUp));

		_float fMax = fmax(fmax(fX, fY), fZ);

		if (fZ == fMax)
		{
			_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vNormalizedUp));
			vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));

			Result.r[0] = vRight;
			Result.r[1] = vNormalizedUp;
			Result.r[2] = vLook;
			Result.r[3] = vPos;
		}
		else
		{
			_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			_vector vRight = XMVector3Normalize(XMVector3Cross(vNormalizedUp, vLook));
			vLook = XMVector3Normalize(XMVector3Cross(vRight, vNormalizedUp));

			Result.r[0] = vRight;
			Result.r[1] = vNormalizedUp;
			Result.r[2] = vLook;
			Result.r[3] = vPos;
		}

		return Result;
	}

	static _fmatrix MH_LerpMatrix(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fTime)
	{
		_vector vScale, vQuat, vPosition;
		_vector vSrcScale, vSrcQuat, vSrcPosition;
		_vector vDstScale, vDstQuat, vDstPosition;

		XMMatrixDecompose(&vSrcScale, &vSrcQuat, &vSrcPosition, SrcMatrix);
		XMMatrixDecompose(&vDstScale, &vDstQuat, &vDstPosition, SrcMatrix);

		vScale		= XMVectorLerp(vSrcScale, vDstScale, fTime);
		vQuat		= XMQuaternionSlerp(vSrcQuat, vDstQuat, fTime);
		vPosition	= XMVectorLerp(vSrcPosition, vDstPosition, fTime);

		return XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuat, vPosition);
	}
}