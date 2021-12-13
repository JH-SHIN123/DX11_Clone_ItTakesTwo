#pragma once
#include "Client_Defines.h"
#include "Dynamic_Env.h"

BEGIN(Client)
class CWall_LaserTennis final : public CDynamic_Env
{
private:
	explicit CWall_LaserTennis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWall_LaserTennis(const CWall_LaserTennis& rhs);
	virtual ~CWall_LaserTennis() = default;

public:/* Setter */
	void Change_State() { m_bMovement = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bActivate = false;
	_bool			m_bMovement = false;

	_float			m_fResetPosY = 0.f;
	_float			m_fDistance = 10.f;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CWall_LaserTennis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END