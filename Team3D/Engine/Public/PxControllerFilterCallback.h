#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPxControllerFilterCallback final : public PxControllerFilterCallback
{
public:
	CPxControllerFilterCallback() = default;
	~CPxControllerFilterCallback() = default;

public:
	virtual bool filter(const PxController & a, const PxController & b) override;
};

END