#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

class CCameraBehaviorCallback : public PxControllerBehaviorCallback
{
public:
	CCameraBehaviorCallback() = default;
	~CCameraBehaviorCallback() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape & shape, const PxActor & actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController & controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle & obstacle) override;

public:
	void Set_CameraActorPtr(class CCameraActor* pCameraActor) { m_pCameraActor = pCameraActor; }

private:
	class CCameraActor* m_pCameraActor = nullptr;
};

END