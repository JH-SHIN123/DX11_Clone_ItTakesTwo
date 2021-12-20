#pragma once
#include "Effect_Env_Particle.h"

BEGIN(Client)
class CEffect_Env_Particle_Follow final : public CEffect_Env_Particle
{
private:
	explicit CEffect_Env_Particle_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Env_Particle_Follow(const CEffect_Env_Particle_Follow& rhs);
	virtual ~CEffect_Env_Particle_Follow() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Follow_Distance(_float fFollow_Distance) { m_fFollow_Distance = fFollow_Distance; }

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	void Check_Target();
	void Check_State(_double TimeDelta);
	void State_Start(_double TimeDelta);
	void State_Disappear(_double TimeDelta);
	void Reset_Instance(_int iIndex, _fmatrix WorldMatrix);

private:
	_float	m_fFollow_Distance	= 1.f;
	_bool	m_IsDetected_Cody	= false;
	_bool	m_IsDetected_May	= false;
	_float4 m_vPos_Cody			= { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vPos_May			= { 0.f, 0.f, 0.f, 1.f };


public:
	static CEffect_Env_Particle_Follow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END