#pragma once

#ifndef __EFFECT_DASH_H__

#include "InGameEffect_Rect.h"

BEGIN(Engine)
class CVIBuffer_Rect_TripleUV;
END

BEGIN(Client)
class CEffect_Dash final : public CInGameEffect_Rect
{
private:
	explicit CEffect_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Dash(const CEffect_Dash& rhs);
	virtual ~CEffect_Dash() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Check_Scale(_double TimeDelta);
	void Check_Move(_double TimeDelta);

public:
	_float m_fAlphaTime = 0.1f;
	_double m_dActivateTime = 0.0;
	_double m_dAngle = 0.0;


public:
	static CEffect_Dash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_DASH_H__
#endif // !__EFFECT_DASH_H__
