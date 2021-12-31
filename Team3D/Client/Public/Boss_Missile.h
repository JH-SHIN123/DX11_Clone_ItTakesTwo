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
			: IsTarget_Cody(IsTarget_Cody) {}

		tagBossMissile_Desc(_bool IsTarget_Cody, _float4 vPos)
			: IsTarget_Cody(IsTarget_Cody), vPosition(vPos) {}
	};
private:
	explicit CBoss_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoss_Missile(const CBoss_Missile& rhs);
	virtual ~CBoss_Missile() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	_bool Get_BossExplosion() const { return m_bBossExplosion; }

public:
	void Set_MissileDead();

private:
	void Combat_Move(_double dTimeDelta);
	void MayControl_Move(_double dTimeDelta);
	void CodyControl_Move(_double dTimeDelta);
	void Falling(_double dTimeDelta);
	void Adjust_Angle(_double dTimeDelta);

private:
	void Explosion_Effect();
	void Set_SmokeEffect(_bool IsActivate);

private:
	_double m_dControlLifeDeltaT = 0.0; // 탔을때부터 시간제한
	_bool	m_isDropped = false;	/* Se */
	_double m_dDroppedTime = 0.0;	/* Se */	

private:
	GameID::Enum m_eTarget_GameID;

	_bool m_IsTargetCody = false;
	_bool m_IsCrashed = false;
	_bool m_IsFalling = false;
	_bool m_bStartAdjustAngle = false;

	_bool m_IsCodyCollide = false;
	_bool m_IsMayCollide = false;

	_bool m_bMayControl = false;
	_bool m_bCodyControl = false;

	_bool m_bPlayerExplosion = false;
	_bool m_bBossExplosion = false;
	_bool m_IsCollide_Wall_Floor = false;

	_float m_fAttackTime = 0.f;
	_float m_fSpeed = 0.f;
	_float m_fRotateAcceleration = 0.f;
	_float m_fMoveAcceleration = 0.f;
	_float m_fCollideTime = 0.f;
	_float m_fTestDegree = 0.f;

	UI::INTERACTIVE_ID m_eInterActiveID = UI::INTERACTIVE_ID_END;
	PxRaycastBuffer	m_MissileRaycastBuffer;

	_vector m_vPlayerOffSetPosition = XMVectorZero();

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;

	PxRaycastBuffer		m_RaycastBuffer;

private:
	class CEffect_Boss_Missile_Smoke_Black* m_pEffect_Smoke_1 = nullptr;
	class CEffect_Boss_Missile_Smoke_Color* m_pEffect_Smoke_2 = nullptr;

public:
	static CBoss_Missile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __BOSS_MISSILE_H__
#endif
