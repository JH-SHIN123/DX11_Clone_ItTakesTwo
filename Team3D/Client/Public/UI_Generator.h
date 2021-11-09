#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CUI_Generator final : public CBase
{
	DECLARE_SINGLETON(CUI_Generator)

private:
	explicit CUI_Generator();
	virtual ~CUI_Generator() = default;

public:
	HRESULT Load_Data(const _tchar* pFilePath);
	HRESULT Generator_UI(UI::TRIGGER eTrigger);

public:
	virtual void Free() override;

};
