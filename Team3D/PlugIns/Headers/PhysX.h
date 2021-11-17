#pragma once

#include "Base.h"

BEGIN(Engine)

struct ENGINE_DLL FilterGroup
{
	enum Enum
	{
		ePLAYER = (1 << 0),
		eSTATIC = (1 << 1),
		eDYNAMIC = (1 << 2)
	};
};

class ENGINE_DLL CPhysX final : public CBase
{
	DECLARE_SINGLETON(CPhysX)
private:
	explicit CPhysX() = default;
	virtual ~CPhysX() = default;

public: /* Getter */
	PxMaterial* Get_BaseMaterial() { return m_pMaterial; }

public:
	HRESULT Ready_PhysX();
	_int	Tick();
	/* For.Actor */
	PxRigidStatic*	Create_StaticActor(PxTransform Transform, PxGeometry& Geometry, PxMaterial* pMaterial, const char* pActorName);
	PxRigidDynamic*	Create_DynamicActor(PxTransform Transform, PxGeometry& Geometry, PxMaterial* pMaterial, const char* pActorName, const PxVec3 vVelocity);
	PxController*	Create_CapsuleController(PxCapsuleControllerDesc CapsuleControllerDesc);
	void			Add_ActorToScene(PxActor* pActor);
	void			Remove_Actor(PxRigidStatic** ppActor);
	void			Remove_Actor(PxRigidDynamic** ppActor);
	/* For.Shape */
	PxTriangleMesh*	Create_Mesh(MESHACTOR_DESC pMeshActorDesc);
	/* For.Material */
	PxMaterial*		Create_Material(PxReal StaticFriction, PxReal DynamicFriction, PxReal Restitution);
	/* For.Raycast */
	_bool			Raycast(PxRaycastBuffer& RaycastHit, _fvector vSrc, _fvector vDst, _float fDist);

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
	class CPxEventCallback*		m_pEventCallback = nullptr;
	class CPxContactCallback*	m_pContactCallback = nullptr;

public:
	virtual void Free() override;

#ifdef _DEBUG
	PxPvd*			m_pPVD = nullptr;
	PxPvdTransport* m_pTransport = nullptr;
#endif
};

END
