#pragma once

#ifndef __EFFECT_PLAYER_DEAD_PARTICLE_FIRE_H__

#include "InGameEffect.h"

BEGIN(Engine)
class CVIBuffer_Rect_TripleUV;
END

BEGIN(Client)
class CEffect_Player_Dead_Particle_Fire final : public CInGameEffect
{
private:
	explicit CEffect_Player_Dead_Particle_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Player_Dead_Particle_Fire(const CEffect_Player_Dead_Particle_Fire& rhs);
	virtual ~CEffect_Player_Dead_Particle_Fire() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();		// 파티클
	//HRESULT Ready_Point();			// 스모크
	HRESULT Ready_Point_Small();	// 스모크 2
	_float4 Set_particleUV(_int iIndex, _int U, _int V);

private:
	_float2 Get_RandSize();
	_float3 Get_Dir_Defulat(_int3 vDirPower, _fmatrix WorldMatrix);
	_float3 Get_Dir(_int3 vDirPower, _fmatrix WorldMatrix);
	_float3 Get_Dir_Random(_int3 vDirPower, _fmatrix WorldMatrix);

private:
	//void Update_Point(_double TimeDelta);
	void Update_Point_Small(_double TimeDelta);


private:
	_double		m_dAlphaTime = 1.5;
	_bool		m_IsDisapear = false;
	CModel*		m_pTargetModel = nullptr;
	_float4*	m_pInstance_LocalPos = nullptr;

private:
// 	CTextures*				m_pTexturesCom_Point_Smoke		= nullptr;
// 	VTXMATRIX_CUSTOM_ST*	m_pPointBuffer_Smoke			= nullptr;
// 	_float					m_fPointInstance_Alpha			= 0.f;
// 	const _float			m_fUVCheckTime_Max = 0.01f;
// 	_float					m_fUVCheckTime = 0.f;
// 	_float					m_fSmokeAlphaTime = 0.5;
// 	_int					m_iPointInstance_Texture_U = 0;
// 	_int					m_iPointInstance_Texture_V = 0;

private:
	CTextures*				m_pTexturesCom_Point_Diff		= nullptr;
	CTextures*				m_pTexturesCom_Point_Sprite		= nullptr;
	VTXMATRIX_CUSTOM_ST*	m_pPointBuffer_Smoke_Small		= nullptr;
	_float3*				m_vPointBuffer_Small_Dir		= nullptr;
	const _int				m_iPointInstanceCount_Small		= 5;
	_float					m_fPointInstance_Small_Alpha	= 0.f;
	_bool					m_IsPointInstance_Small_Alpha_Add = true;


public:
	static CEffect_Player_Dead_Particle_Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_DEAD_PARTICLE_FIRE_H__
#endif
