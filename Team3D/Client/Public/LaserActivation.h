#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CLaserActivation final : public CDynamic_Env
{
private:
	explicit CLaserActivation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserActivation(const CLaserActivation& rhs);
	virtual ~CLaserActivation() = default;

public:
	_bool Get_Active() { return m_bActive; }
	_fvector Get_Direction() { return XMLoadFloat3(&m_vDirection); }

public:/* Setter */
	void Change_State() { m_bMovement = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bActive		  = false;
	_bool			m_bMovement		  = false;
	_bool			m_bCreateLaser	  = false;

	_float3			m_vDirection	  = {};
	_float			m_fResetPosY	  = 0.f;
	_float			m_fDistance		  = 1.3f;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserActivation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*     Clone_GameObject(void* pArg);
	virtual void			 Free();
};
END