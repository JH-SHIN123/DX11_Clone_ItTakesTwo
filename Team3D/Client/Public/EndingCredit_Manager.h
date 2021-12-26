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
	_bool Get_2DMeshStart() { return m_b2DMeshStart; }

public: /* Setter */
	void Set_Dead_Environment() { m_bDeadEnvironment = true; }
	void Set_RandomModel();
	void Set_2DMeshStart() { m_b2DMeshStart = true; }

public:
	HRESULT NativeConstruct_EndingCredit();
	HRESULT Create_3DText(_uint iIndex, _float fPosY);
	HRESULT Create_Environment();
	HRESULT Create_HugeRock(_float fPosY);
	HRESULT Create_WhiteOut();

	void End_EndingCredit();

private:
	CGameInstance*	m_pGameInstance = nullptr;

	_bool			m_bDeadEnvironment = false;
	_bool			m_b2DMeshStart = false;
	
	_uint			m_iTextIndex = 0;
	_uint			m_iRandomModel = 0;

private:
	HRESULT Create_Rocks(_uint iNumRock);
	HRESULT Create_2DMesh(_uint iNum2DMesh);

public:
	virtual void Free() override;
};
END