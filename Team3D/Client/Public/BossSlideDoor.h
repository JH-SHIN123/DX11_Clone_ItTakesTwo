#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CBossSlideDoor final : public CDynamic_Env
{
private:
	explicit CBossSlideDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBossSlideDoor(const CBossSlideDoor& rhs);
	virtual ~CBossSlideDoor() = default;

public:
	void Set_DoorClose();

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	static _bool			m_bCloseDoor;
	_float					m_fSpeed = 0.f;
	_float					m_fDistance = 0.f;
	_float					m_fMaxY = 0.f;
	_float3					m_vMaxPos = {};

private:
	void Close_Door(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CBossSlideDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*  Clone_GameObject(void* pArg) override;
	virtual void		  Free() override;
};
END