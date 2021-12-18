#pragma once
#include "Client_Defines.h"

#define MOONBABOONFORCE 10

BEGIN(Client)
class CRunningMoonBaboon : public CGameObject
{
protected:
	explicit CRunningMoonBaboon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRunningMoonBaboon(const CRunningMoonBaboon& rhs);
	virtual ~CRunningMoonBaboon() = default;

public:/* Getter */
	CTransform*		Get_Transform() { return m_pTransformCom; }
	CModel*			Get_Model()		{ return m_pModelCom; }
	CDynamicActor*	Get_Actor() { return m_pDynamicActorCom; }
	_bool			Get_IsHitLaser() { return m_IsLaserHit; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public: /* Setter */
	void Set_LaserHit(_bool IsLaserHit) { m_IsLaserHit = IsLaserHit; }

private:
	void	Movement(_double dTimeDelta);
	void	Calculate_Matrix(_double dTimeDelta);
	void	Add_LerpInfo();
	void	LaserHit_Movement(_double dTimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CDynamicActor*			m_pDynamicActorCom = nullptr;
	PxSphericalJoint*		m_pJoint = nullptr;
	PxRaycastBuffer			m_RaycastBuffer;

	_bool					m_bRotateRight = false;
	_bool					m_bRotateLeft = false;
	_bool					m_bJetPackAnimOnce = false;
	_bool					m_bHitOnceAnim = false;
	_bool					m_IsLaserHit = false;
	_float					m_fJetAcceleration = 0.f;
	_float					m_fShieldTime = 0.f;
	_float					m_fJetPackTime = 0.f;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRunningMoonBaboon*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void         Free() override;
};
END