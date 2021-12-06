#pragma once

namespace Client
{
	typedef struct tagWorldMatrix
	{
		XMFLOAT4	vScale;
		XMFLOAT4	vRotQuat;
		XMFLOAT4	vTrans;
	}WORLDMATRIX;

	typedef struct PassConstant
	{
		_float fTimeDelta = 0.f;
	}PASS_CB;
}