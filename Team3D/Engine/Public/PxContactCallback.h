#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPxContactCallback final : public PxContactModifyCallback
{
public:
	CPxContactCallback() = default;
	~CPxContactCallback() = default;

public:
	virtual void onContactModify(PxContactModifyPair * const pairs, PxU32 count) override;
};

END