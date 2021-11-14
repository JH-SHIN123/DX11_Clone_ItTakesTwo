#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEnvironment_Generator final : public CBase
{
	DECLARE_SINGLETON(CEnvironment_Generator)

private:
	explicit CEnvironment_Generator();
	virtual ~CEnvironment_Generator() = default;

public:
	HRESULT Load_Environment_Prototype();
	HRESULT Load_Environment_Clone(const _tchar* pLayerTag);

private:
	CGameInstance*	m_pGameInstance = nullptr;

public:
	virtual void Free() override;
};

END