#pragma once

#ifndef __EFFECT_CODY_SIZE_ML_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Cody_Size_ML final : public CInGameEffect
{
private:
	explicit CEffect_Cody_Size_ML(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Cody_Size_ML(const CEffect_Cody_Size_ML& rhs);
	virtual ~CEffect_Cody_Size_ML() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();

public:
	static CEffect_Cody_Size_ML* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};

END
#define __EFFECT_CODY_SIZE_ML_H__
#endif
