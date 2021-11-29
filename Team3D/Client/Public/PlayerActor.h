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

public: /* Getter */
	_bool  Get_IsJump() { return m_bJump; }
	_bool  Get_IsFalling() { return m_IsFalling; }

	PxController* Get_Controller() { return m_pController; }
	PxRigidDynamic* Get_Actor() { return m_pActor; }

public: /* Setter */
	void	Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
	void    Set_Jump(_bool bJump) { m_bJump = bJump; }
	void	Set_Scale(_float fRadius, _float fHeight);
	void	Set_GroundPound(_bool bGroundPound) { m_bGroundPound = bGroundPound; }
	void    Set_ZeroGravity(_bool bZeroGravity, _bool IsGoUp, _bool IsStatic) { m_bZeroGravity = bZeroGravity; m_IsGoUp = IsGoUp; m_bStatic = IsStatic; }
	void	Set_IsFalling(_bool IsFalling) { m_IsFalling = IsFalling; }
	void	Set_Position(_fvector vPosition);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void	Move(_fvector vMove, _double dTimeDelta);
	void	Update(_double dTimeDelta);
	void	Jump_Start(_float fJumpForce);
	void	Jump_Higher(_float fJumpForce);
	void	Step_GravityPath(PxVec3 vNormal);
	void	Reorder_Gravity();

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

	_float  m_fFallingTime = 0.f;

	/* For.Gravity */
	_bool	m_isGravityReordered = false;
	_float	m_fGravity = -9.8f;
	_float3 m_vPlayerUp = _float3(0.f, 0.f, 0.f);

private:
	void	Jump_Stop();
	_float	Get_Height(_double dTimeDelta);

public:
	static CPlayerActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END