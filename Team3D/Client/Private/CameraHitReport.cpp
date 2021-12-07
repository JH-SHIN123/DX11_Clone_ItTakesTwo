#include "stdafx.h"
#include "..\Public\CameraHitReport.h"
#include "CameraActor.h"

void CCameraHitReport::onShapeHit(const PxControllerShapeHit & hit)
{
	USERDATA* pUserData = (USERDATA*)hit.actor->userData;

	if (nullptr == pUserData) return;
	
	if (hit.triangleIndex != PxU32(-1))
	{
		m_pCameraActor->Set_HitPos(hit.worldPos);
		m_pCameraActor->Set_HitNormal(hit.worldNormal);
	}
}

void CCameraHitReport::onControllerHit(const PxControllersHit & hit)
{
}

void CCameraHitReport::onObstacleHit(const PxControllerObstacleHit & hit)
{
}