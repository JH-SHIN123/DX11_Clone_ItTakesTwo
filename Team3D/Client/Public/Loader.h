#pragma once

#include "Base.h"

class CLoader final : public CBase
{
	DECLARE_SINGLETON(CLoader)

private:
	explicit CLoader();
	virtual ~CLoader() = default;

public:
	HRESULT LoadData_UI(const _tchar* pFilePath);


public:
	virtual void Free() override;

};
