#include "..\Public\PhysX.h"

IMPLEMENT_SINGLETON(CPhysX)

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);

	if (PxFilterObjectIsTrigger(attributes0) != PxFilterObjectIsTrigger(attributes1))
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	else
	{
		pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_CONTACT_POINTS;
	}

	return PxFilterFlag::eDEFAULT;
}

HRESULT CPhysX::Ready_PhysX(PxSimulationEventCallback* pEventCallback)
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

	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.f, -GRAVITY, 0.f);
	SceneDesc.cpuDispatcher = m_pDispatcher;
	SceneDesc.filterShader = FilterShader;
	SceneDesc.simulationEventCallback = pEventCallback;
	SceneDesc.contactModifyCallback = NULL;
	SceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;

	m_pScene = m_pPhysics->createScene(SceneDesc);
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

#ifdef _DEBUG
	PxPvdSceneClient* pClient = m_pScene->getScenePvdClient();
	NULL_CHECK_RETURN(pClient, E_FAIL);

	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif

	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.f);
	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	return S_OK;
}

_int CPhysX::Tick()
{
	m_pScene->simulate(1.f / 60.f);
	m_pScene->fetchResults(true);

	return NO_EVENT;
}

PxRigidStatic * CPhysX::Create_StaticActor(PxTransform Transform, PxGeometry & Geometry)
{
	PxRigidStatic* pRigidBody = PxCreateStatic(*m_pPhysics, Transform, Geometry, *m_pMaterial);
	NULL_CHECK_RETURN(pRigidBody, nullptr);

	Set_ShapeOption(pRigidBody, 0.02f, -0.5f);

	m_pScene->addActor(*pRigidBody);

	return pRigidBody;
}

PxRigidStatic * CPhysX::Create_TriggerActor(PxTransform Transform, PxGeometry & Geometry)
{
	PxRigidStatic* pRigidBody = PxCreateStatic(*m_pPhysics, Transform, Geometry, *m_pMaterial);
	NULL_CHECK_RETURN(pRigidBody, nullptr);

	Set_TriggerOption(pRigidBody);

	m_pScene->addActor(*pRigidBody);

	return pRigidBody;
}

PxRigidDynamic * CPhysX::Create_DynamicActor(PxTransform Transform, PxGeometry & Geometry, _float fDensity, const PxVec3 vVelocity)
{
	PxRigidDynamic* pRigidBody = PxCreateDynamic(*m_pPhysics, Transform, Geometry, *m_pMaterial, fDensity);
	NULL_CHECK_RETURN(pRigidBody, nullptr);

	Set_DynamicOption(pRigidBody, 0.02f, 0.5f, vVelocity);

	m_pScene->addActor(*pRigidBody);

	return pRigidBody;
}

PxController * CPhysX::Create_CapsuleController(PxCapsuleControllerDesc CapsuleControllerDesc)
{
	PxControllerDesc* ControllerDesc = &CapsuleControllerDesc;

	PxController* Controller = static_cast<PxCapsuleController*>(m_pControllerManager->createController(*ControllerDesc));
	NULL_CHECK_RETURN(Controller, nullptr);

	return Controller;
}

void CPhysX::Remove_Actor(PxRigidStatic ** pActor)
{
	if (nullptr == *pActor) return;

	(*pActor)->userData = nullptr;
	m_pScene->removeActor(**pActor);
	(*pActor)->release();
	*pActor = nullptr;
}

void CPhysX::Remove_Actor(PxRigidDynamic ** pActor)
{
	if (nullptr == *pActor) return;

	(*pActor)->userData = nullptr;
	m_pScene->removeActor(**pActor);
	(*pActor)->release();
	*pActor = nullptr;
}

