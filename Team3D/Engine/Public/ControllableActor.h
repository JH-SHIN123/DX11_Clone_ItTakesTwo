#pragma once

#include "Actor.h"

BEGIN(Engine)

class ENGINE_DLL CControllableActor final : public CActor
{
private:
	explicit CControllableActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CControllableActor(const CControllableActor& rhs);
	virtual ~CControllableActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*		pTransform;
		PxCapsuleControllerDesc	CapsuleControllerDesc;
		_float					fJumpGravity;
		USERDATA*				pUserData;
	}ARG_DESC;
	PxController* Get_Controller() { return m_pController; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void	Move(_fvector vMove, _double dTimeDelta);
	void	Update(_double dTimeDelta);
	void	Update_Cam(_double dTimeDelta);
	void	Jump_Start(_float fJumpForce);
	void	Jump_Higher(_float fJumpForce);

	//void	Kinematic_On() { m_pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); }
	//void	Kinematic_Off() { m_pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); }

	/* Setter */
	void	Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
	void    Set_Jump(_bool bJump) { m_bJump = bJump; }
	void	Set_GroundPound(_bool bGroundPound) { m_bGroundPound = bGroundPound; }

	/* Getter */
	_bool  Get_IsJump() { return m_bJump; }
	_bool  Get_IsFalling() { return m_IsFalling; }

private:
	PxController*						m_pController = nullptr;
	PxRigidDynamic*						m_pActor = nullptr;
	PxRigidStatic*						m_pTrigger = nullptr;
	PxControllerFilters*				m_pFilters = nullptr;
	class CTransform*					m_pTransform = nullptr;
	class CPxControllerCallback*		m_pCallback = nullptr;
	class CPxControllerFilterCallback*	m_pFilterCallback = nullptr;
	/* For.Jump */
	_float	m_fJumpTime = 0.f;
	_float	m_fHeightDelta = 0.f;
	_float	m_fJumpForce = 0.f;
	_float	m_fBaseJumpForce = 0.f;
	_bool	m_bJump = false;
	_bool	m_bGroundPound = false;
	_bool	m_IsFalling = false;

	_float  m_fFallingTime = 0.f;


	/* For.Gravity */
	_float m_fGravity = -9.8f;

private:
	void	Jump_Stop();
	_float	Get_Height(_double dTimeDelta);

public:
	static CControllableActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END