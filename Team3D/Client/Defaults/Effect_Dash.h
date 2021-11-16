#pragma once

#ifndef __EFFECT_DASH_H__

#include "InGameEffect_Rect.h"

BEGIN(Engine)
class CVIBuffer_Rect;
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
	virtual HRESULT	Render() override;

public:
	void Check_Scale(_double TimeDelta, _uint iIndex);

public:
	_double m_dAlphaTime = 1.0;
	_float4 m_vWeight = { 0.f, 0.f, 0.125f, 0.125f };
	_float3 m_vScale = { 1.f, 0.f, 5.f };
//	CTextures* m_pTexturesCom_Color = nullptr;

private:
	CVIBuffer_Rect* m_pRectCom = nullptr;

public:
	static CEffect_Dash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_DASH_H__
#endif // !__EFFECT_DASH_H__
