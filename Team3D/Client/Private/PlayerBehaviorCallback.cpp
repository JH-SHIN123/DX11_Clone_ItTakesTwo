#include "stdafx.h"
#include "..\Public\PlayerBehaviorCallback.h"

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	PxRaycastBuffer Buffer;

	if (CPhysX::GetInstance()->Raycast(MH_PxVec3(m_pController->getFootPosition()), MH_PxVec3(XMVectorSet(0.f, 1.f, 0.f, 0.f)), 0.5f, Buffer))
		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}