#include "..\Public\PhysX.h"

IMPLEMENT_SINGLETON(CPhysX)

HRESULT CPhysX::Ready_PhysX()
{
	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);
	NULL_CHECK_RETURN(m_pFoundation, E_FAIL);

#ifdef _DEBUG
	m_pPVD = PxCreatePvd(*m_pFoundation);
	NULL_CHECK_RETURN(m_pPVD, E_FAIL);

	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pPVD->connect(*pTransport, PxPvdInstrumentationFlag::eALL);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);
	NULL_CHECK_RETURN(m_pPhysics, E_FAIL);

	NULL_CHECK_RETURN(PxInitExtensions(*m_pPhysics, m_pPVD), E_FAIL);
#else
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale());
	NULL_CHECK_RETURN(m_pPhysics);
#endif

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));
	NULL_CHECK_RETURN(m_pCooking, E_FAIL);

	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);

	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.f, -GRAVITY, 0.f);
	SceneDesc.cpuDispatcher = m_pDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//SceneDesc.simulationEventCallback = &


	return S_OK;
}

void CPhysX::Free()
{
}
