#pragma once

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_CS_Levitation_Beam final : public CInGameEffect
{
private:
	explicit CEffect_CS_Levitation_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_CS_Levitation_Beam(const CEffect_CS_Levitation_Beam& rhs);
	virtual ~CEffect_CS_Levitation_Beam() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	void Check_WorldMatrix();
	void Check_RotateUV(_double TimeDelta);

private:
	CVIBuffer_Rect*		m_pBufferRectCom = nullptr;
	_double				m_dActivate_Time = 0.0;
	_bool				m_IsActivate = true;

	_float4 m_vColor = { 0.22549081f, 0.22549081f, 1.000000000f, 1.000000000f };//= { 0.22549081f, 0.22549081f, 1.000000000f, 1.000000000f };
	_double m_dRotateTime = 0.0;
	_float  m_fTime = 0.0f;
	_float2 m_vSize = { 8.f, 27.f };
	_float4 m_vColorRamp_UV = { 0.f, 0.f, 1.f, 1.f };

private:
	class CEffect_CS_Levitation_Beam_Particle* m_pParticle = nullptr;

public:
	static CEffect_CS_Levitation_Beam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END