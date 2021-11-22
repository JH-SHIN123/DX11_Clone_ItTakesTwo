#pragma once

#ifndef __EFFECT_RESPAWNTUNNEL_PORTAL_H__

#include "InGameEffect_Model.h"

BEGIN(Engine)
class CVIBuffer_PointInstance_Custom_STT;
END

BEGIN(Client)
class CEffect_RespawnTunnel_Portal final : public CInGameEffect_Model
{
private:
	explicit CEffect_RespawnTunnel_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_RespawnTunnel_Portal(const CEffect_RespawnTunnel_Portal& rhs);
	virtual ~CEffect_RespawnTunnel_Portal() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void SetUp_WorldMatrix(_fmatrix WorldMatrix) override;


public:
	HRESULT Ready_Instance();

private:
	_float4 Get_TexUV(_int iTexture_U, _int iTexture_V, _bool IsRand);
	_vector Get_RandDir_Look();
	void Check_Smoke(_double TimeDelta);
	_float4 Check_TexUV(_float4 vNowUV);
	void Set_Shader_SmokeData();

private:
	_double m_dAngle = 0.0;
	_float	m_fColorRamp_U = 0.f;
	CTextures* m_pTexturesCom_ColorRamp = nullptr;

private:
	CVIBuffer_PointInstance_Custom_STT*	m_pPointInstanceCom_Smoke			= nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_Smoke				= nullptr;
	CTextures*							m_pTexturesCom_Smoke				= nullptr;
	CTextures*							m_pTexturesCom_Smoke2 = nullptr;

	_float3*							m_pInstanceBuffer_Smoke_Dir			= nullptr;
	_double*							m_pInstanceBuffer_Smoke_PosReset	= nullptr;
	_double*							m_pInstanceBuffer_Smoke_Spawn = nullptr;

	_double		m_dTexUV_Time = 0.0;

	const _float2 m_vDefaultSize_Smoke				= { 2.f, 2.f };
	const _int m_iInstanceCount_Smoke = 30;
	const _float m_fSmoke_AlphaPower				= 0.5f;
	const XMINT2 m_vTextureUV_Smoke					= { 8, 8 };
	const _double m_fSmoke_PosResetTime				= 3.f;
	const _double m_dTexUV_TimeMax					= 0.02;



public:
	static CEffect_RespawnTunnel_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_RESPAWNTUNNEL_PORTAL_H__
#endif // !__EFFECT_RESPAWNTUNNEL_PORTAL_H__
