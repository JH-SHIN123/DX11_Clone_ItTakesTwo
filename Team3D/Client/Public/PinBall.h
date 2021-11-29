#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CPinBall final : public CDynamic_Env
{
private:
	explicit CPinBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPinBall(const CPinBall& rhs);
	virtual ~CPinBall() = default;

public:
	void ReadyGame();
	void StartGame();
	void PlayerMove();
	void Respawn();

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth();
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;
	CDynamicActor*		m_pDynamicActorCom = nullptr;
	CModel*				m_pAttachBall = nullptr;

	_bool				m_IsStartGame = false;
	_bool				m_IsReady = false;

	_float3				m_RespawnPos;

private:
	void MoveMent(_double dTimaDelta);
	HRESULT Create_DynamicActor();
	HRESULT Create_StaticActor();

public:
	static CPinBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};
END