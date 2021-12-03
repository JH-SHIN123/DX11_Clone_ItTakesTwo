#pragma once

#ifndef __EFFECT_PINBALL_MOVE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Pinball_Move final : public CInGameEffect
{
private:
	explicit CEffect_Pinball_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Pinball_Move(const CEffect_Pinball_Move& rhs);
	virtual ~CEffect_Pinball_Move() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;


public:
	static CEffect_Pinball_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};

END
#define __EFFECT_PINBALL_MOVE_H__
#endif // !__EFFECT_PINBALL_MOVE_H__
