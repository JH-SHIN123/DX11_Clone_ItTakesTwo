#pragma once
#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_MoonBaboon_Shield final : public CInGameEffect_Model
{
// Fresnel
private:
	explicit CEffect_MoonBaboon_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_MoonBaboon_Shield(const CEffect_MoonBaboon_Shield& rhs);
	virtual ~CEffect_MoonBaboon_Shield() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

private:
	void Check_TargetMatrix();

private:
	CTextures* m_pTexture_ColorRamp	 = nullptr;
	CTextures* m_pTexture_Distortion = nullptr;
	CTextures* m_pTexture_Distortion_Point = nullptr;

private:
	_float	m_fTime = 0.f;
	_float	m_fAlphaTime = 1.f;
	_double m_dLifeTime = 20.0;
	_float4 m_vColor = { 0.980392218f, 0.201960814f, 0.147058856f, 1.000000000f };

	_float m_fScale_Pow = 7.f;
	const _float m_fScale_Max = 13.f;
	CVIBuffer_PointInstance_Custom_ST* m_pPointInstance = nullptr;
	VTXMATRIX_CUSTOM_ST* m_pInstanceBuffer = nullptr;

public:
	static CEffect_MoonBaboon_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END