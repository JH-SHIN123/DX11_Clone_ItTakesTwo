#pragma once

#include "Base.h"
#include "Ortho_UIObject.h"


class CUI_Loader final : public CBase
{
	DECLARE_SINGLETON(CUI_Loader)

public:
	enum UI_TRIGGER { UI_MENU, UI_FRAME_CIRCLE, UI_FRAME_RECT, UI_FRAME_END };

private:
	explicit CUI_Loader();
	virtual ~CUI_Loader() = default;

public:
	HRESULT LoadData_UI(const _tchar* pFilePath);

public:
	HRESULT Create_UI(UI_TRIGGER eTrigger);

private:
	typedef unordered_map<const _tchar*, vector<COrtho_UIObject::UI_DESC>>			UIOBJECTS;
	UIOBJECTS																		m_UIObjects;

public:
	virtual void Free() override;

};
