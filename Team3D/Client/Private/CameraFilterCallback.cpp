#include "stdafx.h"
#include "..\Public\CameraFilterCallback.h"

bool CCameraFilterCallback::filter(const PxController & a, const PxController & b)
{
	USERDATA* pUserDataA = (USERDATA*)(a.getUserData());
	USERDATA* pUserDataB = (USERDATA*)(b.getUserData());

	if (nullptr == pUserDataA || nullptr == pUserDataB)
		return false;

	if (pUserDataA->eID == GameID::eMAINCAMERA && pUserDataB->eID == GameID::eCODY)
		return true;
	else if (pUserDataA->eID == GameID::eCODY && pUserDataB->eID == GameID::eMAINCAMERA)
		return true;
	else if (pUserDataA->eID == GameID::eSUBCAMERA && pUserDataB->eID == GameID::eMAY)
		return true;
	else if (pUserDataA->eID == GameID::eMAY && pUserDataB->eID == GameID::eSUBCAMERA)
		return true;

	return false;
}