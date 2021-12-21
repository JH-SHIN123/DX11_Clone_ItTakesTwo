#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_PipeLocker_Particle final : public CInGameEffect
{
private:
	explicit CEffect_PipeLocker_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_PipeLocker_Particle(const CEffect_PipeLocker_Particle& rhs);
	virtual ~CEffect_PipeLocker_Particle() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }
	void Set_Dir(_fvector vDir) { XMStoreFloat3(&m_vDir, vDir); }

private:
	void Check_Instance(_double TimeDelta);

private:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0)		override;

private:
	void Reset_Instance(_double TimeDelta, _vector vPos, _int iIndex = 0);

private:
	HRESULT Ready_InstanceBuffer();
	_float3 Get_Particle_Rand_Dir(_fvector vDefaultPos);
	void	Check_Target_Matrix();
	_float2 Get_RandSize();

private:
	_double m_dControlTime = 0.5; //
	_bool m_IsActivate = true;

private:
	_float4 m_vColor = { 0.101176534f, 0.101176534f, 1.000000000f, 1.0f };
	_float3 m_vDir;

	const _float  m_fInstance_SpeedPerSec = 1.5f;
	const _double m_dInstance_Pos_Update_Time = 2.0;
	const _float2 m_vDefaultSize = { 0.6f, 0.6f };

public:
	static CEffect_PipeLocker_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END