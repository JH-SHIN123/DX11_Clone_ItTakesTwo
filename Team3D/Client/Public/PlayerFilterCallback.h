#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CPlayerFilterCallback final : public PxControllerFilterCallback
{
public:
	CPlayerFilterCallback() = default;
	~CPlayerFilterCallback() = default;

public:
	virtual bool filter(const PxController & a, const PxController & b) override;
};

END