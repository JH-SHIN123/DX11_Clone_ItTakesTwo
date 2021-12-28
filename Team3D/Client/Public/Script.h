#pragma once
#include "GameObject.h"
#include "ScriptBackGround.h"

#include <queue>

BEGIN(Client)
#define SCRIPT ((CScript*)(DATABASE->Get_Script()))
class CScript final : public CGameObject
{
public:
	enum SCREEN	 { FULL, HALF, SCREEN_END };

	typedef struct tagScriptInfo
	{
		_uint		iScriptIndex;
		_float		dChangeTime;
		SCREEN		eScreen;

		tagScriptInfo() {}
		tagScriptInfo(_uint iScriptIndex, _float dChangeTime, SCREEN eScreen = SCREEN::HALF) : iScriptIndex(iScriptIndex), dChangeTime(dChangeTime), eScreen(eScreen) {}
	}SCRIPTINFO;

private:
	explicit CScript(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CScript(const CScript& rhs);
	virtual ~CScript() = default;

public:
	/* 스크립트 인덱스, 화면비율, 삭제속도, 백그라운드 유무 */
	HRESULT Render_Script(_uint iScriptIndex, SCREEN eScreenMode, _float fDeadTime, _bool bBackGround = true);
	/* 두줄이상출력시 사용 *Only FullScreen */
	HRESULT Render_Script_DoubleLine(_uint iIndexFirst, _uint iIndexSecond, _float fDeadTime, _bool bBackGround = true);

public:
	/* Voice 파일 재생과 사운드에 맞춰 스크립트가 자동생성 */
#pragma region Voice
	void VoiceFile_No01();
	void VoiceFile_No02();
	void VoiceFile_No03();
	void VoiceFile_No04();
	void VoiceFile_No05();
	void VoiceFile_No06();
	void VoiceFile_No07();
	void VoiceFile_No08();
	void VoiceFile_No09();
	void VoiceFile_No10();
	void VoiceFile_No11();
	void VoiceFile_No12();
	void VoiceFile_No13();
	void VoiceFile_No14();
	void VoiceFile_No15();
	void VoiceFile_No16();
	void VoiceFile_No17();
	void VoiceFile_No18();
	void VoiceFile_No19();
	void VoiceFile_No20();
	void VoiceFile_No21();
	void VoiceFile_No22();
	void VoiceFile_No23();
	void VoiceFile_No24();
	void VoiceFile_No25();
	void VoiceFile_No26();
	void VoiceFile_No27();
	void VoiceFile_No28();
	void VoiceFile_No29();
	void VoiceFile_No30();
	void VoiceFile_No31();
	void VoiceFile_No32();
	void VoiceFile_No33();
	void VoiceFile_No34();
	void VoiceFile_No35();
	void VoiceFile_No36();
	void VoiceFile_No37();
	void VoiceFile_No38();
	void VoiceFile_No39();
	void VoiceFile_No40();
	void VoiceFile_No41();
	void VoiceFile_No42();
	void VoiceFile_No43();
	void VoiceFile_No44();
	void VoiceFile_No45();
	void VoiceFile_No46();
	void VoiceFile_No47();
	void VoiceFile_No48();
	void VoiceFile_No49();
	void VoiceFile_No50();
	void VoiceFile_No51();
	void VoiceFile_No52();
	void VoiceFile_No53();
	void VoiceFile_No54();
	void VoiceFile_No55();
#pragma endregion

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup);

private:
	CRenderer*			m_pRendererCom = nullptr;
	CFontDraw*			m_pFontCom_First = nullptr;
	CFontDraw*			m_pFontCom_Second = nullptr;

	CScriptBackGound*	m_pBackGround_First = nullptr;
	CScriptBackGound*	m_pBackGround_Sub = nullptr;

	SCREEN				m_eScreenMode = SCREEN_END;

	_uint				m_iIndexFirst = 0;
	_uint				m_iIndexSecond = 0;

	_float				m_fDeadCheck = 0.f;
	_double				m_dDeadTime = 0.0;
	_bool				m_bRender = false;
	_bool				m_isDouble = false;
	_bool				m_bBackGround = false;

	_bool				m_bVoice = false;
	_float				m_fChangeTimeCheck = 0.f;
	_double				m_dChangeTime = 0.0;
	queue<SCRIPTINFO>	m_queueScriptInfo;

private:
	_tchar* Find_Script(_uint iIndex);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END