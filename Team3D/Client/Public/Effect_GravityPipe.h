#pragma once

#ifndef __EFFECT_GRAVITYPIPE_H__
#include "InGameEffect_Model.h"

BEGIN(Engine)
class CTriggerActor;
END

BEGIN(Client)
class CEffect_GravityPipe final : public CInGameEffect_Model
{
	struct tagGravity_Desc
	{
		_float4 vPosition;
		tagGravity_Desc() {}
		tagGravity_Desc(_float4 vPos)
			: vPosition(vPos){}
	};
	
private:
	explicit CEffect_GravityPipe(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_GravityPipe(const CEffect_GravityPipe& rhs);
	virtual ~CEffect_GravityPipe() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	HRESULT Ready_Instance();

public:
	void Set_Activate(_bool IsActivate);
private:
	_bool	m_IsActivate	= false; // 배터리와 연관이 있는
	_double m_dActivateTime	= 0.f;

	/* Sound */
	_float m_fGravity_Create_Volume = 1.f;
	_float m_fGravity_On_Volume = 1.f;

	_bool m_bCreateSoundOnce = false;
	_bool m_bCreateSecondSoundOnce = false;

private:
	_double m_dAngle = 0.0;
	_float m_fTime = 0.f;
	_float m_fColorRamp_U = 0.f;
	CTextures* m_pTexturesCom_ColorRamp = nullptr;
	CTextures* m_pTexturesCom_Distortion = nullptr;

	/* For.Trigger */
	CTriggerActor*	m_pTriggerCom = nullptr;
	CTransform*		m_pPhysxTransformCom = nullptr;

private:
	class CEffect_Env_Particle* m_pParticle = nullptr;

public:
	static CEffect_GravityPipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_GRAVITYPIPE_H__
#endif
