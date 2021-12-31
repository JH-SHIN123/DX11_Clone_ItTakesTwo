#pragma once

#include "Laser.h"

BEGIN(Client)

class CLaser_TypeA final : public CLaser
{
private:
	explicit CLaser_TypeA(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaser_TypeA(const CLaser_TypeA& rhs);
	virtual	~CLaser_TypeA() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	_double m_dChargingTime = 0.0;
	_bool	m_IsPaticleCreate = true;
	_float	m_fSound = 1.f;

private:
	class CUFO*										m_pBossUFO = nullptr;
	class CMoonUFO*									m_pMoonUFO = nullptr;
	class CEffect_Boss_Laser_Smoke*				m_pLaserSmoke = nullptr;

public:
	static CLaser_TypeA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END