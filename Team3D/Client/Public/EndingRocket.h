#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#define ENDING_ROCKET_SPEED			25.f 
#define ENDING_ROCKET_BOOSTSPEED	70.f

BEGIN(Client)
class CEndingRocket final : public CGameObject
{

private:
	explicit CEndingRocket(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEndingRocket(const CEndingRocket& rhs);
	virtual ~CEndingRocket() = default;

public:
	/* 부스트 세팅 */
	void Set_Boost() { m_bBoost = true; m_fCurSpeed = ENDING_ROCKET_BOOSTSPEED; }
	_bool Get_Boost() { return m_bBoost; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	void Ready_Players(_double dTimeDelta);
	void Movement(_double dTimeDelta);

private:
	_float	m_fLeft = 0.f;
	_float	m_fRight = 0.f;
	_float	m_fUp = 0.f;
	_float	m_fDown = 0.f;
	_float	m_fAcceleration = 0.f;

	_bool	m_bBoost = false;
	_double m_dBoostTime = 0.0;
	_float	m_fCurSpeed = 0.f;
	_bool	m_bStartMove = false;
	_bool m_IsBoostEffect = false;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom			= nullptr;
	CTransform*			m_pTransformCom			= nullptr;
	CModel*				m_pModelCom				= nullptr;
	CTriggerActor*		m_pTriggerActorCom		= nullptr;

private: // Effect
	class CEffect_EndingRocket_Smoke*  m_pEffect_Boost  = nullptr;
	class CEffect_EndingRocket_Circle* m_pEffect_Cirlce = nullptr;

public:
	static CEndingRocket* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
