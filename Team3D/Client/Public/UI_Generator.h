#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIObject.h"

class CUI_Generator final : public CBase
{
	DECLARE_SINGLETON(CUI_Generator)

private:
	explicit CUI_Generator();
	virtual ~CUI_Generator() = default;

public:
	HRESULT Set_Device(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);

public:
	HRESULT Load_Data(const _tchar* pFilePath);
	HRESULT Generator_UI(UI::TRIGGER eTrigger);

private:
	HRESULT Add_Prototype_Interactive_UI(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Fixed_UI(CUIObject::UI_DESC UIDesc);
	HRESULT Add_Prototype_Texture();

private:
	vector<CUIObject::UI_DESC*> m_vecPSData;


private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

public:
	virtual void Free() override;

};