PxTriangleMesh* CPhysX::Create_Mesh(MESHACTOR_DESC pMeshActorDesc)
{
	PxTriangleMeshDesc MeshDesc;
	MeshDesc.points.count		= pMeshActorDesc.iVertexCount;
	MeshDesc.points.data		= pMeshActorDesc.pVertices;
	MeshDesc.points.stride		= sizeof(PxVec3);
	MeshDesc.triangles.count	= pMeshActorDesc.iFaceCount;
	MeshDesc.triangles.data		= pMeshActorDesc.pFaces;
	MeshDesc.triangles.stride	= sizeof(POLYGON_INDICES32);

	PxCookingParams Params = m_pCooking->getParams();
	Params.midphaseDesc = PxMeshMidPhase::eBVH33;

	Params.suppressTriangleMeshRemapTable = true;
	Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	Params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);

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

_bool CPhysX::Raycast(const PxVec3 & origin, const PxVec3 & unitDir, const PxReal distance, PxRaycastCallback & hitCall, PxHitFlags hitFlags, const PxQueryFilterData & filterData, PxQueryFilterCallback * filterCall, const PxQueryCache * cache)
{
	return m_pScene->raycast(origin, unitDir, distance, hitCall, hitFlags, filterData, filterCall, cache);
}

PxSphericalJoint * CPhysX::Create_Joint(PxRigidActor* Actor1, PxTransform Transform1, PxRigidActor* Actor2, PxTransform Transform2, PxJointLimitCone Limitcone, _bool bLimitEnabled)
{
	PxSphericalJoint* pJoint = PxSphericalJointCreate(*m_pPhysics, Actor1, Transform1, Actor2, Transform2);
	if (nullptr == pJoint)
		return nullptr;

	pJoint->setLimitCone(Limitcone);
	pJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, bLimitEnabled);
	return pJoint;
}

PxDistanceJoint * CPhysX::Create_Joint(PxRigidActor * Actor1, PxTransform Transform1, PxRigidActor * Actor2, PxTransform Transform2, _float fDistance)
{
	PxDistanceJoint* pJoint = PxDistanceJointCreate(*m_pPhysics, Actor1, Transform1, Actor2, Transform2);
	if (nullptr == pJoint)
		return nullptr;

	pJoint->setMaxDistance(fDistance);
	pJoint->setMinDistance(fDistance);
	pJoint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	pJoint->setDistanceJointFlag(PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
	return pJoint;
}

void CPhysX::Create_Ground()
{
	// 임시 바닥
	PxMaterial* pMaerial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *pMaerial);
	groundPlane->setName("Ground");
	m_pScene->addActor(*groundPlane);
}

void CPhysX::Set_TriggerOption(PxRigidActor * pActor)
{
	const PxU32 iShapeCount = pActor->getNbShapes();
	PxShape** Shapes = new PxShape*[iShapeCount];
	pActor->getShapes(Shapes, iShapeCount);

	for (PxU32 iIndex = 0; iIndex < iShapeCount; ++iIndex)
	{
		PxShape* pShape = Shapes[iIndex];
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	}

	Safe_Delete_Array(Shapes);
}

void CPhysX::Set_ShapeOption(PxRigidActor * pActor, _float fContactOffset, _float fRestOffset)
{
	const PxU32 iShapeCount = pActor->getNbShapes();
	PxShape** Shapes = new PxShape*[iShapeCount];
	pActor->getShapes(Shapes, iShapeCount);

	for (PxU32 iIndex = 0; iIndex < iShapeCount; ++iIndex)
	{
		PxShape* pShape = Shapes[iIndex];
		pShape->setContactOffset(fContactOffset);
		pShape->setRestOffset(fRestOffset);
	}

	Safe_Delete_Array(Shapes);
}

void CPhysX::Set_DynamicOption(PxRigidDynamic * pActor, _float fAngularDamping, _float LinearDamping, PxVec3 vLinearVelocity)
{
	pActor->setAngularDamping(fAngularDamping);
	pActor->setLinearDamping(LinearDamping);
	pActor->setLinearVelocity(vLinearVelocity);
}

void CPhysX::Free()
{
	m_pControllerManager->release();

	m_pScene->release();
	m_pDispatcher->release();

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
// 머티리얼 생성
// PxMaterial* pMaterial = m_pPhysics->createMaterial(StaticFriction, DynamicFriction, Restitution);
