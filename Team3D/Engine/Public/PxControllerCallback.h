#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPxControllerCallback : public PxControllerBehaviorCallback
{
public:
	CPxControllerCallback() = default;
	~CPxControllerCallback() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape & shape, const PxActor & actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController & controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle & obstacle) override;
};

END