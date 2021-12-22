#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPhysX final : public CBase
{
	DECLARE_SINGLETON(CPhysX)
private:
	explicit CPhysX() = default;
	virtual ~CPhysX() = default;

public: /* Getter */
	PxMaterial* Get_BaseMaterial() { return m_pMaterial; }
	PxPhysics*	Get_PxPhysics() { return m_pPhysics; }

public:
	HRESULT Ready_PhysX(PxSimulationEventCallback* pEventCallback);
	_int	Tick();
	/* For.Actor */
	PxRigidStatic*	Create_StaticActor(PxTransform Transform, PxGeometry& Geometry);
	PxRigidStatic*	Create_TriggerActor(PxTransform Transform, PxGeometry& Geometry);
	PxRigidDynamic*	Create_DynamicActor(PxTransform Transform, PxGeometry& Geometry, _float fDensity, const PxVec3 vVelocity);
	PxController*	Create_CapsuleController(PxCapsuleControllerDesc CapsuleControllerDesc);
	void			Remove_Actor(PxRigidStatic** ppActor);
	void			Remove_Actor(PxRigidDynamic** ppActor);
	/* For.Shape */
	PxTriangleMesh*	Create_Mesh(MESHACTOR_DESC pMeshActorDesc);
	/* For.Fuction */
	_bool			Raycast(const PxVec3& origin, const PxVec3& unitDir, const PxReal distance, PxRaycastCallback& hitCall, PxHitFlags hitFlags = PxHitFlags(PxHitFlag::eDEFAULT), const PxQueryFilterData& filterData = PxQueryFilterData(), PxQueryFilterCallback* filterCall = NULL, const PxQueryCache* cache = NULL);

	PxSphericalJoint*		Create_Joint(PxRigidActor* Actor1, PxTransform Transform1, PxRigidActor* Actor2, PxTransform Transform2, PxJointLimitCone Limitcone, _bool bLimitEnabled = true);
	PxDistanceJoint*		Create_Joint(PxRigidActor* Actor1, PxTransform Transform1, PxRigidActor* Actor2, PxTransform Transform2, _float fDistance);
	void					Create_Ground();
	void					Resize_Actor(PxRigidActor* pActor, _float fScale);


private:
	PxDefaultAllocator			m_Allocator;
	PxDefaultErrorCallback		m_ErrorCallback;
	PxFoundation*				m_pFoundation = nullptr;
	PxPhysics*					m_pPhysics = nullptr;
	PxDefaultCpuDispatcher*		m_pDispatcher = nullptr;
	PxScene*					m_pScene = nullptr;
	PxCooking*					m_pCooking = nullptr;
	PxControllerManager*		m_pControllerManager = nullptr;
	PxMaterial*					m_pMaterial = nullptr;
private:
	void Set_TriggerOption(PxRigidActor* pActor);
	void Set_ShapeOption(PxRigidActor* pActor, _float fContactOffset, _float fRestOffset);
	void Set_DynamicOption(PxRigidDynamic* pActor, _float fAngularDamping, _float LinearDamping, PxVec3 vLinearVelocity);

public:
	virtual void Free() override;

#ifdef _DEBUG
	PxPvd*			m_pPVD = nullptr;
	PxPvdTransport* m_pTransport = nullptr;
#endif
};

END