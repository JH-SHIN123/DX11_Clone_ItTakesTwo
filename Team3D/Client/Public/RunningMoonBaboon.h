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
	_float4 Get_LaserStartPos() const { return m_vLaserGunPos; }
	_float4 Get_LaserDir() const { return m_vLaserDir; }

public:/* Setter */
	void Set_ShootLaser(_bool _bShootLaser) { m_IsShootLaser = _bShootLaser; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CDynamicActor*			m_pDynamicActorCom = nullptr;
	PxSphericalJoint*		m_pJoint = nullptr;

	_bool					m_bRotateRight = false;
	_bool					m_bRotateLeft = false;

	// YYY
	/* For. Laser_TypeA */
	_float4				m_vLaserGunPos = {};
	_float4				m_vLaserDir = {};
	_bool				m_IsShootLaser = false;
	PxRaycastBuffer		m_RaycastBuffer;

private:
	void	Movement(_double dTimeDelta);
	void	Calculate_Matrix(_double dTimeDelta);
	void	Add_LerpInfo();

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRunningMoonBaboon*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void         Free() override;
};
END