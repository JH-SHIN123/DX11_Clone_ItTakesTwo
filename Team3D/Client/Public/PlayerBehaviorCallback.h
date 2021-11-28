#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CPlayerBehaviorCallback : public PxControllerBehaviorCallback
{
public:
	CPlayerBehaviorCallback() = default;
	~CPlayerBehaviorCallback() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape & shape, const PxActor & actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController & controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle & obstacle) override;

public:
	void Set_PlayerActorPtr(class CPlayerActor* pPlayerActor) { m_pPlayerActor = pPlayerActor; }
	void Set_Controller(PxController* pController) { m_pController = pController; }

private:
	class CPlayerActor* m_pPlayerActor = nullptr;
	PxController* m_pController = nullptr;
};

END