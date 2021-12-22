#pragma once

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	explicit CLevel_Manager() = default;
	virtual ~CLevel_Manager() = default;

public: /* Getter */
	const _uint Get_CurrentLevelStep();

public:
	HRESULT	Change_CurrentLevel(class CLevel* pCurrentLevel);
	_int	Tick(_double dTimedelta);
	HRESULT	Render();
	HRESULT Clear_Level();

private:
	class CLevel* m_pCurrentLevel = nullptr;

public:
	virtual void Free() override;
};

END