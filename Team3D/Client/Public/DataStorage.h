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
	class CPlayer* Player;

public:
	virtual void Free() override;
};

END