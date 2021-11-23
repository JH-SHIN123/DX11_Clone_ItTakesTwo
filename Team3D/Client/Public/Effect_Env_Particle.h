#pragma once

#ifndef __EFFECT_ENV_PARTICLE_H__
#include "InGameEffect.h"
BEGIN(Engine)
class CVIBuffer_PointInstance_Custom_STT;
END

BEGIN(Client) 
// 동그란 파티클(포탈, 중력 파이프, 우산 등)
// 메쉬 안에서 뿌릴 애들
class CEffect_Env_Particle final : public CInGameEffect
{
private:
	explicit CEffect_Env_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Env_Particle(const CEffect_Env_Particle& rhs);
	virtual ~CEffect_Env_Particle() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void	Set_InstanceCount(_uint iInstanceCount);
	void	Set_Particle_Radius(_float3 vRadiusXYZ);
	void	Set_IsActivateParticles(_bool IsActivate);
	void	Set_ParentMatrix(_fmatrix ParentMatrix);

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;


private:
	void Check_State(_double TimeDelta);
	void State_Start(_double TimeDelta);
	void State_Disappear(_double TimeDelta);
	void State_On(_double TimeDelta);
	void State_Off(_double TimeDelta);

private:
	_float4 Get_Rand_Pos();
	_float2 Get_Rand_Size();
	HRESULT Reset_Instance(_int iIndex);
	HRESULT Reset_Instance_All();
	HRESULT Initialize_Instance();

private: // 전체적인 인스턴싱을 제어함
	enum STATE_VALUE {STATE_START, STATE_DISAPPEAR, STATE_ON, STATE_OFF, STATE_END};
	STATE_VALUE		m_eStateValue_Cur		= STATE_END;
	STATE_VALUE		m_eStateValue_Next		= STATE_START;
	_double			m_dControl_Time			= 0.0;		// 인스턴싱의 알파값을 통괄적으로 제어
	_float3			m_vParticleRadius		= { 0.f, 0.f, 0.f};
	const _float	m_fResetPosTime			= 3.5f;
	const _float2	m_vDefaultSize			= { 0.1f, 0.1f };

private: 
	// 랜덤한 위치에서 사이즈0에서 서서히 커지며 알파값이 올라가면서 등장
	// 모델받아옴 > 원점 정점 사이 구함(길이, 방향) > 구한 길이로 랜덤값 구함 원점으로 부터 방향 * 랜덤길이 = Pos
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT* m_pInstanceBuffer_STT = nullptr;
	_float4* m_pInstanceBuffer_LocalPos = nullptr;
	

public:
	static CEffect_Env_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_ENV_PARTICLE_H__
#endif
