#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CPlayerHitReport final : public PxUserControllerHitReport
{
public:
	CPlayerHitReport() = default;
	~CPlayerHitReport() = default;

public:
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

public:
	void Set_PlayerActorPtr(class CPlayerActor* pPlayerActor) { m_pPlayerActor = pPlayerActor; }

private:
	class CPlayerActor* m_pPlayerActor = nullptr;
};

END