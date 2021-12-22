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

public: /* Getter */
	_bool Get_Dead_Environment() { return m_bDeadEnvironment; }
	_uint Get_RandomModel() { return m_iRandomModel; }

public: /* Setter */
	void Set_Dead_Environment() { m_bDeadEnvironment = true; }

public:
	HRESULT NativeConstruct_EndingCredit();
	HRESULT Create_3DText(_uint iIndex, _float fPosY);
	HRESULT Create_Environment();

private:
	CGameInstance*	m_pGameInstance = nullptr;
	_uint			m_iTextIndex = 0;
	_bool			m_bDeadEnvironment = false;
	_uint			m_iRandomModel = 0;
private:
	HRESULT Create_Rocks(_uint iNumRock);
	HRESULT Create_HugeRock();
	HRESULT Create_2DMesh(_uint iNum2DMesh);

public:
	virtual void Free() override;
};
END