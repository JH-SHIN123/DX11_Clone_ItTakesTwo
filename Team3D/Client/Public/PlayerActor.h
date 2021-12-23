#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CPlayerActor final : public CActor
{
private:
	explicit CPlayerActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerActor(const CPlayerActor& rhs);
	virtual ~CPlayerActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*		pTransform;
		PxCapsuleControllerDesc	CapsuleControllerDesc;
		_float					fJumpGravity;
		USERDATA*				pUserData;
	}ARG_DESC;

	enum PLAYER_TYPE { PLAYER_CODY, PLAYER_MAY, PLAYER_END};

public: /* Getter */
	_bool  Get_IsJump() { return m_bJump; }
	_bool  Get_IsFalling() { return m_IsFalling; }
	PxExtendedVec3 Get_ContactPos() { return m_vContactPosition; }

	PxController* Get_Controller() { return m_pController; }
	PxRigidDynamic* Get_Actor() { return m_pActor; }
	_bool	Get_IsWallCollide() { return m_IsWallCollide; }
	PxVec3 Get_CollideNormal() { return m_vCollideNormal; }
	_bool	Get_IsOnGravityPath() { return m_IsOnGravityPath; }
	_bool	Get_IsPlayerInUFO() { return m_IsPlayerInUFO; }
	PLAYER_TYPE Get_Player_Type() { return m_ePlayerType; }
	PxVec3 Get_GravityNormal() { return m_vHitNormal; }

public: /* Setter */
	void	Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
	void    Set_Jump(_bool bJump) { m_bJump = bJump; }
	void	Set_Scale(_float fRadius, _float fHeight);
	void	Set_GroundPound(_bool bGroundPound) { m_bGroundPound = bGroundPound; }
	void    Set_ZeroGravity(_bool bZeroGravity, _bool IsGoUp, _bool IsStatic) { m_bZeroGravity = bZeroGravity; m_IsGoUp = IsGoUp; m_bStatic = IsStatic; }
	void	Set_IsFalling(_bool IsFalling) { m_IsFalling = IsFalling; }
	void	Set_Position(_fvector vPosition);
	void	Set_IsOnGravityPath(_bool bOnGravityPath) { m_IsOnGravityPath = bOnGravityPath; }
	void	Set_IsPlayerSizeSmall(_bool _IsPlayerSizeSmall) { m_IsPlayerSizeSmall = _IsPlayerSizeSmall; }
	void	Set_IsPlayerInUFO(_bool _IsPlayerInUFO) { m_IsPlayerInUFO = _IsPlayerInUFO; }
	void	Set_PlayerType(CPlayerActor::PLAYER_TYPE _ePlayerType) { m_ePlayerType = _ePlayerType; }
	void	Set_ShapeFlag(_bool bValue);
	void	Set_PlayerUp(_fvector vPlayerUp) { XMStoreFloat3(&m_vPlayerUp, vPlayerUp); }
	void	Set_Gravity_Normally();

	/* Wall */
	void	Set_ContactPos(PxExtendedVec3 vPosition) { m_vContactPosition = vPosition; }
	void	Set_WallCollide(_bool bWallCollide) { m_IsWallCollide = bWallCollide; }
	void	Set_CollideNormal(PxVec3 vNormal) { m_vCollideNormal = vNormal; }
	void	Set_ReorderGravityStep(_uint iStep) { m_iReorderGravityStep = iStep; }
	void	Set_HitNormal(PxVec3 vNormal) { m_vHitNormal = vNormal; }
	

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void	Move(_fvector vMove, _double dTimeDelta);
	void	Update(_double dTimeDelta);
	void	Jump_Start(_float fJumpForce);
	void	Jump_Higher(_float fJumpForce);
	void	Step_GravityPath();
	void	Reorder_Gravity();
	void	MoveToTarget(PxTransform PxTransform);

private:
	PxController*					m_pController = nullptr;
	PxRigidDynamic*					m_pActor = nullptr;
	PxRigidStatic*					m_pTrigger = nullptr;
	PxControllerFilters*			m_pFilters = nullptr;
	class CTransform*				m_pTransform = nullptr;
	class CPlayerBehaviorCallback*	m_pBehaviorCallback = nullptr;
	class CPlayerFilterCallback*	m_pFilterCallback = nullptr;
	class CPlayerHitReport*			m_pHitReport = nullptr;
	USERDATA*						m_pUserData = nullptr;
	PLAYER_TYPE						m_ePlayerType = PLAYER_END;


	/* For.Jump */
	_float	m_fJumpTime = 0.f;
	_float	m_fHeightDelta = 0.f;
	_float	m_fJumpForce = 0.f;
	_float	m_fBaseJumpForce = 0.f;
	_bool	m_bJump = false;
	_bool	m_bGroundPound = false;
	_bool	m_IsFalling = false;
	_bool   m_bZeroGravity = false;
	_bool	m_IsGoUp = false;
	_bool   m_bStatic = false;
	_uint	m_iReorderGravityStep = 0;
	_bool	m_IsPlayerSizeSmall = false;
	_bool	m_IsPlayerInUFO = false;

	_float  m_fFallingTime = 0.f;

	/* For.Gravity */
	_float	m_fGravity = -9.8f;
	_float3 m_vPlayerUp = _float3(0.f, 0.f, 0.f);
	_bool	m_IsOnGravityPath = false;
	PxVec3	m_vHitNormal;

	/* For.WallClimb */
	_bool	m_IsWallCollide = false;
	PxExtendedVec3 m_vContactPosition = {};
	PxVec3 m_vCollideNormal = {0.f ,0.f ,0.f};

private:
	void	Jump_Stop();
	_float	Get_Height(_double dTimeDelta);

public:
	static CPlayerActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END