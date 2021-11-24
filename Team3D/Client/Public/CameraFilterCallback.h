#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

class CCameraFilterCallback final : public PxControllerFilterCallback
{
public:
	CCameraFilterCallback() = default;
	~CCameraFilterCallback() = default;

public:
	virtual bool filter(const PxController & a, const PxController & b) override;

public:
	void Set_Controller(PxController* pController) { m_pController = pController; }

private:
	PxController* m_pController = nullptr;
};

END