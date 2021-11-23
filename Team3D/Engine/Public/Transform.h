#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public: /* Enum */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public: /* Struct */
	typedef struct tagTransformDesc
	{
		_double	dSpeedPerSec	= 0.0;
		_double dRotationPerSec	= 0.0;

		tagTransformDesc() {}
		tagTransformDesc(_double InSpeedPerSec, _double InRotationPerSec) : dSpeedPerSec(InSpeedPerSec), dRotationPerSec(InRotationPerSec) {}
	}TRANSFORM_DESC;

public: /* Getter */
	_vector	Get_State(STATE eState) const;
	_float	Get_Scale(STATE eSTate) const;
	_matrix	Get_WorldMatrix() const;

public: /* Setter */
	void	Set_State(STATE eState, _fvector vState);
	void	Set_Scale(_fvector vScale);
	void	Set_RotateAxis(_fvector vAxis, _float fRadian);
	void	Set_WorldMatrix(_fmatrix WorldMatrix);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	/* For.Transform */
	void	Go_Straight(_double dTimeDelta);
	void	Go_Backward(_double dTimeDelta);
	void	Go_Right(_double dTimeDelta);
	void	Go_Left(_double dTimeDelta);
	void	Go_Up(_double dTimeDelta);
	void	Go_Down(_double dTimeDelta);
	void	Rotate_Axis(_fvector vAxis, _double dTimeDelta);
	void	Rotate_ToTarget(_fvector vTargetPos);
	void	Rotate_ToTargetOnLand(_fvector vTargetPos);
	void	Move_ToTarget(_fvector vTargetPos, _double dTimeDelta);

	void    RotateYawDirectionOnLand(const _fvector& vMoveDir, const _double TimeDelta, const _double dAcceleration = 1.f, class CNavigation* pNavigation = nullptr);
	void    RotatePitchDirectionOnLand(const _fvector& vMoveDir, const _double TimeDelta, const _double dAcceleration = 1.f, class CNavigation* pNavigation = nullptr);

	void    MoveDirectionOnLand(const _fvector& vMoveDir, const _double TimeDelta, const _double dAcceleration = 1.f, class CNavigation* pNavigation = nullptr);
	void	RotateYaw(const _double TimeDelta);
	void	RotatePitch(const _double TimeDelta);

	void    MoveToDir(const _fvector& vMoveDir, const _double TimeDelta, const _double dAcceleration = 1.f, class CNavigation* pNavigation = nullptr);


private:
	_float4x4		m_WorldMatrix;
	TRANSFORM_DESC	m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END