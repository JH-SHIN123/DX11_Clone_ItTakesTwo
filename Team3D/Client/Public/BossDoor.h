#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CBossDoor final : public CDynamic_Env
{
private:
	explicit CBossDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBossDoor(const CBossDoor& rhs);
	virtual ~CBossDoor() = default;

public:
	_bool Get_DoorClose() { return m_bClose; }

public:
	void Set_DoorClose();
	void Set_DoorUp(_float fMaxDistance, _float fSpeed);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CDynamicActor*	m_pDynamicActorCom = nullptr;

	_bool			m_bCloseDoor = false;
	_bool			m_bGoUp = false;
	_bool			m_bClose = false;

	_float			m_fSpeed = 0.f;
	_float			m_fDistance = 0.f;
	_float			m_fMaxY = 0.f;
	_float3			m_vMaxPos = {};

private:
	void Movement(_double dTimeDelta);
	void Close_Door(_double dTimeDelta);
	void GoUp(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CBossDoor*	  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*  Clone_GameObject(void* pArg) override;
	virtual void		  Free() override;
};
END