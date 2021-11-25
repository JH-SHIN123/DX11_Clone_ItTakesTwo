#include "..\Public\PxControllerCallback.h"
#include "PhysX.h"

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	PxRaycastBuffer Buffer;

	if (CPhysX::GetInstance()->Raycast(MH_PxVec3(m_pController->getFootPosition()), MH_PxVec3(XMVectorSet(0.f, 1.f, 0.f, 0.f)), 0.5f, Buffer))
		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}