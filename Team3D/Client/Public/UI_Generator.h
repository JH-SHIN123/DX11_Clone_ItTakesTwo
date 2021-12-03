#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIObject.h"

 BEGIN(Client)

#define UI_Create(ePlayer, eTrigger)															\
	CUI_Generator::GetInstance()->Set_TriggerOn();												\
	CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger);					\

#define UI_CreateOnlyOnce(ePlayer, eTrigger)													\
	if(true == CUI_Generator::GetInstance()->Get_EmptyCheck(Player::ePlayer, UI::eTrigger))	{	\
		CUI_Generator::GetInstance()->Set_TriggerOn();											\
		CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger);				\
	}																							\

#define UI_Generator CUI_Generator::GetInstance()
#define UI_Delete(ePlayer, eTrigger) CUI_Generator::GetInstance()->Delete_UI(Player::ePlayer, UI::eTrigger);

class CUI_Generator final : public CBase
{
	DECLARE_SINGLETON(CUI_Generator)

public:
	typedef struct tagFontDesc
	{
		_float2 vPosition = { 0.f, 0.f };
		_float2 vScale = { 100.f, 100.f };
		_uint	iShaderPassNum = 0;
		_float	fAlpha = 1.f;
		_float3 vColor = { 1.f, 1.f, 1.f };


	}FONTDESC;

private:
	explicit CUI_Generator();
	virtual ~CUI_Generator() = default;

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);

public:
	HRESULT Load_Data(const _tchar* pFilePath, Level::ID eLevel, _uint iOption = 0);
	HRESULT Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger);
	HRESULT Delete_UI(Player::ID ePlayer, UI::TRIGGER eTrigger);
	HRESULT Render_Font(_tchar* pText, FONTDESC tFontDesc, Player::ID ePlayer);
	HRESULT Render_AlphaFont(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer);

public:
	CUIObject* Get_UIObject(Player::ID ePlayer, UI::TRIGGER eTrigger) { return m_vecUIOBjects[ePlayer][eTrigger].front(); };
	_bool Get_EmptyCheck(Player::ID ePlayer, UI::TRIGGER eTrigger) { return m_vecUIOBjects[ePlayer][eTrigger].empty(); };
	class CHeaderBox* Get_HeaderBox(_int iIndex) { if (true == m_vecHeaderBox.empty()) return nullptr; return m_vecHeaderBox[iIndex]; }

public:
	void Set_TriggerOn();
	void Set_TargetPos(Player::ID ePlayer, UI::TRIGGER eTrigger, _vector vTargetPos);
	void Set_ScaleEffect(Player::ID ePlayer, UI::TRIGGER eTrigger);

public:
	HRESULT Add_Prototype_LogoTexture();

public:
	HRESULT Create_Logo();
	HRESULT Create_ChapterSelect();

public:
	/* 타겟 오브젝트와 플레이어의 범위에 따라 InterActive UI생성 삭제 해주는 함수 */
	void CreateInterActiveUI_AccordingRange(Player::ID ePlayer, UI::TRIGGER eTrigger, _vector vTargetPosition);

	
public:
	void UI_RETutorial(Player::ID ePlayer, UI::TRIGGER eTrigger);

private:
	_bool							m_IsTrigger = true;
	_float							m_fFontAlpha = 1.f;
	_float							m_fTime = 0.f;
	_float							m_fChange = 1;
	VTXFONT*						m_VTXFONT;
	FONTDESC						m_FontDesc;


private:
	vector<CUIObject::UI_DESC*>		m_vecPSData;
	vector<CUIObject*>				m_vecUIOBjects[Player::PLAYER_END][UI::TRIGGER_END];
	vector<class CHeaderBox*>		m_vecHeaderBox;

private:
	CTextures*						m_pTexturesCom = nullptr;
	CTextures*						m_pEngTexturesCom = nullptr;
	CVIBuffer_FontInstance*			m_pVIBuffer_FontCom = nullptr;
	CVIBuffer_Rect*					m_pVIBuffer_Rect = nullptr;

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pDeviceContext = nullptr;

private:
	HRESULT Add_Prototype_Interactive_UI(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Fixed_UI(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Menu(CUIObject::UI_DESC* UIDesc);
	HRESULT Add_Prototype_Chapter(CUIObject::UI_DESC* UIDesc);

private:
	HRESULT Add_Prototype_Texture();
	
private:
	HRESULT SetUp_Clone(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar* PrototypeTag, Level::ID eLevel, void* pArg = nullptr);
	HRESULT SetUp_Clone_Ptr(Player::ID ePlayer, UI::TRIGGER eTrigger, const _tchar * PrototypeTag, Level::ID eLevel, void * pArg, CGameObject** pGameObject);

public:
	virtual void Free() override;

};

END