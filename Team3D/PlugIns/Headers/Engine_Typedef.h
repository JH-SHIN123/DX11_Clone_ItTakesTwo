#pragma once

namespace Engine
{
	typedef		bool			_bool;

	typedef		signed char		_char;
	typedef		signed char		_byte;
	typedef		unsigned char	_uchar;
	typedef		unsigned char	_ubyte;

	typedef		wchar_t			_tchar;

	typedef		signed short	_short;
	typedef		unsigned short	_ushort;

	typedef		signed int		_int;
	typedef		unsigned int	_uint;

	typedef		signed long		_long;
	typedef		unsigned long	_ulong;

	typedef		float			_float;
	typedef		double			_double;

	typedef		XMFLOAT2		_float2;
	typedef		XMFLOAT3		_float3;
	typedef		XMFLOAT4		_float4;
	typedef		XMFLOAT4X4		_float4x4;

	typedef		XMVECTOR		_vector;
	typedef		FXMVECTOR		_fvector;
	typedef		GXMVECTOR		_gvector;
	typedef		HXMVECTOR		_hvector;

	typedef		XMMATRIX		_matrix;
	typedef		FXMMATRIX		_fmatrix;
	typedef		CXMMATRIX		_cmatrix;

	typedef		BoundingBox						AABB;
	typedef		BoundingOrientedBox				OBB;
	typedef		BoundingSphere					SBB;

	typedef		vector<BoundingBox*>			AABB_GROUP;
	typedef		vector<BoundingOrientedBox*>	OBB_GROUP;
	typedef		vector<BoundingSphere*>			SBB_GROUP;
}