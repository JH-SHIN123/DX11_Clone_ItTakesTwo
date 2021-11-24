#include "stdafx.h"
#include "..\Public\CameraBehaviorCallback.h"
#include "PhysX.h"

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	

	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}