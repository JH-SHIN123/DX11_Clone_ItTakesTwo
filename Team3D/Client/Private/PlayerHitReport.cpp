#include "stdafx.h"
#include "..\Public\PlayerHitReport.h"
#include "PlayerActor.h"

void CPlayerHitReport::onShapeHit(const PxControllerShapeHit & hit)
{
	USERDATA* pUserData = (USERDATA*)hit.actor->userData;

	if (nullptr == pUserData) return;

	if (GameID::eGRAVITYPATH_CENTER == pUserData->eID && (CPlayerActor::PLAYER_MAY == m_pPlayerActor->Get_Player_Type()))
	{
		if (hit.triangleIndex != PxU32(-1))
		{
			m_pPlayerActor->Set_ReorderGravityStep(1);
			m_pPlayerActor->Set_HitNormal(hit.worldNormal);
		}
	}
	if ((GameID::eDUMMYWALL == pUserData->eID || GameID::ePIPEJUMPWALL == pUserData->eID || GameID::eELECTRICWALL == pUserData->eID) && (CPlayerActor::PLAYER_CODY == m_pPlayerActor->Get_Player_Type()))
	{
		if (hit.triangleIndex != PxU32(-1))
		{
			m_pPlayerActor->Set_ContactPos(hit.worldPos);
			m_pPlayerActor->Set_WallCollide(true);
			m_pPlayerActor->Set_CollideNormal(hit.worldNormal);
		}
	}
	if (GameID::eMAYJUMPWALL == pUserData->eID && (CPlayerActor::PLAYER_MAY == m_pPlayerActor->Get_Player_Type()))
	{
		if (hit.triangleIndex != PxU32(-1))
		{
			m_pPlayerActor->Set_ContactPos(hit.worldPos);
			m_pPlayerActor->Set_WallCollide(true);
			m_pPlayerActor->Set_CollideNormal(hit.worldNormal);
		}
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