#pragma once
#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_Cody_Size_ShockWave final : public CInGameEffect_Model
{
private:
	explicit CEffect_Cody_Size_ShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Cody_Size_ShockWave(const CEffect_Cody_Size_ShockWave& rhs);
	virtual ~CEffect_Cody_Size_ShockWave() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

private:
	_float3 m_vScale = { 0.f, 0.f, 0.f };
	_bool  m_IsScaling_Bigger = false;
	_double m_dLifeTime = 0.0;
	_float m_fAlphaTime = 0.3f;
	_float4 m_vColor = { 0.980392218f, 0.980392218f, 0.980392218f, 0.300000000f };

	const _float m_fScale_Pow = 10.25f;

public:
	static CEffect_Cody_Size_ShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END