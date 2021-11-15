#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

#define DATABASE CDataStorage::GetInstance()

class CDataStorage final : public CBase
{
	DECLARE_SINGLETON(CDataStorage)
private:
	explicit CDataStorage() = default;
	virtual ~CDataStorage() = default;


public:
	void	Set_Player(class CPlayer* pPlayer) { Player = pPlayer; }
	class CPlayer* Get_Player() { return Player; }

public:
	class CPlayer* Player;

public:
	virtual void Free() override;
};

END