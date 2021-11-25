#pragma once

#include"Client_Defines.h"
#include"Base.h"

BEGIN(Client)
class CCutScene : CBase
{
public:
	CCutScene();
	~CCutScene();

	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END