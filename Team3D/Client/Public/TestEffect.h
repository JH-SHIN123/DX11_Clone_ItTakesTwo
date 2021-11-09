#pragma once
#include "GameEffect.h"
class CTestEffect final : public CGameEffect
{
private:
	explicit CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTestEffect(const CTestEffect& rhs);
	virtual ~CTestEffect() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	static CTestEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};

