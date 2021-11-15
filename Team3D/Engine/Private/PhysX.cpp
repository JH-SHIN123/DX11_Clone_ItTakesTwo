
#include "..\Public\PhysX.h"
#include "PxEventCallback.h"

IMPLEMENT_SINGLETON(CPhysX)

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_CONTACT_POINTS;

	return PxFilterFlag::eDEFAULT;
}

HRESULT CPhysX::Ready_PhysX()
{
	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);
	NULL_CHECK_RETURN(m_pFoundation, E_FAIL);

#ifdef _DEBUG
	m_pPVD = PxCreatePvd(*m_pFoundation);
	NULL_CHECK_RETURN(m_pPVD, E_FAIL);

	m_pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pPVD->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);
	NULL_CHECK_RETURN(m_pPhysics, E_FAIL);

	NULL_CHECK_RETURN(PxInitExtensions(*m_pPhysics, m_pPVD), E_FAIL);
#else
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale());
	NULL_CHECK_RETURN(m_pPhysics, E_FAIL);
#endif

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));
	NULL_CHECK_RETURN(m_pCooking, E_FAIL);

	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	m_pEventCallback = new CPxEventCallback;

	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.f, -GRAVITY, 0.f);
	SceneDesc.simulationEventCallback = m_pEventCallback;
	SceneDesc.cpuDispatcher = m_pDispatcher;
	SceneDesc.filterShader = FilterShader;

	m_pScene = m_pPhysics->createScene(SceneDesc);
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	//m_pScene->setFlag(PxSceneFlag::eENABLE_KINEMATIC_PAIRS, true);
	//m_pScene->setFlag(PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS, true);

#ifdef _DEBUG
	PxPvdSceneClient* pClient = m_pScene->getScenePvdClient();
	NULL_CHECK_RETURN(pClient, E_FAIL);

	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif

	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	// 임시 바닥
	PxMaterial* pMaerial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *pMaerial);
	groundPlane->setName("Ground");
	m_pScene->addActor(*groundPlane);

	return S_OK;
}

_int CPhysX::Tick()
{
	m_pScene->simulate(1.f / 60.f);
	m_pScene->fetchResults(true);

	return NO_EVENT;
}

PxRigidStatic * CPhysX::Create_StaticActor(PxTransform Transform, PxGeometry & Geometry, PxMaterial * pMaterial, const char * pActorName)
{
	PxRigidStatic* pRigidBody = PxCreateStatic(*m_pPhysics, Transform, Geometry, *pMaterial);

	pRigidBody->setName(pActorName);

	return pRigidBody;
}

PxRigidDynamic * CPhysX::Create_DynamicActor(PxTransform Transform, PxGeometry & Geometry, PxMaterial * pMaterial, const char * pActorName, const PxVec3 vVelocity)
{
	PxRigidDynamic* pRigidBody = PxCreateDynamic(*m_pPhysics, Transform, Geometry, *pMaterial, 5.f);

	pRigidBody->setAngularDamping(1.f);
	pRigidBody->setLinearDamping(0.5f);
	pRigidBody->setLinearVelocity(vVelocity);
	pRigidBody->setName(pActorName);

	return pRigidBody;
}

PxController * CPhysX::Create_CapsuleController(PxCapsuleControllerDesc CapsuleControllerDesc)
{
	PxControllerDesc* ControllerDesc = &CapsuleControllerDesc;

	PxController* Controller = static_cast<PxCapsuleController*>(m_pControllerManager->createController(*ControllerDesc));

	return Controller;
}

void CPhysX::Add_ActorToScene(PxActor * pActor)
{
	m_pScene->addActor(*pActor);
}

void CPhysX::Remove_Actor(PxRigidStatic ** pActor)
{
	m_pScene->removeActor(**pActor);
	(*pActor)->release();
	*pActor = nullptr;
}

void CPhysX::Remove_Actor(PxRigidDynamic ** pActor)
{
	m_pScene->removeActor(**pActor);
	(*pActor)->release();
	*pActor = nullptr;
}

