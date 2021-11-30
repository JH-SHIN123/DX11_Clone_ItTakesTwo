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
	_bool Get_Failed()	  { return m_bFailed; };
	_bool Get_StartGame() { return m_IsStartGame; }
	_bool Get_Ready()	  { return m_IsReady; }

public:
	void Goal(_fvector vGatePosition);
	void ReadyGame();
	void PlayerMove();
	void StartGame();
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
	virtual void	OnContact(ContactStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	CTriggerActor*		m_pTriggerActorCom = nullptr;
	CDynamicActor*		m_pDynamicActorCom = nullptr;
	/* 문 붙어있는 모델 */
	CModel*				m_pAttachBall = nullptr;

	_bool				m_bFailed = false;
	_bool				m_IsStartGame = false;
	_bool				m_IsReady = false;
	_float3				m_RespawnPos;

private:
	void MoveMent(_double dTimaDelta);

public:
	static CPinBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};
END