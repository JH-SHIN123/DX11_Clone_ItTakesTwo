#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPinBall_BallDoor final : public CDynamic_Env
{
private:
	explicit CPinBall_BallDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPinBall_BallDoor(const CPinBall_BallDoor& rhs);
	virtual ~CPinBall_BallDoor() = default;

public:
	_bool Get_Finish() { return m_bFinish; }

public:
	/* false:Open true:Close */
	void Set_DoorState(_bool bDoorState);
	void Set_Render(_bool bRender) { m_bRender = bRender; }

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

	_bool			m_bReady = false;
	_bool			m_bDoorState = false;
	_bool			m_bRender = true;
	_bool			m_bFinish = false;
	_float			m_fAngle = 0.f;

private:
	void MoveMent(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static	CPinBall_BallDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	   Clone_GameObject(void* pArg) override;
	virtual void			   Free() override;
};
END