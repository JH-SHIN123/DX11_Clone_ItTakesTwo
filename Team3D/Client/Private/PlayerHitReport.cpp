#include "stdafx.h"
#include "..\Public\PlayerHitReport.h"
#include "PlayerActor.h"

void CPlayerHitReport::onShapeHit(const PxControllerShapeHit & hit)
{
	USERDATA* pUserData = (USERDATA*)hit.actor->userData;

	if (nullptr == pUserData) return;

	if (GameID::eGRAVITYPATH_CENTER == pUserData->eID)
	{
		if (hit.triangleIndex != PxU32(-1))
		{
			m_pPlayerActor->Set_ReorderGravityStep(1);
			m_pPlayerActor->Set_HitNormal(hit.worldNormal);
		}
		//
		//
	}
	//else if (GameID::ePLANET == pUserData->eID)
	//{
	//	m_pPlayerActor->MoveToTarget(hit.actor->getGlobalPose());
	//}
}

void CPlayerHitReport::onControllerHit(const PxControllersHit & hit)
{
}

void CPlayerHitReport::onObstacleHit(const PxControllerObstacleHit & hit)
{
}