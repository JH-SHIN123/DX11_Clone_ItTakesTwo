#pragma once

#ifndef __EFFECT_PLAYER_LAIL_H__

#include "InGameEffect.h"
BEGIN(Client)
class CEffect_Player_Lail :
	public CInGameEffect
{
public:
	CEffect_Player_Lail();
	virtual ~CEffect_Player_Lail();
};

END
#define __EFFECT_PLAYER_LAIL_H__
#endif // !__EFFECT_PLAYER_LAIL_H__
