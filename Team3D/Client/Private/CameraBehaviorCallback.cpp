#include "stdafx.h"
#include "..\Public\CameraBehaviorCallback.h"
#include "PhysX.h"

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	PxRaycastBuffer Buffer;

	_vector vSrc = MH_XMVec3(m_pController->getFootPosition());
	_vector vDst = vSrc + XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (CPhysX::GetInstance()->Raycast(Buffer, vSrc, vDst, 0.5f))
		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}