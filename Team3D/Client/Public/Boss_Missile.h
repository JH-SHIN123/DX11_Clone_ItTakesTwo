#pragma once

#ifndef __BOSS_MISSILE_H__

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)

END

BEGIN(Client)
class CBoss_Missile final : public CGameObject
{
public:
	struct tagBossMissile_Desc
	{
		_bool IsTarget_Cody = true; //false == May
		_float4 vPosition = { 0.f,0.f,0.f,1.f };

		tagBossMissile_Desc() {}
		tagBossMissile_Desc(_bool IsTarget_Cody)
			: IsTarget_Cody(IsTarget_Cody){}

		tagBossMissile_Desc(_bool IsTarget_Cody, _float4 vPos)
			: IsTarget_Cody(IsTarget_Cody), vPosition(vPos){}
	};
private:
	explicit CBoss_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoss_Missile(const CBoss_Missile& rhs);
	virtual ~CBoss_Missile() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	void Combat_Move(_double TimeDelta);
	void Playable_Mode(_double TimeDelta);

private:
	_int Dead_Explosion();

private:
	_bool m_IsNull_Trigger = false;
	_bool m_IsExplosion = false;

private:
	_float m_fCurvePower = 10.f;
	_float m_fCombatTime = 10.f; // 추적시간
	_bool m_IsCombatMode = true; //
	_bool m_IsTargetCody = false;

	_float m_fUpdateTrigger_Term = 0.f;
	const _float m_fUpdateTrigger_Term_Max = 1.f;
	_bool m_IsUpadate_Trigger = false;


private: //
	_bool m_IsPlayable_Ready = false;
	_bool m_IsPlayable = false;

	GameID::Enum m_eTarget_GameID;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom			= nullptr;
	CTransform*			m_pTransformCom			= nullptr;
	CModel*				m_pModelCom				= nullptr;

	CTransform*			m_pTransformCom_Actor	= nullptr;
	CTriggerActor*		m_pTriggerCom_Combat	= nullptr;
	CTriggerActor*		m_pTriggerCom_Playable	= nullptr;

private:
	CGameObject* m_pTargetObject = nullptr;

public:
	static CBoss_Missile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __BOSS_MISSILE_H__
#endif
