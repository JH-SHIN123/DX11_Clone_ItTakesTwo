#pragma once
#include "Base.h"
class CDataBase :public CBase
{

	DECLARE_SINGLETON(CDataBase)
public:
	CDataBase();
	virtual ~CDataBase() = default;



public:
	void Set_PlayerPtr(class CGameObject* pPlayer) { m_pPlayer = pPlayer; }
	CGameObject* GetPlayer() { return m_pPlayer; }
private:
	class CGameObject* m_pPlayer = nullptr;

public:
	virtual void Free();
};

