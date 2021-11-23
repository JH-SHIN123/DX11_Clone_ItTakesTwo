#pragma once

#ifndef __EFFECT_GRAVITYPIPE_H__
#include "InGameEffect_Model.h"

BEGIN(Engine)
class CTriggerActor;
END

class CEffect_GravityPipe final : public CInGameEffect_Model
{
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

private:
	_double m_dAngle = 0.0;
	_float m_fTime = 0.f;
	_float m_fColorRamp_U = 0.f;
	CTextures* m_pTexturesCom_ColorRamp = nullptr;
	CTextures* m_pTexturesCom_Distortion = nullptr;

	/* For.Trigger */
	CTriggerActor*	m_pTriggerCom = nullptr;
	CTransform*		m_pPhysxTransformCom = nullptr;


public:
	static CEffect_GravityPipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

#define __EFFECT_GRAVITYPIPE_H__
#endif
