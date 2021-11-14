#pragma once
#pragma once

#include "Base.h"

/* 그림자 정보들을 보관한다. */
BEGIN(Engine)
class CShadow_Manager final : public CBase
{
	DECLARE_SINGLETON(CShadow_Manager)

private:
	CShadow_Manager() = default;
	virtual ~CShadow_Manager() = default;

public:
	const _float* Get_CascadedEnds() const { return m_fCascadedEnds; }
	//const _float* Get_CascadedEndsW() const { return m_fCascadedEndsW; }

	/* 0: Main / 1: Sub */
	void Get_CascadeShadowLightViewProjTranspose(_uint iViewportIndex, _matrix* OutMatrix) const;
	void Get_CascadeShadowTransformsTranspose(_uint iViewportIndex, _matrix* OutMatrix) const;

public:
	HRESULT Ready_ShadowManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);

public:
	_int	Update_CascadedShadowTransform_MainViewport();
	_int	Update_CascadedShadowTransform_SubViewport();

	HRESULT RSSet_CascadedViewports();

private:
	HRESULT	Set_CascadeViewportsInfo();
	HRESULT Update_CascadeShadowTransform(_uint iViewportIndex);

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDevice_Context = nullptr;

private: /* For. Cascaded */
	D3D11_VIEWPORT	m_CascadeViewport[MAX_CASCADES];
	const _float	m_fCascadedEnds[MAX_CASCADES + 1] = { 0.05f, 0.15f, 0.5f, 1.f };

	/* For. Main Viewports */
	_float4x4 m_CascadeViews[MAX_CASCADES];
	_float4x4 m_CascadeProjs[MAX_CASCADES];
	_float4x4 m_CascadedShadowTransforms[MAX_CASCADES];

	/* For. Sub Viewports */
	_float4x4 m_CascadeViews_Sub[MAX_CASCADES];
	_float4x4 m_CascadeProjs_Sub[MAX_CASCADES];
	_float4x4 m_CascadedShadowTransforms_Sub[MAX_CASCADES];


public:
	virtual void Free() override;
};

END