#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPxBroadCallback final : public PxBroadPhaseCallback
{
public:
	CPxBroadCallback() = default;
	~CPxBroadCallback() = default;

public:
	virtual void onObjectOutOfBounds(PxShape & shape, PxActor & actor) override;
	virtual void onObjectOutOfBounds(PxAggregate & aggregate) override;
};

END