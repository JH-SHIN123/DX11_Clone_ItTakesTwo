#pragma once
#include "Client_Defines.h"

BEGIN(Client)

#define ENDINGCREDIT CEndingCredit_Manager::GetInstance()

class CEndingCredit_Manager final : public CBase
{
	DECLARE_SINGLETON(CEndingCredit_Manager)

private:
	explicit CEndingCredit_Manager();
	virtual ~CEndingCredit_Manager() = default;

public:
	/* 충돌한 텍스트의 좌표, 충돌여부(부스트여부)*/
	HRESULT Create_3DText(_vector vPosition, _bool bBoost);

public:
	HRESULT NativeConstruct_EndingCredit();

private:
	CGameInstance*	m_pGameInstance = nullptr;

	_uint		m_iTextIndex = 0;

public:
	virtual void Free() override;
};
END