PxTriangleMesh* CPhysX::Create_Mesh(MESHACTOR_DESC pMeshActorDesc)
{
	PxTriangleMeshDesc MeshDesc;
	MeshDesc.points.count		= pMeshActorDesc.iVertexCount;
	MeshDesc.points.data		= pMeshActorDesc.pVertices;
	MeshDesc.points.stride		= sizeof(_vector);
	MeshDesc.triangles.count	= pMeshActorDesc.iFaceCount;
	MeshDesc.triangles.data		= pMeshActorDesc.pFaces;
	MeshDesc.triangles.stride	= sizeof(POLYGON_INDICES32);

	PxCookingParams Params = m_pCooking->getParams();
	Params.midphaseDesc = PxMeshMidPhase::eBVH33;

	Params.suppressTriangleMeshRemapTable = true;
	Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);

	//Params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	Params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eSIM_PERFORMANCE;
	Params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.55f;

	m_pCooking->setParams(Params);

	PxTriangleMesh* TriMesh = nullptr;

	PxDefaultMemoryOutputStream OutBuffer;
	m_pCooking->cookTriangleMesh(MeshDesc, OutBuffer);

	PxDefaultMemoryInputData stream(OutBuffer.getData(), OutBuffer.getSize());
	TriMesh = m_pPhysics->createTriangleMesh(stream);

	return TriMesh;
}

PxMaterial * CPhysX::Create_Material(PxReal StaticFriction, PxReal DynamicFriction, PxReal Restitution)
{
	PxMaterial* pMaterial = nullptr;

	pMaterial = m_pPhysics->createMaterial(StaticFriction, DynamicFriction, Restitution);
	NULL_CHECK_RETURN(pMaterial, nullptr);

	return pMaterial;
}

void CPhysX::Free()
{
	m_pControllerManager->release();

	m_pScene->release();
	m_pDispatcher->release();

	Safe_Delete(m_pEventCallback);
	PxCloseExtensions();

	m_pCooking->release();
	m_pPhysics->release();
#ifdef _DEBUG
	m_pPVD->release();
	m_pTransport->release();
#endif
	m_pFoundation->release();
}

// 단방향 상호작용
//	m_pScene->setDominanceGroupPair(0, 1, PxDominanceGroupPair(0, 1));
// 쿠킹 파라미터 설정
//void CPhysX::Set_CommomCookingParams(PxCookingParams & Params, _bool bSkipMeshCleanUp, _bool bSkipEdgeData)
//{
//	Params.suppressTriangleMeshRemapTable = true;
//
//	if (false == bSkipMeshCleanUp)
//		Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
//	else
//		Params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
//
//	if (false == bSkipEdgeData)
//		Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
//	else
//		Params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
//}
// 메시 액터 생성
//PxRigidStatic * CPhysX::Create_MeshActor(MESHACTOR_DESC pMeshActorDesc, PxMaterial * pMaterial)
//{
//	PxTriangleMeshDesc MeshDesc;
//	MeshDesc.points.count = pMeshActorDesc.iVertexCount;
//	MeshDesc.points.data = pMeshActorDesc.pVertices;
//	MeshDesc.points.stride = sizeof(_vector);
//	MeshDesc.triangles.count = pMeshActorDesc.iFaceCount;
//	MeshDesc.triangles.data = pMeshActorDesc.pFaces;
//	MeshDesc.triangles.stride = sizeof(POLYGON_INDICES32);
//
//	PxCookingParams Params = m_pCooking->getParams();
//	Params.midphaseDesc = PxMeshMidPhase::eBVH33;
//
//	Set_CommomCookingParams(Params, false, false);
//	//Params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
//	Params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eSIM_PERFORMANCE;
//	Params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.55f;
//
//	m_pCooking->setParams(Params);
//
//	PxTriangleMesh* TriMesh = nullptr;
//	PxU32 iMeshSize = 0;//
//
//	PxDefaultMemoryOutputStream OutBuffer;
//	m_pCooking->cookTriangleMesh(MeshDesc, OutBuffer);
//
//	PxDefaultMemoryInputData stream(OutBuffer.getData(), OutBuffer.getSize());
//	TriMesh = m_pPhysics->createTriangleMesh(stream);
//
//	iMeshSize = OutBuffer.getSize();//
//
//	PxTriangleMeshGeometry Geom(TriMesh);
//	PxRigidStatic* MeshActor = m_pPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
//	MeshActor->createShape(Geom, *pMaterial);
//
//	PxShape* Shape;
//	MeshActor->getShapes(&Shape, 1);
//	Shape->setContactOffset(0.02f);
//	// A negative rest offset helps to avoid jittering when the deformed mesh moves away from objects resting on it.
//	Shape->setRestOffset(-0.5f);
//
//	TriMesh->release();
//
//	return MeshActor;
//}

