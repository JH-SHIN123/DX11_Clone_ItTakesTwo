#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIObject.h"
#include "GameInstance.h"

BEGIN(ENGINE)
class CTextures;
class CVIBuffer_FontInstance;
END

BEGIN(Client)

#define UI_Create(ePlayer, eTrigger)															\
	CUI_Generator::GetInstance()->Set_TriggerOn();												\
	CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger);					\

#define  UI_Generator CUI_Generator::GetInstance()

class CUI_Generator final : public CBase
{
	DECLARE_SINGLETON(CUI_Generator)

public:
	typedef struct tagFontDesc
	{
		_float2 vPosition = { 0.f, 0.f };
		_float2 vScale = { 100.f, 100.f };
		_float  fInterval = 0.f;
	}FONTDESC;

private:
	explicit CUI_Generator();
	virtual ~CUI_Generator() = default;

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);

public:
	HRESULT Load_Data(const _tchar* pFilePath);
	HRESULT Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger);
	HRESULT Delete_UI(Player::ID ePlayer, UI::TRIGGER eTrigger);
	HRESULT Render_Font(_tchar* pText, FONTDESC tFontDesc, Player::ID ePlayer);

public:
	void Set_TriggerOn();

private:
	_bool							m_IsTrigger = true;
	VTXFONT*						m_VTXFONT;
	FONTDESC						m_FontDesc;

private:
	vector<CUIObject::UI_DESC*>		m_vecPSData;
	vector<CUIObject*>				m_vecUIOBjects[Player::PLAYER_END][UI::TRIGGER_END];

private:
	CTextures*						m_pTexturesCom = nullptr;
	CVIBuffer_FontInstance*			m_pVIBuffer_FontCom = nullptr;

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pDeviceContext = nullptr;

private:
	HRESULT Add_Prototype_Interactive_UI(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Fixed_UI(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Texture();

private:
	HRESULT SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar* PrototypeTag);

public:
	virtual void Free() override;

};

END
