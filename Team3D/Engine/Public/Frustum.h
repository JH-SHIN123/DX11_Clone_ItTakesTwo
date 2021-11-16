#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)

public:
	enum TYPE { FRUSTUM_MAIN, FRUSTUM_SUB, FRUSTUM_MAIN_FULLSCREEN, FRUSTUM_SUB_FULLSCREEN, FRUSTUM_END };

private:
	explicit CFrustum() = default;
	virtual ~CFrustum() = default;

public: /* Struct */
	typedef struct tagFrustumDesc
	{
		_float3	vPoints_World[8];
		_float3	vPoints_Local[8];
		_float4	PlaneWorld[6];
		_float4	PlaneLocal[6];

		_float	fFrustumDepth = 0.f;
	}FRUSTUM_DESC;

public:
	const _float3*	Get_FrustumPointsInWorld(_uint iIndex /* 1: Main 2: Sub */) const { return m_Frustum[iIndex].vPoints_World; }
	_float			Get_FrustumDepth(_uint iIndex) const { return m_Frustum[iIndex].fFrustumDepth; }

public:
	HRESULT	Ready_Frustum();
	void	Transform_ToWorldSpace();
	void	Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool	IsIn_WorldSpace(_fvector vPosition, _float fRadius);
	_bool	IsIn_LocalSpace(_fvector vPosition, _float fRadius);

private:
	_float3			m_vPoints[8];
	FRUSTUM_DESC	m_Frustum[FRUSTUM_END];

public:
	 virtual void Free() override;
};

END

/*
	Frustum 구조체 하나가 절두체 하나를 정의.
	0번 구조체는 메인카메라 기준 절두체.
	1번 구조체는 서브카메라 기준 절두체.
	Transform 함수로는 두 절두체를 동시에 갱신.
	IsIn 함수는 position이 절두체 둘 중 하나 이상에 들어가면 true를 리턴.
*/