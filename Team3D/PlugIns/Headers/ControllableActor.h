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

		tagArgumentDesc() {}
		tagArgumentDesc(class CTransform* _pTransform, PxCapsuleControllerDesc _CapsuleControllerDesc, _float _fJumpGravity) : pTransform(_pTransform), CapsuleControllerDesc(_CapsuleControllerDesc), fJumpGravity(_fJumpGravity) {}
	}ARG_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void	Move(_fvector vMove, _double dTimeDelta);
	void	Update(_double dTimeDelta);
	void	Update_Cam(_double dTimeDelta);
	void	Jump_Start(_float fJumpForce);
	void	Jump_Higher(_float fJumpForce);
<<<<<<< HEAD
	PxController* Get_Controller() { return m_pController; }
=======

	/* Setter */
	void	Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
	void    Set_Jump(_bool bJump) { m_bJump = bJump; }

	/* Getter */
	_bool  Get_IsJump() { return m_bJump; }

>>>>>>> origin/YYY_RealPlayer
private:
	PxController*					m_pController = nullptr;
	PxRigidDynamic*					m_pActor = nullptr;
	class CTransform*				m_pTransform = nullptr;
	class CPxControllerCallback*	m_pCallback = nullptr;
	/* For.Jump */
	_float	m_fJumpTime = 0.f;
	_float	m_fHeightDelta = 0.f;
	_float	m_fJumpForce = 0.f;
	_float	m_fBaseJumpForce = 0.f;
	_bool	m_bJump = false;

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