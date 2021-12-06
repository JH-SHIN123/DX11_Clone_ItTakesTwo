#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPinBall_Spring final : public CDynamic_Env
{
private:
	explicit CPinBall_Spring(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPinBall_Spring(const CPinBall_Spring& rhs);
	virtual ~CPinBall_Spring() = default;

public:
	void PlayerMove();
	void Respawn_Pos(_double dTimeDelta);

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

	_float			m_fRespawnPosX = 0.f;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CPinBall_Spring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	 Clone_GameObject(void* pArg) override;
	virtual void			 Free() override;
};
END