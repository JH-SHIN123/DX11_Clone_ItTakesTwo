#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CLaserButtonLarge_Gate final : public CDynamic_Env
{
private:
	explicit CLaserButtonLarge_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserButtonLarge_Gate(const CLaserButtonLarge_Gate& rhs);
	virtual ~CLaserButtonLarge_Gate() = default;

public:
	_bool Get_Active() { return m_bActive; }

public:
	void Change_State() { m_bMovement = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bMovement = false;
	_bool			m_bActive = false;

	_float4x4		m_matResetWorld;
	_float			m_fScale = 1.f;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserButtonLarge_Gate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END