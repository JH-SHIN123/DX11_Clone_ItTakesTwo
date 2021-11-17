#include "..\Public\PxControllerCallback.h"

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	if (!strcmp(actor.getName(), "TileBox"))
	{
		_int i = 0;
	}

	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPxControllerCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}
