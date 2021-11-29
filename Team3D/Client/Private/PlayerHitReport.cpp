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
			m_pPlayerActor->Step_GravityPath(hit.worldNormal);
		}
	}
	else if (GameID::ePLANET == pUserData->eID)
	{
		if (hit.triangleIndex != PxU32(-1))
		{
			m_pPlayerActor->Set_ContactPos(hit.worldPos);
		}
	}
}

void CPlayerHitReport::onControllerHit(const PxControllersHit & hit)
{
}

void CPlayerHitReport::onObstacleHit(const PxControllerObstacleHit & hit)
{
}