#pragma once

#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_Boss_Gravitational_Bomb final : public CInGameEffect_Model
{
private:
	explicit CEffect_Boss_Gravitational_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Gravitational_Bomb(const CEffect_Boss_Gravitational_Bomb& rhs);
	virtual ~CEffect_Boss_Gravitational_Bomb() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;


private:
	void Scale_Check(_double TimeDelta);

private:
	CTextures* m_pTexture_Distortion = nullptr;

private:
	_bool  m_IsScaling_End = false;
	_double m_dLifeTime = 15.0;
	_float m_fTime = 0.f;
	_float m_fSpinTime = 0.001f;
	_float4 m_vColor = { 0.980392218f, 0.201960814f, 0.147058856f, 1.000000000f };

	const _float m_fScale_Pow = 5.f;
	const _float m_fScale_Max = 13.f;

public:
	static CEffect_Boss_Gravitational_Bomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END

