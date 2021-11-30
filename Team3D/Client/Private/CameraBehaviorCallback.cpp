#include "stdafx.h"
#include "..\Public\CameraBehaviorCallback.h"
#include "PhysX.h"
#include "CameraActor.h"

PxControllerBehaviorFlags CCameraBehaviorCallback::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	if (nullptr != m_pCameraActor)
	{
		USERDATA* pUserData = (USERDATA*)(actor.userData);
		
		if (nullptr == pUserData)
			return PxControllerBehaviorFlags();

		if (pUserData->eID != GameID::eCODY && pUserData->eID != GameID::eMAY)
   			m_pCameraActor->Set_IsCollision(true);
		else
			m_pCameraActor->Set_IsCollision(false);
	}

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