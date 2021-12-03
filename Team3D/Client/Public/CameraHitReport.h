#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CCameraHitReport final : public PxUserControllerHitReport
{
public:
	CCameraHitReport() = default;
	~CCameraHitReport() = default;

public:
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

public:
	void Set_CameraActorPtr(class CCameraActor* pCameraActor) { m_pCameraActor = pCameraActor; }

private:
	class CCameraActor* m_pCameraActor = nullptr;
};

END