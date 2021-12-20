#pragma once
#ifndef __EFFECT_POINTLIGHT_H__

#include "InGameEffect.h"
#include "Effect_Generator.h"

BEGIN(Client)
class CEffect_PointLight final : public CInGameEffect
{
private:
	explicit CEffect_PointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_PointLight(const CEffect_PointLight& rhs);
	virtual ~CEffect_PointLight() = default; public:

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
	void Set_Pos(_float3 vPos);
	void Set_Radius(_float vRadius);
	void Set_Color(EPoint_Color eColorRamp);

private:
	HRESULT Ready_InstanceBuffer();

private:
	CEffect_Generator::Effect_PointLight_Desc m_PointLight_Desc;

public:
	static CEffect_PointLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_POINTLIGHT_H__
#endif // !__EFFECT_POINTLIGHT_H__
