#pragma once

#ifndef __EFFECT_WORMHOLE_H__

#include "InGameEffect_Model.h"

BEGIN(Engine)
class CTriggerActor;
END

BEGIN(Client)
class CEffect_Wormhole final : public CInGameEffect_Model
{
private:
	explicit CEffect_Wormhole(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Wormhole(const CEffect_Wormhole& rhs);
	virtual ~CEffect_Wormhole() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	HRESULT Ready_Instance();

private:
	CTextures* m_pTexturesCom_ColorRamp = nullptr;
	_float m_fTime = 3.5f;
	_float m_fRadianAngle = 0.f;

public:
	static CEffect_Wormhole* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_WORMHOLE_H__
#endif // !__EFFECT_WORMHOLE_H__
