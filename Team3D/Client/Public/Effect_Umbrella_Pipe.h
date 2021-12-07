#pragma once

#ifndef __EFFECT_UMBRELLA_PIPE_H__

#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_Umbrella_Pipe final : public CInGameEffect_Model
{
private:
	explicit CEffect_Umbrella_Pipe(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Umbrella_Pipe(const CEffect_Umbrella_Pipe& rhs);
	virtual ~CEffect_Umbrella_Pipe() = default;

public:
	void Set_ParentWorldMatrix(_matrix ParentMatrix);

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

public:
	HRESULT Ready_Instance();
	void Set_Activate(_bool IsActivate);

private:
	void Activate_Check(_double TimeDelta);

private:
	_double		m_dAngle = 0.0;
	_bool		m_IsActivate = true;
	_float4x4	m_matParent;

private:
	_double m_dActivateTime = 0.f;
	_float m_fTime = 0.f;
	_float m_fColorRamp_U = 0.f;
	CTextures* m_pTexturesCom_Diffuse = nullptr;
	CTextures* m_pTexturesCom_ColorRamp = nullptr;
	CTextures* m_pTexturesCom_Distortion = nullptr;

private:
	class CEffect_Env_Particle* m_pParticle = nullptr;

public:
	static CEffect_Umbrella_Pipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_UMBRELLA_PIPE_H__
#endif // !__EFFECT_UMBRELLA_PIPE_H__
