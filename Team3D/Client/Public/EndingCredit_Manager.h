#pragma once
#include "Client_Defines.h"
#include "C3DText.h"

BEGIN(Client)

#define ENDINGCREDIT CEndingCredit_Manager::GetInstance()

class CEndingCredit_Manager final : public CBase
{
	DECLARE_SINGLETON(CEndingCredit_Manager)

private:
	explicit CEndingCredit_Manager();
	virtual ~CEndingCredit_Manager() = default;

public:
	/* 충돌여부(부스트여부) */
	HRESULT Create_3DText(_bool bBoost);
	HRESULT Start_EndingCredit();
	HRESULT Create_Environment();

public:
	HRESULT NativeConstruct_EndingCredit();

private:
	CGameInstance*	m_pGameInstance = nullptr;

	_uint		m_iTextIndex = 0;

private:
	void Add_Argument_Info(_uint iIndex, C3DText::ARG_DESC &tArg);
	HRESULT Create_Rocks(_uint iNumRock);
	HRESULT Create_2DMesh(_uint iNum2DMesh);

public:
	virtual void Free() override;
};
END