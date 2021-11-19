#include "..\Public\PxControllerCallback.h"
#include "PhysX.h"

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	PxRaycastBuffer Buffer;

	_vector vSrc = MH_XMVec3(m_pController->getFootPosition());
	_vector vDst = vSrc + XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (CPhysX::GetInstance()->Raycast(Buffer, vSrc, vDst, 0.5f))
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