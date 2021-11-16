#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeline final : public CBase
{
	DECLARE_SINGLETON(CPipeline)
private:
	explicit CPipeline() = default;
	virtual ~CPipeline() = default;

public: /* Enum */
	enum TRANSFORM_STATETYPE { 
		TS_MAINVIEW, TS_MAINVIEW_INVERSE, TS_MAINPROJ, TS_MAINPROJ_INVERSE, TS_SUBVIEW, TS_SUBVIEW_INVERSE, TS_SUBPROJ, TS_SUBPROJ_INVERSE, 
		TS_FULLSCREEN_PROJ, TS_FULLSCREEN_PROJ_INVERSE,
		TS_END };

public: /* Getter */
	_matrix			Get_Transform(TRANSFORM_STATETYPE eType) const { return XMLoadFloat4x4(&m_StateMatrices[eType]); }
	_fvector		Get_MainCamPosition() const { return XMLoadFloat4((_float4*)&m_StateMatrices[CPipeline::TS_MAINVIEW_INVERSE].m[3][0]); }
	_fvector		Get_SubCamPosition() const { return XMLoadFloat4((_float4*)&m_StateMatrices[CPipeline::TS_SUBVIEW_INVERSE].m[3][0]); }
	const _float	Get_FullscreenFar() const { return m_fFullscreenFar; }
	const _float	Get_MainCamFar() const { return m_fMainCamFar; }
	const _float	Get_SubCamFar() const { return m_fSubCamFar; }

public: /* Setter */
	void Set_Transform(TRANSFORM_STATETYPE eType, _fmatrix matTransformMatrix) { return XMStoreFloat4x4(&m_StateMatrices[eType], matTransformMatrix); }
	void Set_FullScreenFar(_float fFar) { m_fFullscreenFar = fFar; }
	void Set_MainCamFar(_float fFar) { m_fMainCamFar = fFar; }
	void Set_SubCamFar(_float fFar) { m_fSubCamFar = fFar; }

private:
	_float4x4	m_StateMatrices[TS_END];

	_float		m_fFullscreenFar = 0.f;
	_float		m_fMainCamFar = 0.f;
	_float		m_fSubCamFar = 0.f;

public:
	virtual void Free() override;
};

END

/*
	MAIN-은 메인카메라용, SUB-는 서브카메라용.
	메인 카메라에서는 메인 뷰행렬/투영행렬/역뷰행렬/역투영행렬을 갱신하도록 세팅할 것.
	서브 카메라에서는 서브 뷰행렬/투영행렬/역뷰행렬/역투영행렬을 갱신하도록 세팅할 것.
*/