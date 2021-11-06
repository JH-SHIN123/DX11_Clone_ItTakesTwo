#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CBehaviorTree;
class CCody final : public CCharacter
{
public:
	enum STATE {
		MH = 1,
		MH_BIGWAITING_ENTER,
		MH_BIGWAITING,
		JOG,
		JOG_LEFT,
		JOG_RIGHT,
		JOG_GOTOSTOP,
		JOG_START_FWD,
		JOG_START_FWD_LEFT,
		JOG_START_FWD_RIGHT,
		JOG_START_INMOTION,
		JOG_STOP_FWD,
		JOG_STOP_FWD_EASY,
		JOG_STOP_FWD_EXH,
		EXH,
		EXH_TO_IDLE,
		SPRINT,
		SPRINT_LEFT,
		SPRINT_RIGHT,
		SPRINT_START_FROM_DASH,
		SPRINT_TURNAROUND,
		SPRINT_JUMP, /*¾È¾¸*/
		STATE_END 
	};

	_bool m_bJog = false;
	_bool m_bJog_Left = false;
	_bool m_bJog_Right = false;
	_bool m_bJog_180 = false;

private:
	explicit CCody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCody(const CCody& rhs);
	virtual ~CCody() = default;

public:
	_bool IsFinish_CurAnimation();

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

private:
	virtual void KeyProcess(_double TimeDelta) override;
	void StateCheck();

private:
	CCamera*		m_pCamera = nullptr;
	CBehaviorTree*	m_pBehaviorTree = nullptr;

public:
	static CCody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END