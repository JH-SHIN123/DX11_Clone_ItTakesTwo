#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CUFO : public CGameObject
{
private:
	explicit CUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFO(const CUFO& rhs);
	virtual ~CUFO() = default;

public:
	enum UFO_TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };
	enum UFO_PHASE { PHASE_1, PHASE_2, PHASE_3, PHASE_END };
	enum UFO_PATTERN { INTERACTION, LASER, GRAVITATIONALBOMB, GUIDEDMISSILE, GROUNDPOUND, PATTERN_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

	/*Getter*/
	_bool			Get_IsRidLaserGun() { return m_IsLaserGunRid; }

public:
	virtual HRESULT Render_ShadowDepth() override;
	HRESULT Add_GameObject_ToRenderGroup();

public:
	void Set_IsGuidedMissileDeadCheck(_bool IsCheck);
	void Set_UFOAnimation(_uint iAnimIndex, _uint iNextAnimIndex);
	void Set_CodyEnterUFO();
	void Set_CutScene(_bool IsCheck);
	void Set_EndIntroCutScene();
	void Set_MoonBaboonPtr(class CMoonBaboon* pMoonBaboon);
	void Set_Who_Collide_Last(GameID::Enum WhoCollide) { m_WhoCollide = WhoCollide; }
	/* For. BossFloorUp */
	void Set_BossUFOUp(_float fMaxDistance, _float fSpeed);
	HRESULT Set_MeshRenderGroup();
	void Set_BossHpBarReduction(_float fDamage);
	void Set_MissilePtrReset(_bool IsTargetCheck);
	void Set_GuidedMissileIncreaseHitCount();
	void Set_Active(_bool IsActive);
	void Set_HpBarActive(_bool IsActive);

public:
	CTransform* Get_Transform() const { return m_pTransformCom; }
	CModel* Get_Model() const { return m_pModelCom; }
	_float4 Get_LaserStartPos() const { return m_vLaserGunPos; }
	_float4 Get_LaserDir() const { return m_vLaserDir; }
	UFO_PHASE Get_BossPhase() const { return m_ePhase; }
	_bool	 Get_IsCutScene() const { return m_IsCutScene; }
	UFO_PATTERN Get_BossPatern() const { return m_ePattern; }
	_bool	Get_Phase2InterActive() const { return m_IsPhase2InterActive; }
	_bool	Get_AdJustPosition() const { return m_IsAdJustPosition; }

private:
	UFO_TARGET				m_eTarget = TARGET_END;
	UFO_PHASE				m_ePhase = PHASE_END;
	UFO_PATTERN				m_ePattern = PATTERN_END;

private:
	GameID::Enum			m_WhoCollide = GameID::eEND;

	/* For.CutScene */
	_bool					m_IsCutScene = false;

	/* For.AllPHASE */
	_float					m_fWaitingTime = 0.f;
	_uint					m_iPatternChangeCount = 0;
	_uint					m_iPhaseChangeCount = 0;
	_bool					m_IsInterActive = false;
	_float4					m_vStartUFOPos;
	_bool					m_IsGoUp = false;
	_float					m_fUpSpeed = 0.f;
	_float3					m_vMaxPos;
	_float					m_fMaxY = 0.f;
	_float					m_fDistance = 0.f;
	_bool					m_IsHit = false;
	_bool					m_IsActive = false;
	_bool					m_IsFirstAngleSetting = true;
	_bool					m_IsCodyCollide = false;
	_bool					m_IsMayCollide = false;

	/* For.PHASE_1 Pattern */
	_bool					m_IsCoreExplosion = false;
	_bool					m_IsStartingPointMove = false;
	_bool					m_IsLaserCreate = true;
	_float					m_fGravitationalBombLanchTime = 0.f;
	_uint					m_iGravitationalBombCount = 0;

	/* For.PHASE_2 Pattern */
	_float					m_fRevAngle = 0.f;
	_float					m_fRotAngle = 0.f;
	_float					m_fGuidedMissileTime = 0.f;
	_float4					m_vTranslationPos;
	_bool					m_IsGuidedMissileCreate = true;
	_bool					m_IsCodyEnter = false;
	_bool					m_IsAnimationTransform = true;
	_bool					m_IsLaserGunRid = false;
	_bool					m_IsActorCreate = true;
	_bool					m_IsTriggerActive = false;
	_bool					m_IsCodySetPos = false;
	_bool					m_IsPhase2InterActive = false;
	_bool					m_IsAdJustPosition = false;
	_uint					m_iGuidedMissileHitCount = 0;
	
	class CBoss_Missile*	m_pCodyMissile = nullptr;
	class CBoss_Missile*	m_pMayMissile = nullptr;

	/* For.PHASE_3 Pattern */
	_float					m_fSubLaserTime = 0.f;
	_float					m_fGroundPoundTime = 0.f;
	_uint					m_iSubLaserIndex = 0;
	_bool					m_IsSubLaserOperation = true;
	_bool					m_IsGroundPound = false;
	_bool					m_IsLastFloor = false;
	_bool					m_IsGoingLastFloor = false;
	_bool					m_IsEjection = false;
	_bool					m_IsLaserDown = false;
	_bool					m_IsGroundPoundEffectCreate = false;
	_float4					m_vGroundPoundTargetPos;

	vector<class CMoonBaboon_SubLaser*>	m_vecSubLaser;

	/* For. Laser_TypeA */
	_float4					m_vLaserGunPos;
	_float4					m_vLaserDir;

	/* For. UI */
	class CBossHpBar*		m_pBossHpBar = nullptr;

	/* For. Sound */		
	_float					m_fLaserSoundVolume = 0.7f;
	_float					m_fCoreSoundVolume = 1.f;
	_float					m_fGravitationalBombSoundVolume = 1.f;
	_float					m_fGroundPoundSoundVolume = 1.f;
	_float					m_fBossMissileSoundVolume = 1.f;
	_bool					m_IsSoundPlayOnce = true;

	/* CutScene Offset */
	_vector					m_vChairOffSetPos = {};

private:
	/* For.Component */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	/* For. StaticActor */
	CStaticActor*			m_pStaticActorCom = nullptr;
	CModel*					m_pStaticModelCom = nullptr;
	CTransform*				m_pStaticTransformCom = nullptr;

	/* For. PlayerTransform */
	CTransform*				m_pCodyTransform = nullptr;
	CTransform*				m_pMayTransform = nullptr;

	/* For. TriggerActorCom */
	CTriggerActor*			m_pTriggerActorCom = nullptr;
	CTransform*				m_pTriggerTransformCom = nullptr;
	CTriggerActor*			m_pEnterTriggerActorCom = nullptr;

	/* For. MoonBaboon */
	class CMoonBaboon*		m_pMoonBaboon = nullptr;

	/* For. Random Script */
	_uint iRandomScript = 0;
	_float m_fScriptDelay = 0.f;
	_uint m_iGroundPoundCount = 0;

private:
	/* For.AllPHASE */
	void GoUp(_double dTimeDelta);
	void Script(_double dTimeDelta);

	/* For.PHASE_1 Pattern */
	void Phase1_Pattern(_double dTimeDelta);
	void Phase1_InterAction(_double dTimeDelta);
	void Laser_Pattern(_double dTimeDelta);
	void MoveStartingPoint(_double dTimeDelta);
	void GravitationalBomb_Pattern(_double dTimeDelta);
	void Core_Destroyed();

	/* For.PHASE_2 Pattern */
	void Phase2_Pattern(_double dTimeDelta);
	void Phase2_InterAction(_double dTimeDelta);
	void OrbitalMovementCenter(_double dTimeDelta);
	void GuidedMissile_Pattern(_double dTimeDelta);
	void SetUp_AnimationTransform();

	/* For.PHASE_3 Pattern */
	void Phase3_Pattern(_double dTimeDelta);
	void Phase3_MoveStartingPoint(_double dTimeDelta);
	void Phase3_InterAction(_double dTimeDelta);
	void GroundPound_Pattern(_double dTimeDelta);
	void DependingTimeSubLaserOperation(_double dTimeDelta);

private:
	/* For.PHASE_1 End */
	HRESULT Phase1_End(_double dTimeDelta);
	HRESULT Ready_TriggerActor_Component();
	HRESULT Ready_StaticActor_Component();
	HRESULT TriggerActorReplacement();
	HRESULT Phase1_End_Sound();

	/* For.PHASE_2 End */
	HRESULT Phase2_End(_double dTimeDelta);
	void GetRidLaserGun();

	/* For.PHASE_3 End */
	HRESULT Phase3_End(_double dTimeDelta);

private:
	/* For.NativeConstruct */
	void Add_LerpInfo_To_Model();
	HRESULT Ready_Component();
	HRESULT Ready_UI();
	HRESULT SetUp_SoundChannel();


private:
	/* For.SubLaser */
	void Ready_Layer_MoonBaboon_SubLaser(const _tchar * pLayerTag);

#pragma region Light
private:
	CLight* m_pBossLight = nullptr;
#pragma endregion

public:
	static CUFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
