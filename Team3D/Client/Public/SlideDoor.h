#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CSlideDoor final : public CDynamic_Env
{
private:
	explicit CSlideDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSlideDoor(const CSlideDoor& rhs);
	virtual ~CSlideDoor() = default;

public:
	void Open_Door();

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

	_float			m_fDistance = 0.f;
	static _bool	m_bOpen;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CSlideDoor*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void		 Free() override;
};
END