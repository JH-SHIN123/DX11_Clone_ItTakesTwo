#pragma once
#include "Client_Defines.h"

BEGIN(Client)
#define UFOFORCE 24000.f
class CMoonUFO : public CGameObject
{
protected:
	explicit CMoonUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonUFO(const CMoonUFO& rhs);
	virtual ~CMoonUFO() = default;

public:/* Getter */
	CTransform*		Get_Transform() { return m_pTransformCom; }
	CModel*			Get_Model()		{ return m_pModelCom; }
	CDynamicActor*	Get_Actor() { return m_pDynamicActorCom; }
	_float4 Get_LaserStartPos() const { return m_vLaserGunPos; }
	_float4 Get_LaserDir() const { return m_vLaserDir; }

public:/* Setter */
	void Set_MayInUFO(_bool bCheck) { m_IsMayInUFO = bCheck; }
	void Set_ShootLaser(_bool _bShootLaser) { m_IsShootLaser = _bShootLaser; }
	void Compensate_LaserDir(_bool bCompensate) { m_bCompensate = bCompensate; }
	void Set_CutSceneEnd(_bool IsCutSceneEnd) { m_IsCutSceneEnd = IsCutSceneEnd; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	HRESULT Set_MeshRenderGroup();
	HRESULT Add_GameObject_ToRenderGroup();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	CDynamicActor*			m_pDynamicActorCom = nullptr;
	PxSphericalJoint*		m_pJoint = nullptr;

	_bool					m_IsMayInUFO = false;
	_bool					m_bRotateRight = false;
	_bool					m_bRotateLeft = false;
	_bool					m_bCompensate = false;
	
	/* CutScene */
	_bool m_IsCutSceneEnd = false;


	// YYY
	/* For. Laser_TypeA */
	_float4				m_vLaserGunPos = {};
	_float4				m_vLaserDir = {};
	_bool				m_IsShootLaser = false;

private:
	void	KeyInPut(_double dTimeDelta);
	void	Calculate_Matrix(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CMoonUFO*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void         Free() override;
};
END