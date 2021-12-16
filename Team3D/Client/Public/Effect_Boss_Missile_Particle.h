#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_Missile_Particle final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Missile_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Missile_Particle(const CEffect_Boss_Missile_Particle& rhs);
	virtual ~CEffect_Boss_Missile_Particle() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Pos(_fvector vPos);
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }
	void Set_Parabola();

private:
	void Check_Instance(_double TimeDelta);
	void Check_Parabola(_double TimeDelta);

private:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0)		override;

private:
	void Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex = 0);

private:
	HRESULT Ready_InstanceBuffer();

private:
	_double m_dControlTime = 0.0; //
	_bool m_IsActivate = true;
	_float3 m_vDir;
	_float m_fJumpPower = 5.f;
	_float m_fJumpTime = 0.f;
	_float m_fJumpStartPos_Y;
	_float m_fMovePower = 0.f;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	CTextures*							m_pTexturesCom_Distortion = nullptr;
	_double*							m_pInstance_Update_TextureUV_Time = nullptr;

	_float m_fNextUV = 0.f;

	const XMINT2  m_vTexUV = { 8,8 };
	const _float  m_fAlphaTime_Power = 0.5f;
	const _float  m_fSize_Power = 0.55f;
	const _float  m_fInstance_SpeedPerSec = 0.5f;
	const _double m_dInstance_Pos_Update_Time = 1.25;
	const _float2 m_vDefaultSize = { 0.5f, 0.5f };

public:
	static CEffect_Boss_Missile_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END