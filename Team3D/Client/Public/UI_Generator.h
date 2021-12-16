#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIObject.h"

BEGIN(Client)

/* 같은 UI 여러 개 생성할 수 있음 아니면 걍 트리거에서 호출 해주세용*/
#define UI_Create(ePlayer, eTrigger)														\
	CUI_Generator::GetInstance()->Set_TriggerOn();											\
	CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger);				\

/* bActive 랜더 끌건지 안끌건지 */
#define UI_Create_Active(ePlayer, eTrigger, bActive)										\
	CUI_Generator::GetInstance()->Set_TriggerOn();											\
	CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger, bActive);		\

/* Tick LateTick에서 그냥 조건없이 한번만 생성해주는 애 대신 같은 UI 여러 개 생성할꺼면 얘 쓰면 안됩니다 */
#define UI_CreateOnlyOnce(ePlayer, eTrigger)													\
	if(true == CUI_Generator::GetInstance()->Get_EmptyCheck(Player::ePlayer, UI::eTrigger))	{	\
		CUI_Generator::GetInstance()->Set_TriggerOn();											\
		CUI_Generator::GetInstance()->Generator_UI(Player::ePlayer, UI::eTrigger);				\
	}																							\

#define UI_AccordingRange(ePlayer, eTrigger)													\
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
	HRESULT Generator_UI(Player::ID ePlayer, UI::TRIGGER eTrigger, void* pArg = nullptr);
	HRESULT Generator_InterActive_UI(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, void* pArg = nullptr);
	HRESULT Generator_InterActive_SwingPoint(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, void * pArg = nullptr);
	HRESULT Delete_UI(Player::ID ePlayer, UI::TRIGGER eTrigger);
	HRESULT Delete_InterActive_UI(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger);
	HRESULT Render_Font(_tchar* pText, FONTDESC tFontDesc, Player::ID ePlayer);
	HRESULT Render_AlphaFont(_tchar * pText, FONTDESC tFontDesc, Player::ID ePlayer);

public:
	_bool Get_EmptyCheck(Player::ID ePlayer, UI::TRIGGER eTrigger) { return m_vecUIOBjects[ePlayer][eTrigger].empty(); };
	_bool Get_InterActive_UI_EmptyCheck(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger) { return m_vecInterActiveUI[ePlayer][eTrigger].empty(); };
	class CHeaderBox* Get_HeaderBox(_int iIndex) { if (true == m_vecHeaderBox.empty()) return nullptr; return m_vecHeaderBox[iIndex]; }
	CUIObject* Get_UIObject(Player::ID ePlayer, UI::TRIGGER eTrigger);

public:
	void Set_TriggerOn();
	void Set_TargetPos(Player::ID ePlayer, UI::TRIGGER eTrigger, _vector vTargetPos);
	void Set_InterActive_TargetPos(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, _vector vTargetPos);
	void Set_Active(Player::ID ePlayer, UI::TRIGGER eTrigger, _bool bActive);
	void Set_ScaleEffect(Player::ID ePlayer, UI::TRIGGER eTrigger);
	void Set_FadeInSpeed(Player::ID ePlayer, UI::TRIGGER eTrigger, _float fSpeed);
	void Set_FadeOutSpeed(Player::ID ePlayer, UI::TRIGGER eTrigger, _float fSpeed);
	void Set_FadeOut(Player::ID ePlayer, UI::TRIGGER eTrigger);

public:
	HRESULT Add_Prototype_LogoTexture();

public:
	HRESULT Create_Logo();
	HRESULT Create_ChapterSelect();

public:
	/* 1. INTERACTIVE_ID에 해당 객체 ID 추가 */
	/* 2. IsCollision은 객체 Trigger 함수 안에서 사용하는 bool 변수 인자로 던져주면 됨 */
	/* 3. fRange는 범위 설정하는거 범위 설정 안해주면 UI 안뜸 */
	/* 4. IsDisable은 그 객체와 플레이어가 상호작용 시작할 때 bool 값 하나 던져주면 됨 */
	/* 5. 상호작용 시작할 때 IsDisable true로 던지고 조건에 맞았다하면 걍 납두고 조건에 안맞아서 다시 상호작용해야 될 때는 IsDisable false로 던져주면 알아서 다시 상호작용 UI 생성해줌 */
	/* 6. 상호작용 키를 눌렀을 때 UI_Delete 한 번 호출 해줘야함!!!!!!! */
	HRESULT CreateInterActiveUI_AccordingRange(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, _vector vTargetPosition, _float fRange = 0.f, _bool IsCollision = false, _bool IsDisable = false);

public:
	void UI_RETutorial(Player::ID ePlayer, UI::TRIGGER eTrigger);

private:
	_bool							m_IsTrigger = true;
	_bool							m_IsInRange = false;
	_float							m_fFontAlpha = 1.f;
	_float							m_fTime = 0.f;
	_float							m_fChange = 1;
	VTXFONT*						m_VTXFONT;
	FONTDESC						m_FontDesc;

private:
	vector<CUIObject::UI_DESC*>		m_vecPSData;
	vector<CUIObject*>				m_vecUIOBjects[Player::PLAYER_END][UI::TRIGGER_END];
	vector<CUIObject*>				m_vecInterActiveUI[Player::PLAYER_END][UI::INTERACTIVE_ID_END];
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
	HRESULT SetUp_Clone_InterActive(Player::ID ePlayer, UI::INTERACTIVE_ID eTrigger, const _tchar * PrototypeTag, Level::ID eLevel, void * pArg);

public:
	virtual void Free() override;

};

END